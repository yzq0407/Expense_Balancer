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

        void addExpParticipants(const std::vector<std::string>&);

        void removeExpParticipants(const std::vector<std::string>&);

    public:
        Control(const Control&) = delete;
        Control& operator=(const Control) = delete;
        ~Control();

        //singleton method
        static Control& getControl();

        //controls
        void control_main();

        void control_expense(Expense expense);

    };
}
#endif
