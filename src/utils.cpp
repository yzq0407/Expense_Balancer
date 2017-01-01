#include "utils.h"

namespace AccountBalancer {
    Debt::Debt(const std::string& _creditor,
            const std::string& _debtor, double _amount): 
        creditor(_creditor), debtor(_debtor), amount(_amount) {}

    std::ostream& operator<<(std::ostream& os, const Debt& transaction) {
        os << transaction.debtor << " owns " 
            << transaction.creditor << "   $" << transaction.amount;
        return os;
    }

    void expenseToDebts(const Expense& expense, std::vector<Debt>& debts){
        double total = 0.0;
        for (auto it = expense.weight.begin(), last = expense.weight.end();
                it != last; ++it) {
            total += it->second;
        }
        for (auto it = expense.weight.begin(), last = expense.weight.end();
                it != last; ++it) {
            double share = static_cast<double>(it->second) / total;
            debts.push_back(Debt(expense.creditor, it->first, share * expense.amount));
        }
    }
} //AccountBalancer
                        
