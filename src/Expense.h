#ifndef __BALANCE_EXPENSE_H
#define __BALANCE_EXPENSE_H
//this class implement every expense details include 
//amount, share people, share weights, notes etc
//Created by Theodore Yang on 1/4/2017

#include <stack>
#include <map>
#include <set>
#include <string>
#include <vector>
#include <iostream>
#include <memory>

#include "utils.h"

namespace AccountBalancer {
    //a single commit in the expense report, we can roll back at any time
    struct ExpenseCommit;

    //the expense class
    class Expense { 
    public:
        //constructor
        explicit Expense(const std::string& _creditor,
                double _amount = 0);

        Expense(const std::string& _creditor,
                double _amount,
                const std::string& _note);
        //dtor
        ~Expense();

        //accessors
        int numOfParticipants() const noexcept;
        bool hasParticipant(const std::string&) const;
        int getWeight(const std::string&) const;
        int getWeightSum() const;
        double getAmount() const;
        std::string getCreditor() const;
        std::string getNote() const;
        const std::map<std::string, int>& getWeightsMap() const;

        void printCommitsHistory(bool verbose = true) const;
        void printExpenseSummary() const;

        //modifiers
        void setNote(std::string) noexcept;
        void setAmount(double) noexcept;
        void addParticipant(const std::vector<std::string>&);
        void removeParticipant(const std::vector<std::string>&);
        void changeWeights(const std::vector<std::pair<std::string, int>>&);

        void rollBack();
        void rollBack(const ExpenseCommit& commit);

        //to debts
        std::vector<Utils::Debt> toDebts(bool isReverse = false);

    private:
        bool verbose = false;
        //creditor
        std::string creditor;
        //total amount, always nonegative
        double amount;
        //a notation
        std::string note;
        //commit history
        std::deque<std::unique_ptr<ExpenseCommit>> commit_hist;
        //the current weight split
        std::map<std::string, int> weights;
        //total weight
        int total_weight;

        //format the weight information so that we can facillitate printing
        std::vector<std::string> formatWeightsString() const;

        friend std::ostream& operator<<(std::ostream&, const Expense&);
    };

    std::ostream& operator<<(std::ostream& s, const Expense& expense);
}
#endif
