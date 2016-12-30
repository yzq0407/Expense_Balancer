#ifndef __ACCOUNT_BALANCE_UTILS
#define __ACCOUNT_BALANCE_UTILS
#include <string>
#include <iostream>
#include <cmath>

namespace {
    constexpr double eps = 0.0000001;
} //anonymous namespace

namespace AccountBalancer {
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

}  //AccountBalancer
#endif
