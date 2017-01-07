//implement the expense class
//Created by Theodore Yang on 1/4/2017

#include <iostream>
#include <set>
#include <sstream>
#include "Expense.h"
namespace {
    constexpr const char* default_note = "no notes";

    constexpr int weight_upper_limit = 999;

    enum CommitType {
        WeightChange,
        AddPartic,
        RemovePartic
    };
}

namespace AccountBalancer {
    //here we consider two types of commit, weight change and participant change
    //both of them can be modeled as [person, weightBefore, weightAfter]
    struct ExpenseCommit {
        using singleDiff = std::pair<std::string, std::pair<int, int>>;

        CommitType type;
        std::vector<singleDiff> diffs;

        ExpenseCommit(CommitType _type): type(_type) {}

        ExpenseCommit(CommitType _type, const std::vector<singleDiff>& _diffs):
            type(_type),
            diffs(_diffs) {}
        ExpenseCommit(CommitType _type, std::vector<singleDiff>&& _diffs):
            type(_type),
            diffs(std::move(_diffs)) {}
    };

    inline void printExpenseCommit(const ExpenseCommit& commit, bool verbose) {

        if (commit.type == CommitType::WeightChange) 
            std::cout << "weight changes";
        else if (commit.type == CommitType::AddPartic) 
            std::cout << "add participant";
        else if (commit.type == CommitType::RemovePartic)
            std::cout << "remove participant";
        else 
            std::cout << "undefined commit";


        if (verbose) {
            std::cout << ": " << std::endl;
            if (commit.type == CommitType::WeightChange) {
                for (auto& diff: commit.diffs) {
                    std::cout << "  " << diff.first << "(" << diff.second.first
                        << " -> " << diff.second.second << ")" << std::endl;
                }
            }
            else if (commit.type == CommitType::AddPartic) {
                for (auto it = commit.diffs.begin(); ; ++it) {
                    if (it ==commit.diffs.end()) {
                        std::cout << it->first << std::endl;
                        break;
                    }
                    else {
                        std::cout << it->first << ", ";
                    }
                }
            }
            else if (commit.type == CommitType::RemovePartic) {
                for (auto it = commit.diffs.begin(); ; ++it) {
                    if (it ==commit.diffs.end()) {
                        std::cout << it->first << std::endl;
                        break;
                    }
                    else {
                        std::cout << it->first << ", ";
                    }
                }
            }
            else {
                std::cout << "undefined commit" << std::endl;
            }
        }
        std::cout << std::endl;
    }


    //--------------------------Expense---------------------------
    //ctor
    Expense::Expense(const std::string& _creditor, 
            double _amount):
        creditor(_creditor),
        amount(_amount), 
        note(default_note),
        total_weight(0){}

    Expense::Expense(const std::string& _creditor,
            double _amount,
            const std::string& _note):
        creditor(_creditor),
        amount(_amount),
        note(_note),
        total_weight(0){}

    //dtor
    Expense::~Expense() = default;

    //accessors
    int Expense::numOfParticipants() const noexcept {
        return weights.size();
    }

    bool Expense::hasParticipant(const std::string& name) const {
        return weights.find(name) != weights.end();
    }

    double Expense::getAmount() const {
        return amount;
    }

    int Expense::getWeight(const std::string& name) const {
        return weights.at(name);
    }

    int Expense::getWeightSum() const {
        return total_weight;
    }

    std::string Expense::getCreditor() const {
        return creditor;
    }

    std::string Expense::getNote() const {
        return note;
    }

    const std::map<std::string, int>& Expense::getWeightsMap() const {
        return weights;
    }

    void Expense::printCommitsHistory(bool verbose) const {
        for (auto& commit: commit_hist) {
            printExpenseCommit(*commit, verbose);
        }
    }

    void Expense::printExpenseSummary() const {
        printf("%s\n", note.c_str());
        printf("Creditor:  %25s\n", creditor.c_str());
        printf("Amount:  %27.2f\n", amount);
        std::vector<std::string> weights = formatWeightsString();
        printf("Shared by:  %-42s\n", weights[0].c_str());
        for (int i = 1; i < weights.size(); ++i) {
            printf("            %-42s\n", weights[i].c_str());
        }
    }

    //modifiers
    void Expense::setNote(std::string _note) noexcept {
        note = std::move(_note);
    }

    void Expense::setAmount(double _amount) noexcept {
        amount = _amount;
    }

