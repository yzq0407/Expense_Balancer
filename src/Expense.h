#ifndef __BALANCE_EXPENSE_H
#define __BALANCE_EXPENSE_H
//this class implement every expense details include 
//amount, share people, share weights, notes etc
//Created by Theodore Yang on 1/1/2017

#include <stack>
#include <map>
#include <string>
#include <vector>
#include <iostream>
#include <memory>
namespace AccountBalancer {
    //a single commit in the expense report, we can roll back at any time
    struct ExpenseCommit;

    class Expense { 
    public:
        //constructor
        template <typename... T>
        explicit Expense(T&&... args);

        //accessors
        int numOfParticipants() const noexcept;
        void printCommitsHistory(bool verbose = true) const;

        //modifiers
        void setNote(std::string) noexcept;
        void setAmount(double) noexcept;
        void addParticipant(const std::vector<std::string>&);
        void removeParticipant(const std::vector<std::string>&);
        void changeWeights(const std::vector<std::pair<std::string, int>>&);

        void rollBack();
        void rollBack(const ExpenseCommit& commit);

    private:
        struct ExpImpl;
        std::shared_ptr<ExpImpl> pimpl;

    friend std::ostream& operator<<(std::ostream&, const Expense&);
    };

    std::ostream& operator<<(std::ostream& s, const Expense& expense);
}
#endif
