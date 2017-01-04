#ifndef __BALANCE_OPTIMIZER_H
#define __BALANCE_OPTIMIZER_H
#include <string>
#include <vector>

#include "BalanceMap.h"
#include "utils.h"

namespace AccountBalancer {
    class BalanceOptimizer {
    private:
        struct BalanceOptimizerImpl;
        std::unique_ptr<BalanceOptimizerImpl> pimpl;

    public:
        BalanceOptimizer();

        BalanceMap& optimizeTransactions(const BalanceMap& original_map);

        BalanceMap& getLatestOptimization() const;
    };
} //AccountBalancer

#endif

