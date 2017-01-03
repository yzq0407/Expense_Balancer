#include <iostream>
#include <string>
#include <unordered_map>
#include <memory>
#include <vector>
#include <set>
#include <stack>

#include "BalanceMap.h"
#include "utils.h"

namespace AccountBalancer {
    struct BalanceMap::BalanceMapImpl {
        //transaction map
        std::unordered_map<std::string, std::unordered_map<std::string, double>> map;
    };

    BalanceMap::BalanceMap(): pimpl(std::make_unique<BalanceMapImpl>()) {}
    BalanceMap::~BalanceMap() = default;

    //accessors
    double BalanceMap::getMoneyOwe(const std::string& debtor, 
            const std::string& creditor) const {
        if (pimpl->map.find(debtor) == pimpl->map.end() ||
                pimpl->map[creditor].find(creditor) == pimpl->map[creditor].end())
            return 0.0;
        return pimpl->map[debtor][creditor];
    }

    const std::unordered_map<std::string, double>* 
        BalanceMap::getAllDebts(const std::string& debtor) const {
        if (pimpl->map.find(debtor) == pimpl->map.end())  return nullptr;
        return &(pimpl->map[debtor]);
    }

    //modifiers
    void BalanceMap::clear() {
        pimpl->map.clear();
    }

    void BalanceMap::addDebt(const Utils::Debt& debt) {
        double original = 0;
        if (pimpl->map[debt.debtor].find(debt.creditor) 
                != pimpl->map[debt.debtor].end())
            original = pimpl->map[debt.debtor][debt.creditor];
        else if(pimpl->map[debt.creditor].find(debt.debtor)
                != pimpl->map[debt.creditor].end())
            original = -pimpl->map[debt.creditor][debt.debtor];

        double updated = original + debt.amount;

        if (Utils::isEqual(updated, 0.0)) {
            pimpl->map[debt.creditor].erase(debt.debtor);
            pimpl->map[debt.debtor].erase(debt.creditor);
        }
        else if (updated > 0.0) {
            pimpl->map[debt.debtor][debt.creditor] = updated;
            pimpl->map[debt.creditor].erase(debt.debtor);
        }
        else if (updated < 0.0) {
            pimpl->map[debt.creditor][debt.debtor] = -updated;
                pimpl->map[debt.debtor].erase(debt.creditor);
        }
    }

    void BalanceMap::addDebt(const std::vector<Utils::Debt>& debts) {
        //commit change
        for (auto& debt: debts) {
            addDebt(debt);
        }
    }

    std::ostream& operator<<(std::ostream& os, const BalanceMap& balanceMap) {
        os << "============= Print out the balance map =================\n";
        for (auto it = balanceMap.pimpl->map.begin(), 
                last = balanceMap.pimpl->map.end();  it != last; ++it) {
            os << "**--**--**--**--**--**--**--**--**--**--**--**\n";
            if (it->second.size() == 0) {
                os << it -> first << " has no debt \n";
            }
            else {
                os << it -> first << "\n";
                for (auto it2 = it->second.begin(), last2 = it->second.end();
                        it2 != last2; ++it2) {
                   os << "  ---->  $" << it2 -> second << "  --->   " << it2 -> first << "\n";
                }
            }
            os << "**--**--**--**--**--**--**--**--**--**--**--**\n";
        }
        os << "=============           End             =================\n";
        return os;
    }
} //AccountBalancer

