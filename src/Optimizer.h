#ifndef __BALANCE_OPTIMIZER_H
#define __BALANCE_OPTIMIZER_H
#include <string>
#include <vector>
#include <chrono>

#include "Expense.h"
#include "utils.h"

namespace AccountBalancer {
    enum class OptimizerStatus {
        SUCCESS,
        FAILED
    };

    class BalanceOptimizer {
    private:
        struct BalanceOptimizerImpl;
        std::unique_ptr<BalanceOptimizerImpl> pimpl;

    public:
        BalanceOptimizer();

        //test whether this optimizer is a valid (newer than the given time)
        bool isUpToTime(const std::chrono::time_point<std::chrono::system_clock>& time_point) const;

        //given a set of expenses, optimize it, return status code
        OptimizerStatus optimizeExpenses(const std::vector<std::shared_ptr<Expense>>& expenses);

        //output a summary of transfers
        OptimizerStatus printAllTransfers() const;

        //output a single person's transfers
        OptimizerStatus printParticipantTransfers(const std::string& name) const;

        //print a single person's expense summary
        OptimizerStatus printParticipantSummary(const std::string& name) const;
    };

    //a summary for a single participants containing
    //1. what is the different expenses that they are engaged in
    //2. who do you need to transfer based on the optimization
    //3. what is the total amount of expense
    class TransferSummary {
    private:
        struct TransferSummaryImpl;
        std::unique_ptr<TransferSummaryImpl> pimpl;
    public:
        friend std::ostream& operator<<(std::ostream&, const TransferSummary&);

        //ctors
        TransferSummary(const std::string& name);

        TransferSummary(std::string&& name);

        //copy ctors and copy assignment operator
        TransferSummary(const TransferSummary&);

        TransferSummary& operator=(const TransferSummary&);

        //dtor
        ~TransferSummary();

        //move ctors and move assignment sctor
        TransferSummary(TransferSummary&&) = default;

        TransferSummary& operator=(TransferSummary&&) = default;

        void swap(TransferSummary&) noexcept;

        //add one more expense
        void addExpense(std::shared_ptr<Expense> expense);

        //get the total_expense this person supposed to make
        double& getTotalExpense();

        const double& getTotalExpense() const;

        //get how much this person paid at the first place
        double& getPaymentMade();

        const double& getPaymentMade() const;

    };

    struct Transfer {
        std::string other;
        //if the amount is positive, meaning "other" owe you
        //otherwise, you owe "other"
        double amount;
        Transfer(const std::string _other, double _amount):
            other(_other),
            amount(_amount) {}

    };

} //AccountBalancer

namespace std {
    void swap(AccountBalancer::TransferSummary& left, 
            AccountBalancer::TransferSummary& right) noexcept;
} //std

#endif

