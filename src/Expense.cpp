//implement the expense class
//Created by Theodore Yang on 1/1/2017

#include "Expense.h"
#include <iostream>
namespace {
    constexpr const char* default_note = "no notes";
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

    template <typename... T>
    Expense::Expense(T&&... args): 
        pimpl(std::make_shared<ExpImpl>(std::forward<T>(args)...))
    {}

    //-----------private fields--------------
    struct Expense::ExpImpl {
        //creditor
        std::string creditor;
        //total amount, always nonegative
        double amount;
        //a notation
        std::string note;
        //commit history
        std::deque<ExpenseCommit> commit_hist;
        //the current weight split
        std::map<std::string, int> weights;

        //inline ctors
        explicit ExpImpl(const std::string& _creditor,
                double _amount = 0,
                const std::string& _note = default_note):
                    creditor(_creditor), amount(_amount), note(_note){}

        ExpImpl(std::string&& _creditor, double _amount, std::string&& _note):
            creditor(std::move(_creditor)), amount(_amount), note(std::move(_note)) {}

        explicit ExpImpl(const std::string& _creditor, 
                const std::vector<std::string>& participants, double _amount = 0):
            creditor(_creditor),
            note(default_note),
            amount(_amount) {
            for (const auto& participant: participants) {
                weights[participant] = 1;
            }
        }

        ExpImpl(std::string&& _creditor, 
                const std::vector<std::string>& participants, double _amount):
            creditor(std::move(_creditor)),
            amount(_amount)
        {
            for (const auto& participant: participants) {
                weights[participant] = 1;
            }
        }
    };

    //accessors
    int Expense::numOfParticipants() const noexcept {
        return pimpl->weights.size();
    }

    void Expense::printCommitsHistory(bool verbose) const {
        for (auto& commit: pimpl->commit_hist) {
            printExpenseCommit(commit, verbose);
        }
    }

    void Expense::printExpenseTitle() const {
        std::cout << "  Creditor:  " << pimpl->creditor << std::endl;
        std::cout << "  Amount:   $" << pimpl->amount << std::endl;
        std::cout << "  Note:      " << pimpl->note << std::endl;
    }

    //modifiers
    void Expense::setNote(std::string note) noexcept {
        pimpl->note = std::move(note);
    }

    void Expense::setAmount(double amount) noexcept {
        pimpl->amount = amount;
    }

    void Expense::addParticipant(const std::vector<std::string>& names) {
        ExpenseCommit commit(CommitType::AddPartic);
        for (auto& name: names) {
            if (pimpl->weights.find(name) == pimpl->weights.end()) {
                pimpl->weights[name] = 1;
                commit.diffs.push_back(std::make_pair(name, std::make_pair(0, 1)));
            }
            else {
                std::cout << "ignore " << name << " for it is already in the participants list" << std::endl;
            }
        }
        pimpl->commit_hist.push_back(std::move(commit));
    }

    void Expense::removeParticipant(const std::vector<std::string>& names) {
        ExpenseCommit commit(CommitType::RemovePartic);
        for (auto& name: names) {
            if (pimpl->weights.find(name) != pimpl->weights.end()) {
                commit.diffs.push_back(std::make_pair(name, 
                            std::make_pair(pimpl->weights[name], 0)));
                pimpl->weights.erase(name);
            }
            else {
                std::cout << "ignore " << name << " for it's not in the participants list" << std::endl;
            }
        }
        pimpl->commit_hist.push_back(std::move(commit));
        //make sure there is at least one participant
        if (pimpl->weights.empty()) {
            std::cout << "at least one participant has to show up" << std::endl;
            rollBack();
        }
    }

    void Expense::changeWeights(const std::vector<std::pair<std::string, int>>& change_list) {
        ExpenseCommit commit (CommitType::WeightChange);
        //check the weight change, make sure it is legal, otherwise, stop and roll back
        //if it is leg
        for (auto& change: change_list) {
            int before = pimpl->weights[change.first];
            int after = change.second;
            if (after < 0) {
                std::cout << change.first << "'s share weight can not be negative" << std::endl;
                std::cout << "aborted" << std::endl;
                rollBack(commit);
                break;
            }
            commit.diffs.push_back(std::make_pair(change.first,
                        std::make_pair(before, after)));
            pimpl->weights[change.first] = after;
        }
        pimpl->commit_hist.push_back(std::move(commit));
    }

    void Expense::rollBack() {
        if (pimpl->commit_hist.empty()) {
            std::cout << "already in the initial commit" << std::endl;
        }
        else {
            auto& commit = pimpl->commit_hist.back();
            pimpl->commit_hist.pop_back();
            rollBack(commit);
        }
    }

    void Expense::rollBack(const ExpenseCommit& commit) {
        for (auto& diff: commit.diffs) {
            int before = diff.second.first;
            int after = diff.second.second;
            if (before) {
                pimpl->weights[diff.first] = before;
            }
            else {
                pimpl->weights.erase(diff.first);
            }
        }
    }

    std::ostream& operator<<(std::ostream& os, const Expense& expense) {
        os << "$" << expense.pimpl->amount << "\n";
        os << "shared by " << expense.pimpl->weights.size() << " participants\n\n";
        os << " note: " << expense.pimpl->note << "\n\n" <<std::endl;
        return os;
    }
}
