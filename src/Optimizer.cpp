#include "Optimizer.h"

namespace AccountBalancer {
    //this is supposed to be the settings
    struct BalanceOptimizer::BalanceOptimizerImpl {
        bool verbose = false;
        bool assertion = false;
    };

    BalanceOptimizer::BalanceOptimizer(): pimpl(std::make_unique<BalanceOptimizerImpl>()) {}

    std::vector<Debt> BalanceOptimizer::getOptimizedTransactions(const BalanceMap& map) {
        //sum up all the debt and get the total amount for each person
        std::unordered_map<std::string, double> totalDebt;
        std::vector<std::string> people = map.getFolks();
        std::vector<Debt> res;
        for (const std::string& name: people) {
            totalDebt[name] = 0.0;
        }
        for (const std::string& name: people) {
            auto debts = map.getAllDebts(name);
            for (auto it = debts->begin(), last = debts->end();
                    it != last; ++it) {
                totalDebt[name] += it->second;
                totalDebt[it->first] -= it->second;
            }
        }
        return std::vector<Debt>();
    }
} //AccountBalancer
