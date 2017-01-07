//This class optimize balance transfers given bunch of expenses
//Created by Theodore Yang on 1/5/2017
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
        OUT_OF_TIME,
        NAME_NOT_FOUND,
        FAILED
    };

    enum class OptimizerStrategy {
        LEAST_TRANSFER,
        LAZY
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


    class BalanceOptimizer {
    private:
        struct BalanceOptimizerImpl;
        std::unique_ptr<BalanceOptimizerImpl> pimpl;

        OptimizerStatus lazyOptimize();

        OptimizerStatus leastTransferOptimize();

    public:
        BalanceOptimizer();

        ~BalanceOptimizer();

        //test whether this optimizer is a valid (newer than the given time)
        bool isUpToTime(const std::chrono::time_point<std::chrono::system_clock>& time_point) const;

        //given a set of expenses, optimize it, return status code
        OptimizerStatus optimizeExpenses(const std::vector<std::shared_ptr<Expense>>& expenses,
                OptimizerStrategy);

        //output a single person's transfers
        OptimizerStatus printParticipantTransfers(const std::string& name) const;

        //print a single person's expenses
        OptimizerStatus printParticipantExpenses(const std::string& name) const;

        //print a single person's expense summary, an overall report for expense and transfer
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
        //add one more payment
        void addPayment(std::shared_ptr<Expense> payment);

        //add one more transfer
        void addTransfer(const Transfer& transfer);
        void addTransfer(Transfer&& transfer);

        //get the total_expense this person supposed to make
        double& getTotalExpense();
        const double& getTotalExpense() const;

        //get how much this person paid at the first place
        double& getPaymentMadeValue();
        const double& getPaymentMadeValue() const;

        //get all the transfers
        std::vector<Transfer>& getTransfers();
        const std::vector<Transfer>& getTransfers() const;

        //get all the payments
        std::vector<std::weak_ptr<Expense>>& getPayments();
        const std::vector<std::weak_ptr<Expense>>& getPayments() const;

        //get all the expenses
        std::vector<std::weak_ptr<Expense>>& getExpenses();
        const std::vector<std::weak_ptr<Expense>>& getExpenses() const;
    };
} //AccountBalancer

namespace std {
    template <>
    void swap<AccountBalancer::TransferSummary> (AccountBalancer::TransferSummary&,
            AccountBalancer::TransferSummary&) noexcept;
} //std
#endif

