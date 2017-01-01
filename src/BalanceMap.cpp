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

        //all the folks
        std::set<std::string> folks;

        //expense list
        std::stack<std::shared_ptr<Expense>> expense_list;
    };

    BalanceMap::BalanceMap(std::initializer_list<std::string> il) {
        for (std::string name: il) {
            if (pimpl->folks.find(name) == pimpl->folks.end()) {
                pimpl->folks.insert(name);
            }
        }
    }

    //accessors
    std::vector<std::string> BalanceMap::getFolks() const {
        return std::vector<std::string> (pimpl->folks.begin(), pimpl->folks.end());
    }

    bool BalanceMap::validateName(const std::string& name) {
        bool res = true;
        if (pimpl->folks.find(name) == pimpl->folks.end()) {
            res = false;
            std::cout << "=======cannot find the name: " << name
                << "===========" << std::endl;
        }
        return res;
    }
            

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
    void BalanceMap::addFolk(const std::string& name) {
        pimpl->folks.insert(name);
    }

    void BalanceMap::addFolk(const std::vector<std::string>& names) {
        for (const std::string& name: names) {
            addFolk(name);
        }
    }

    void BalanceMap::deleteFolk(const std::string& name) {
        pimpl->folks.erase(pimpl->folks.find(name));
        pimpl->map.erase(pimpl->map.find(name));
    }

    void BalanceMap::deleteFolk(const std::vector<std::string>& names) {
        for (const std::string& name: names) {
            deleteFolk(name);
        }
    }

    void BalanceMap::clear() {
        pimpl->folks.clear();
        pimpl->map.clear();
    }

    void BalanceMap::addDebt(const Debt& debt) {
        if (validateName(debt.debtor) && validateName(debt.creditor)) {
            double original = 0;
            if (pimpl->map[debt.debtor].find(debt.creditor) 
                != pimpl->map[debt.debtor].end())
                original = pimpl->map[debt.debtor][debt.creditor];
            else if(pimpl->map[debt.creditor].find(debt.debtor)
                    != pimpl->map[debt.creditor].end())
                original = -pimpl->map[debt.creditor][debt.debtor];

            double updated = original + debt.amount;

            if (isEqual(updated, 0.0)) {
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
    }

    void BalanceMap::addDebt(const std::vector<Debt>& debts) {
        //commit change
        for (const Debt& debt: debts) {
            addDebt(debt);
        }
    }

    void BalanceMap::addExpense(std::shared_ptr<Expense> expense_ptr) {
        //expense need to be added to the list
        std::vector<Debt> debts;
        expenseToDebts(*expense_ptr, debts);
        addDebt(debts);
        pimpl->expense_list.push(expense_ptr);
    }

    void BalanceMap::undoExpense() {
        if (pimpl->expense_list.empty()) {
            std::cout << "!!!!!        Expense list is empty       !!!!!" << std::endl;
        }
        else {
            auto expense_ptr = pimpl->expense_list.top();
            pimpl->expense_list.pop();
            expense_ptr->amount *= -1;
            std::vector<Debt> debts;
            expenseToDebts(*expense_ptr, debts);
            addDebt(debts);
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

