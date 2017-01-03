#include <sstream>

#include "utils.h"
#include "Expense.h"

namespace AccountBalancer {
    namespace Utils {
        Debt::Debt(const std::string& _creditor,
                const std::string& _debtor, double _amount): 
            creditor(_creditor), debtor(_debtor), amount(_amount) {}

        std::ostream& operator<<(std::ostream& os, const Debt& transaction) {
            os << transaction.debtor << " owns " 
                << transaction.creditor << "   $" << transaction.amount;
            return os;
        }

        std::vector<std::string> splitLine(std::string& str) {
            std::istringstream iss(str);
            std::vector<std::string> tokens;
            copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(),
                    back_inserter(tokens));
            return tokens;
        }

        std::string concatTokens(const std::vector<std::string>& tokens, int start, int end) {
            std::stringstream ss;
            for (int pos = start; pos < end; ++pos) {
                ss << tokens[pos] << " ";
            }
            return ss.str();
        }

    } //Utils
} //AccountBalancer
                        
