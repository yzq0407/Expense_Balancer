#ifndef __BALANCE_MAP_H
#define __BALANCE_MAP_H
#include <string>
#include <memory>
#include <vector>
#include <unordered_map>

#include "utils.h"

namespace AccountBalancer {
    class BalanceMap {
    private:
        struct BalanceMapImpl;

        std::unique_ptr<BalanceMapImpl> pimpl;  

    public:
        friend std::ostream& operator<<(std::ostream&, const BalanceMap&);

        BalanceMap();
        BalanceMap(const BalanceMap& other) = delete;
        BalanceMap& operator=(const BalanceMap& other) = delete;

        BalanceMap(BalanceMap&& other) = default;
        BalanceMap& operator=(BalanceMap&& other) = default;

        ~BalanceMap();

        //accessors
        double getMoneyOwe(const std::string& debtor, const std::string& creditor) const;

        std::vector<std::string> getAllPeople() const;

        const std::unordered_map<std::string, double>* getAllDebts(const std::string& debtor) const;

        void clear();

        void addDebt(const Utils::Debt& debt);

        void addDebt(const std::vector<Utils::Debt>& debts);
    };

    std::ostream& operator<<(std::ostream& os, const BalanceMap& balanceMap);

}  //AccountBalancer
#endif