    void Expense::addParticipant(const std::vector<std::string>& names) {
        auto commit_ptr(std::make_unique<ExpenseCommit>(CommitType::AddPartic));
        for (auto& name: names) {
            if (weights.find(name) == weights.end()) {
                weights[name] = 1;
                commit_ptr->diffs.push_back(std::make_pair(name, std::make_pair(0, 1)));
                total_weight += 1;
            }
            else {
                if (verbose) {
                    std::cerr << "ignore " << name << " for it is already in the participants list" << std::endl;
                }
            }
        }
        commit_hist.push_back(std::move(commit_ptr));
    }

    void Expense::removeParticipant(const std::vector<std::string>& names) {
        auto commit_ptr(std::make_unique<ExpenseCommit>(CommitType::RemovePartic));
        for (auto& name: names) {
            if (weights.find(name) != weights.end()) {
                commit_ptr->diffs.push_back(std::make_pair(name, 
                            std::make_pair(weights[name], 0)));
                total_weight -= weights[name];
                if (verbose)
                    std::cout << "remove " << name << " as a participant" << std::endl;
                weights.erase(name);
            }
            else {
                if (verbose)
                    std::cerr << "ignore " << name << " for it's not in the participants list" << std::endl;
            }
        }
        commit_hist.push_back(std::move(commit_ptr));
        //make sure there is at least one participant
        if (weights.empty()) {
            std::cerr << "at least one participant has to show up" << std::endl;
            rollBack();
        }
    }

    void Expense::changeWeights(const std::vector<std::pair<std::string, int>>& change_list) {
        auto commit_ptr (std::make_unique<ExpenseCommit>(CommitType::WeightChange));
        //check the weight change, make sure it is legal, otherwise, stop and roll back
        //if it is leg
        for (auto& change: change_list) {
            int before = weights[change.first];
            int after = change.second;
            if (after < 0 || after > weight_upper_limit) {
                std::cerr << change.first << "'s share weight can not be negative" << std::endl;
                std::cerr << "aborted" << std::endl;
                rollBack(*commit_ptr);
                break;
            }
            total_weight += (after - before);
            commit_ptr->diffs.push_back(std::make_pair(change.first,
                        std::make_pair(before, after)));
            if (!after) {
                if (verbose)
                    std::cout << "remove " << change.first << " as a participant" << std::endl;
                weights.erase(change.first);
            }
            else
                weights[change.first] = after;
        }
        commit_hist.push_back(std::move(commit_ptr));
    }

    void Expense::rollBack() {
        if (commit_hist.empty()) {
            std::cout << "already in the initial commit" << std::endl;
        }
        else {
            auto& commit_ptr = commit_hist.back();
            commit_hist.pop_back();
            rollBack(*commit_ptr);
        }
    }

    void Expense::rollBack(const ExpenseCommit& commit) {
        for (auto& diff: commit.diffs) {
            int before = diff.second.first, after = diff.second.second;
            if (before) {
                weights[diff.first] = before;
            }
            else {
                weights.erase(diff.first);
            }
            total_weight -= (after - before);
        }
    }

    std::vector<Utils::Debt> Expense::toDebts(bool isReverse) {
        using Utils::Debt;
        double totalWeight = 0.0;
        double coeff = isReverse? -1.0: 1.0;
        std::vector<Debt> res;
        for (auto it = weights.begin(), last = weights.end();
                it != last; ++it) {
            totalWeight += it -> second;
        }
        for (auto it = weights.begin(), last = weights.end();
                it != last; ++it) {
            double share = static_cast<double>(it->second) / totalWeight;
            res.push_back(Debt(creditor, it->first, coeff * share * amount));
        }
        return res;
    }
    
    std::vector<std::string> Expense::formatWeightsString() const {
        constexpr int one_line_width = 42;
        //format the string such that they are 42 characters each line
        int width_now = 0;
        std::stringstream line;
        std::vector<std::string> res;
        for (const auto& weight_pair: weights) {
            const std::string pair_str = weight_pair.first + "(" +
                std::to_string(weight_pair.second) + ")";
            /* std::cout << line.str() << std::endl; */
            int offset = width_now == 0? 0: 1;
            if ((width_now + pair_str.size() + offset) <= one_line_width)
            {
                line << (offset?  " ": "") << pair_str;
                width_now += offset + pair_str.size();
            }
            else {
                res.push_back(line.str());
                line.str(std::string());
                line << pair_str;
                width_now = pair_str.size();
            }
        }
        res.push_back(line.str());
        return res;
    }

    std::ostream& operator<<(std::ostream& os, const Expense& expense) {
        os << "$" << expense.amount << "\n";
        os << "shared by " << expense.weights.size() << " participants\n\n";
        os << " note: " << expense.note << "\n\n" <<std::endl;
        return os;
    }
}
