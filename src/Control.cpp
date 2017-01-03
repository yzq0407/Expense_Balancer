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
    constexpr const char* welcome 
        = "===========================================================\n"
          "===========================================================\n"
          "Welcome using account balancer to optimize balance transfers\n"
          "'help' for more info\n"
          "===========================================================\n"
          "===========================================================\n";

    constexpr const char* main_menu_title
        = "[main menu] $";

    constexpr const char* add_expense_title
        = "[add expense] $";

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
        Add_Note,
        Rm_Part,
        CG_WG,
        Show,
        Commit,
        Help,
        Quit,
        Undefine
    };

    enum class Show_Opt_Main {
        Expense,
        Partic,
        Balance,
    };

    const std::unordered_map<std::string, Action_Main> 
        option_map_main {
            {"add-p", Action_Main::Add_Part}, 
            {"rm-p", Action_Main::Rm_Part}, 
            {"add-exp", Action_Main::Add_Exp},
            {"rm-exp", Action_Main::Rm_Exp},
            {"verbose", Action_Main::Verb},
            {"show", Action_Main::Show},
            {"opt", Action_Main::Opt},
            {"help", Action_Main::Help},
            {"quit", Action_Main::Quit}
        };

    const std::unordered_map<std::string, Action_Exp>
        option_map_exp {
            {"add-p", Action_Exp::Add_Part},
            {"add-n", Action_Exp::Add_Note},
            {"rm-p", Action_Exp::Rm_Part},
            {"cw", Action_Exp::CG_WG},
            {"show", Action_Exp::Show},
            {"help", Action_Exp::Help},
            {"commit", Action_Exp::Commit},
            {"quit", Action_Exp::Quit}
        };

    const std::unordered_map<std::string, Show_Opt_Main>
        show_map_main {
            {"expense", Show_Opt_Main::Expense},
            {"partic", Show_Opt_Main::Partic},
            {"balance", Show_Opt_Main::Balance},
        };

    inline Action_Main parseOptionMain(const std::vector<std::string> &tokens) {
        if (tokens.size() == 0 || option_map_main.find(tokens[0]) == option_map_main.end()) {
            std::cerr << "unkown option" <<  ((tokens.size() != 0)? (": " + tokens[1]): "") << std::endl;
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

    inline Show_Opt_Main parseOptionsShowMain(const std::vector<std::string> &tokens) {
        if (tokens.size() < 2 || show_map_main.find(tokens[1]) == show_map_main.end()) {
            return Show_Opt_Main::Expense;
        }
        return show_map_main.at(tokens[1]);
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
            auto expense_ptr = pimpl->expense_hist.front();
            pimpl->map.addDebt(expense_ptr->toDebts(true));
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

    //print welcome message
    void Control::printWelcome() const {
        std::cout << welcome << std::endl;
    }

    //singleton method
    Control& Control::getControl() {
        static Control control;
        return control;
    }

    //add and remove all the folks
    void Control::addFolks(const std::vector<std::string>& folks) {
        pimpl->participants.insert(folks.begin(), folks.end());
        std::cout << "added " << pimpl->participants.size() << " folks" << std::endl;
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

    bool Control::validateParticipant(const std::set<std::string>& names) {
        for (auto& name: names) {
            if (pimpl->participants.find(name) == pimpl->participants.end()) {
                std::cerr << name << " is not in the participants list" << std::endl;
                return false;
            }
        }
        return true;
    }

    void Control::printFolks() const {
        for (auto& name: pimpl->participants) {
            std::cout << name << "  ";
        }
        std::cout << std::endl;
    }

    //The main menu show option
    void Control::showMain(const std::vector<std::string>& args) {
        switch (parseOptionsShowMain(args)) {
            case Show_Opt_Main::Expense:
                printExpense();
                break;
            case Show_Opt_Main::Partic:
                printFolks();
                break;
            case Show_Opt_Main::Balance:
                std::cout << pimpl->map << std::endl;
                break;
        }
    }


    //for an expense session add and remove multiple participants
    void Control::addExpParticipants(const std::vector<std::string>& names,
            Expense& expense) const {
        for (auto& name: names) {
            if (pimpl->participants.find(name) == pimpl->participants.end()) {
                std::cerr << name << " is not in the main participants pool" << std::endl;
                std::cerr << "Aborted" << std::endl;
                return;
            }
        }
        expense.addParticipant(names);
    }

    void Control::removeExpParticipants(const std::vector<std::string>& folks, Expense& expense) const{
        expense.removeParticipant(folks);
    }

    void Control::commitExpense(std::shared_ptr<Expense> expense_ptr) {
        pimpl->map.addDebt(expense_ptr->toDebts());
        pimpl->expense_hist.push_front(expense_ptr);
    }

    void Control::control_main() {
        bool run = true;
        std::string input;
        while (run) {
            std::cout << std::endl;
            std::cout << main_menu_title << std::endl;
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
                            std::set<std::string> partics(tokens.begin() + 3, tokens.end());
                            if (validateParticipant(partics)) {
                                partics.insert(creditor);
                                auto expense_ptr = std::make_shared<Expense>(creditor, amount,
                                        partics);
                                control_expense(expense_ptr);
                            }
                        }
                        else {
                            auto expense_ptr = std::make_shared<Expense>(creditor, amount, pimpl->participants); 
                            control_expense(expense_ptr);
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
                    showMain(tokens);
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

    void Control::control_expense(std::shared_ptr<Expense> expense_ptr) {
        std::cout << "============== Expense session ================" << std::endl;
        expense_ptr->printExpenseTitle();
        bool run = true;
        std::string input;
        while (run) {
            std::cout << std::endl;
            std::cout << add_expense_title << std::endl;
            std::getline(std::cin, input);
            std::vector<std::string> tokens = Utils::splitLine(input);
            switch (parseOptionExp(tokens)) {
                case Action_Exp::Add_Part:
                    addExpParticipants(std::vector<std::string>(++tokens.begin(), tokens.end()), *expense_ptr);
                    break;

                case Action_Exp::Add_Note:
                    if (tokens.size() > 1)
                        expense_ptr->setNote(Utils::concatTokens(tokens, 1, tokens.size()));
                    break;

                case Action_Exp::Rm_Part:
                    removeExpParticipants(std::vector<std::string>(++tokens.begin(), tokens.end()), *expense_ptr);
                    break;

                case Action_Exp::CG_WG: 
                {
                    std::cout << "change weight " << std::endl;
                    std::vector<std::pair<std::string, int>> weight_pairs;
                    for (int pos = 2; pos < tokens.size(); pos += 2) {
                        if (!expense_ptr->hasParticipant(tokens[pos - 1]))
                        {
                            std::cerr << "Unknown participant: " << tokens[pos - 1] <<std::endl;
                            std::cerr << "To add participants, use --add-p" <<std::endl;
                            break;
                        }
                        int newWeight = 1;
                        //make sure the weight value is correct
                        try {
                            newWeight = std::stoi(tokens[pos]);
                            weight_pairs.push_back(std::make_pair(tokens[pos - 1], newWeight));
                        } catch(const std::invalid_argument&) {
                            std::cerr << "invalid weight value: " << tokens[pos] << std::endl;
                            break;
                        }
                    }
                    expense_ptr->changeWeights(weight_pairs);
                    break;
                }

                case Action_Exp::Show:
                    expense_ptr->printExpenseTitle();
                    expense_ptr->printExpenseWeight();
                    break;

                case Action_Exp::Commit:
                    commitExpense(expense_ptr);
                    run = false;
                    break;

                case Action_Exp::Help:
                    break;

                case Action_Exp::Quit:
                    std::cout << std::endl;
                    run = false;
                    break;

                case Action_Exp::Undefine:
                    break;
            }
        }

    }
}


