#include <chrono>
#include <algorithm>

#include "Optimizer.h"
#include "Expense.h"

namespace AccountBalancer {
    //optimizer implimentations
    struct BalanceOptimizer::BalanceOptimizerImpl {
        bool verbose = false;
        //the time point with high resolution, which is kept at record
        //to see if we need to update optimization
        std::chrono::time_point<std::chrono::system_clock> last_optimize_time = 
            std::chrono::system_clock::now();

        //a map from name to tansferSummary, support orderer output
        std::map<std::string, TransferSummary> result;
    };

    BalanceOptimizer::BalanceOptimizer(): pimpl(std::make_unique<BalanceOptimizerImpl>()) {}

    bool BalanceOptimizer::isUpToTime (
            const std::chrono::time_point<std::chrono::system_clock>& time_point) const {
        return pimpl->last_optimize_time > time_point;
    }

    OptimizerStatus BalanceOptimizer::optimizeExpenses(
            const std::vector<std::shared_ptr<Expense>>& expenses) {
        pimpl->result.clear();
        //process each expenses
        for (auto& expense: expenses) {
            auto weight_map = expense->getWeightsMap();
            double amount = expense->getAmount();
            double weight_sum = static_cast<double> (expense->getWeightSum());
            for (auto it = weight_map.begin(), last = weight_map.end();
                    it != last; ++it) {
                std::string name = it->first;
                int share = it->second;
                
                if (pimpl->result.find(name) == pimpl->result.end()) {
                    pimpl->result.emplace(name, name);
                }
                //this method might throw exception, but since we construct it before,
                //it should be fine
                pimpl->result.at(name).addExpense(expense);

                //the expense need to add to everybody's account
                pimpl->result.at(name).getTotalExpense() += amount * share / weight_sum;
            }
            //now the creditor of this expense has to be added into payment
            auto creditor = expense->getCreditor();
            if (pimpl->result.find(creditor) == pimpl->result.end()) {
                pimpl->result.emplace(creditor, creditor);
            }
            pimpl->result.at(creditor).getPaymentMade() += amount;
        }

        //the strategy to optimize the gap
        //"gap" is defined to be the difference between the amount this person should pay
        //and the he actually paied
        //overall, the gap must be 0, but we want to optimize such that the total number of
        //transfers is minimum
        std::vector<std::pair<std::string, double>> gaps;
        gaps.reserve(pimpl->result.size());
        //for each person, calculate gap and put it into the vector
        for (auto it = pimpl->result.cbegin(), last = pimpl->result.cend();
                it != last; ++it) {
            auto name = it->first;
            double payment_made = it->second.getPaymentMade();
            double total_expense = it->second.getTotalExpense();
            double gap = payment_made - total_expense;
            gaps.push_back(std::make_pair(name, gap));
        }
        //sort the gaps, based on the gap value
        auto comparator = [](const std::pair<std::string, double>& p1, 
                const std::pair<std::string, double>& p2) -> bool {
            return p1.second < p2.second;
        };
        std::sort(gaps.begin(), gaps.end(), comparator);
        //use dfs to get the minimum amount of transfer

        return OptimizerStatus::SUCCESS;
    }

    //transfer summay implementation
    struct TransferSummary::TransferSummaryImpl {
        //name of the person
        std::string name;
        //all the expenses that is engaged in, use weak pointer since it might dangle
        std::vector<std::weak_ptr<Expense>> expenses;
        //a list of transfers that are supposed to happen, it is a list of debt, also, use weak_ptr
        std::vector<Transfer> transfers;
        //total expense, this is the expense this person should make
        double total_expense;
        //total payment, this is the payment this person made at the first place
        double payment_made;

        //constructor
        TransferSummaryImpl(const std::string& _name): name(_name), 
                                                       total_expense(0.0),
                                                       payment_made(0.0) {}

        TransferSummaryImpl(std::string&& _name): name(std::move(_name)),
                                                  total_expense(0.0),
                                                  payment_made(0.0) {}
    };

    //ctors
    TransferSummary::TransferSummary(const std::string& name):
        pimpl(std::make_unique<TransferSummaryImpl>(name)) {}

    TransferSummary::TransferSummary(std::string&& name):
        pimpl(std::make_unique<TransferSummaryImpl>(std::move(name))) {}

    //copy ctor and copy assignment operator
    //note that this class have value semantics
    TransferSummary::TransferSummary(const TransferSummary& _other):
        pimpl(std::make_unique<TransferSummaryImpl>(*_other.pimpl)) {}

    TransferSummary& TransferSummary::operator=(const TransferSummary& _other) {
        //strong exception guarantee
        using std::swap;
        TransferSummary temp(_other);
        swap(*this, temp);
        return *this;
    }

    //dtor
    TransferSummary::~TransferSummary() = default;

    void TransferSummary::swap(TransferSummary& other) noexcept {
        using std::swap;
        swap(pimpl, other.pimpl);
    }

    //add one more expense
    void TransferSummary::addExpense(std::shared_ptr<Expense> expense) {
        if (expense) {
            pimpl->expenses.push_back(std::weak_ptr<Expense>(expense));
        }
    }

    //get total expense
    double& TransferSummary::getTotalExpense() {
        return pimpl->total_expense;
    }

    const double& TransferSummary::getTotalExpense() const {
        return pimpl->total_expense;
    }

    //get payment made
    double& TransferSummary::getPaymentMade() {
        return pimpl->payment_made;
    }

    const double& TransferSummary::getPaymentMade() const {
        return pimpl->payment_made;
    }
} //AccountBalancer

namespace std {
    void swap(AccountBalancer::TransferSummary& left, 
            AccountBalancer::TransferSummary& right) noexcept {
        left.swap(right);
    }
} //std
