//Interface for the control of whole program
//Created by Theodore Yang on 1/1/2017
#ifndef __BALANCE_CONTROL_H
#define __BALANCE_CONTROL_H

#include <memory>
#include <vector>
#include "Expense.h"

namespace AccountBalancer {
    //this is a singleton
    class Control {
    private:
        struct ControlImpl;
        std::unique_ptr<ControlImpl> pimpl;
        Control();

        void undoExpense();
        void printExpense() const;

        void addFolks(const std::vector<std::string>&);
        void removeFolks(const std::vector<std::string>&);
        void printFolks() const;

        //the main menu show option
        void showMain(const std::vector<std::string>&);

        void showExp(const std::vector<std::string>&);

        void addExpParticipants(const std::vector<std::string>&, Expense&) const;

        void removeExpParticipants(const std::vector<std::string>&, Expense&) const;

    public:
        Control(const Control&) = delete;
        Control& operator=(const Control) = delete;
        ~Control();

        //singleton method
        static Control& getControl();

        void printWelcome() const;
        //controls
        void control_main();

        void control_expense(std::shared_ptr<Expense> expense);

    };
}
#endif
