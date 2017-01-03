//Implementation for the control of whole program
//Created by Theodore Yang on 1/1/2017
#include <iostream>
#include <deque>
#include <set>

#include "BalanceMap.h"
#include "Expense.h"
#include "Control.h"
#include "InputReader.h"

namespace {
    constexpr const char* welcome = "Welcome using account balancer to optimize \
                                     transfers, type '--help' for more info";
    enum ControlStatus {
        Main,
        Expense
    };

    enum class Action_Main {
        Add_Part,
        Rm_Part,
        Add_Exp,
        Rm_Exp,
        Opt,
        Verb,
        Show,
        Help,
        Quit,
        Undefine
    };

    enum class Action_Exp {
        Add_Part,
        Rm_Part,
        CG_WG,
        Show,
        Help,
        Quit,
        Undefine
    };

    const std::unordered_map<std::string, Action_Main> 
        option_map_main {
            {"--add-p", Action_Main::Add_Part}, 
            {"--rm-p", Action_Main::Rm_Part}, 
            {"--add-exp", Action_Main::Add_Exp},
            {"--rm-exp", Action_Main::Add_Exp},
            {"--add-exp", Action_Main::Add_Exp},
            {"--verbose", Action_Main::Verb},
            {"--show", Action_Main::Show},
            {"--opt", Action_Main::Opt},
            {"--help", Action_Main::Help},
            {"--quit", Action_Main::Quit}
        };

    const std::unordered_map<std::string, Action_Exp>
        option_map_exp {
            {"--add-p", Action_Exp::Add_Part},
            {"--rm-p", Action_Exp::Rm_Part},
            {"--cw", Action_Exp::CG_WG},
            {"--show", Action_Exp::Show},
            {"--help", Action_Exp::Help},
            {"--quit", Action_Exp::Quit}
        };

    inline Action_Main parseOptionMain(const std::vector<std::string> &tokens) {
        if (tokens.size() == 0 || option_map_main.find(tokens[0]) == option_map_main.end()) {
            return Action_Main::Undefine;
        }
        return option_map_main.at(tokens[0]);
    }

    inline Action_Exp parseOptionExp(const std::vector<std::string> &tokens) {
        if (tokens.size() == 0 || option_map_exp.find(tokens[0]) == option_map_exp.end()) {
            return Action_Exp::Undefine;
        }
        return option_map_exp.at(tokens[0]);
    }

}

namespace AccountBalancer {
    struct Control::ControlImpl {
        BalanceMap map;
        std::deque<std::shared_ptr<Expense>> expense_hist;
        std::set<std::string> participants;
        
        ControlStatus status = Main;
    };

    //ctors and dtors
    Control::Control(): pimpl(std::make_unique<ControlImpl>()) {}
    Control::~Control() = default;

    //undo expense
    void Control::undoExpense() {
        if (pimpl->expense_hist.empty()) {
            std::cerr << "No expense history yet" << std::endl;
        }
        else {
            pimpl->expense_hist.pop_front();
        }
    }

    //print expense 
    void Control::printExpense() const {
        for (auto& expense: pimpl->expense_hist) {
            expense->printExpenseTitle();
            std::cout << std::endl; 
        }
    }

    //add and remove all the folks
    void Control::addFolks(const std::vector<std::string>& folks) {
        if (folks.empty()) {
            std::string input;
            std::getline(std::cin, input);
            std::vector<std::string> tokens = Utils::splitLine(input);
            addFolks(tokens);
        }
        else {
            pimpl->participants.insert(folks.begin(), folks.end());
        }
    }

    void Control::removeFolks(const std::vector<std::string>& folks) {
        if (folks.empty()) {
            std::string input;
            std::getline(std::cin, input);
            std::vector<std::string> tokens = Utils::splitLine(input);
            removeFolks(tokens);
        }
        else {
            for (auto& folk: folks) {
                pimpl->participants.erase(folk);
            }
        }
    }

    //for an expense session add and remove multiple participants
    void Control::addExpParticipants(const std::vector<std::string>& folks) {
    }

    void Control::removeExpParticipants(const std::vector<std::string>& folks) {
    }

    void Control::control_main() {
        bool run = true;
        std::string input;
        while (run) {
            std::getline(std::cin, input);
            std::vector<std::string> tokens = Utils::splitLine(input);
            switch (parseOptionMain(tokens)) {
                case Action_Main::Add_Part:
                    addFolks(std::vector<std::string>(++tokens.begin(), tokens.end()));
                    break;

                case Action_Main::Rm_Part:
                    removeFolks(std::vector<std::string>(++tokens.begin(), tokens.end()));
                    break;

                //enter the expense adding interface
                case Action_Main::Add_Exp:
                    if (tokens.size() < 2) {
                        std::cerr << "must designate a expense creditor" << std::endl;
                    }
                    else if (pimpl->participants.find(tokens[1]) 
                            == pimpl->participants.end()) {
                        std::cerr << "unknown name: " << tokens[1] << ". Must add first" << std::endl;
                    }
                    else {
                        auto creditor = tokens[1];
                        double amount = 0;
                        //parse amount info
                        if (tokens.size() >= 3) {
                            try {
                                amount = std::stod(tokens[2]);
                            }
                            catch (const std::invalid_argument& ia) {
                                std::cerr << "invalid expense amount: " << tokens[2] << std::endl;
                                break;
                            }
                        }
                        if (tokens.size() >= 4) {
                            control_expense(Expense(std::move(creditor), amount, 
                                        std::move(tokens[4])));
                        }
                        else {
                            control_expense(Expense(creditor, amount));
                        }
                    }
                    break;

                case Action_Main::Rm_Exp:
                    undoExpense();
                    break;

                case Action_Main::Opt:
                    break;
                case Action_Main::Verb:
                    break;
                case Action_Main::Show:
                    printExpense();
                    break;
                case Action_Main::Help:
                    break;
                case Action_Main::Undefine:
                    break;
                case Action_Main::Quit:
                    run = false;
                    break;
            }
        }
    }

    void Control::control_expense(Expense expense) {
        std::cout << "============== Expense session ================" << std::endl;
        expense.printExpenseTitle();
        bool run = true;
        std::string input;
        while (run) {
            std::getline(std::cin, input);
            std::vector<std::string> tokens = Utils::splitLine(input);
            switch (parseOptionExp(tokens)) {
                case Action_Exp::Add_Part:
                    addExpParticipants(std::vector<std::string>(++tokens.begin(), tokens.end()));
                    break;

                case Action_Exp::Rm_Part:
                    removeExpParticipants(std::vector<std::string>(++tokens.begin(), tokens.end()));
                    break;

                case Action_Exp::CG_WG:
                    break;

                case Action_Exp::Show:
                    expense.printCommitsHistory(true);
                    break;

                case Action_Exp::Help:
                    break;

                case Action_Exp::Quit:
                    run = false;
                    break;

                case Action_Exp::Undefine:
                    break;
            }
        }

    }
}


