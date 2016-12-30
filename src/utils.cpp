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
} //AccountBalancer
                        
