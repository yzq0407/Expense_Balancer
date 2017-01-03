#ifndef __ACCOUNT_BALANCE_UTILS
#define __ACCOUNT_BALANCE_UTILS
#include <string>
#include <sstream>
#include <iterator>
#include <iostream>
#include <cmath>
#include <map>
#include <vector>

namespace {
    constexpr double eps = 0.0000001;
} //anonymous namespace

namespace AccountBalancer {
    namespace Utils {
        struct Debt {
            std::string creditor;
            std::string debtor;
            double amount;

            Debt(const std::string& _creditor, 
                    const std::string& _debtor, double _amount);
        };

        std::ostream& operator<<(std::ostream& os, const Debt& transaction);

        inline bool isEqual(double a, double b) {
            return std::abs(a - b) <= eps;
        }

        //split a line by space delimiter
        std::vector<std::string> splitLine(std::string& str);

        //concatenate multiple string tokens into single space separated string
        std::string concatTokens(const std::vector<std::string>& tokens, int start, int end);

    } // Utils
}  //AccountBalancer
#endif
