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

        BalanceMap() = default;
        BalanceMap(const BalanceMap& other) = delete;
        BalanceMap& operator=(const BalanceMap& other) = delete;

        BalanceMap(BalanceMap&& other) = default;
        BalanceMap& operator=(BalanceMap&& other) = default;

        explicit BalanceMap(std::initializer_list<std::string>);
        //accessors
        std::vector<std::string> getFolks() const;
        
        double getMoneyOwe(const std::string& debtor, const std::string& creditor) const;

        const std::unordered_map<std::string, double>* getAllDebts(const std::string& debtor) const;

        //modifiers
        void addFolk(const std::string& name);

        void addFolk(const std::vector<std::string>& names);

        void deleteFolk(const std::string& name);

        void deleteFolk(const std::vector<std::string>& names);
        
        void clear();

        void addDebt(const Debt& debt);

        void addDebt(const std::vector<Debt>& debts);

        void addExpense(std::shared_ptr<Expense> expense_ptr);

        void undoExpense();

        bool validateName(const std::string& name);
    };

    std::ostream& operator<<(std::ostream& os, const BalanceMap& balanceMap);

}  //AccountBalancer
#endif
