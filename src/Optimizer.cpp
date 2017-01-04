#include <algorithm>
#include "Optimizer.h"

namespace AccountBalancer {
    //this is supposed to be the settings
    struct BalanceOptimizer::BalanceOptimizerImpl {
        bool verbose = false;
        std::vector<std::unique_ptr<BalanceMap>> history;
    };

    BalanceOptimizer::BalanceOptimizer(): pimpl(std::make_unique<BalanceOptimizerImpl>()) {}

    BalanceMap& BalanceOptimizer::optimizeTransactions(const BalanceMap& original_map) {
        //sum up all the debt and get the total amount for each person
        std::vector<std::pair<std::string, double>> personal_debts;
        {
            std::unordered_map<std::string, double> personal_debts_map;
            for (const std::string& name: original_map.getAllPeople()) {
                //if not found initialize to 0.0
                if (personal_debts_map.find(name) == personal_debts_map.end()) {
                    personal_debts_map[name] = 0.0;
                }

                auto debts = original_map.getAllDebts(name);
                double owing = 0.0;
                for (auto it = debts->begin(), last = debts->end();
                        it != last; ++it) {
                    personal_debts_map[name] += it->second;
                    personal_debts_map[it->first] = personal_debts_map.find(it->first) 
                        == personal_debts_map.end()? -it->second: 
                        personal_debts_map[it->first] - it->second;
                }
            }

            //push everything into the vector and sort it by value
            for (auto it = personal_debts_map.begin(), last = personal_debts_map.end();
                    it != last; ++it) {
                if (!Utils::isEqual(it->second, 0.0)) {
                    personal_debts.push_back(*it);
                }
            }
            auto lambda_sort = [] (const std::pair<std::string, double>& p1, 
                    const std::pair<std::string, double>& p2) {
                return p1.second < p2.second;
            };
            std::sort(personal_debts.begin(), personal_debts.end(), lambda_sort);
        }
    }
} //AccountBalancer
