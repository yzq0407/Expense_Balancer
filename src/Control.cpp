//Implementation for the control of whole program
//Created by Theodore Yang on 1/4/2017
#include <iostream>
#include <deque>
#include <set>
#include <unordered_map>

#include "Expense.h"
#include "Control.h"
#include "Optimizer.h"

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

    enum class CommandMain {
        ADD,
        RM,
        SHOW,
        OPT,
        UNDO,
        HELP,
        QUIT
    };

    enum class CommandExpense {
        ADD,
        RM,
        CG,
        SHOW,
        COMMIT,
        HELP,
        QUIT
    };

    enum class Show_Opt_Main {
        Expense,
        Partic,
        Balance,
    };

    const std::unordered_map<std::string, CommandMain> 
        commands_main_map {
            {"add", CommandMain::ADD},
            {"rm", CommandMain::RM},
            {"show", CommandMain::SHOW},
            {"opt", CommandMain::OPT},
            {"undo", CommandMain::UNDO},
            {"help", CommandMain::HELP},
            {"quit", CommandMain::QUIT},
        };

    const std::unordered_map<std::string, CommandExpense>
        commands_expense_map {
            {"add", CommandExpense::ADD},
            {"rm", CommandExpense::RM},
            {"cg", CommandExpense::CG},
            {"show", CommandExpense::SHOW},
            {"commit", CommandExpense::COMMIT},
            {"quit", CommandExpense::QUIT},
            {"help", CommandExpense::HELP}
        };
} //anonymous namespace

namespace AccountBalancer {
    struct Control::ControlImpl {
        std::deque<std::shared_ptr<Expense>> expense_hist;
        std::set<std::string> participants;
        //optimizer contains all the optimization history
        std::unique_ptr<BalanceOptimizer> optimizer;
        ControlStatus status = Main;

        //last expense commit, use to justify whether an optimization result is valid
        std::chrono::time_point<std::chrono::system_clock> last_expense_commit_time = 
            std::chrono::system_clock::now();
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
            pimpl->expense_hist.pop_front();
        }
    }

    //print expense 
    void Control::printExpense() const {
        //TODO
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
        //TODO
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
            //TODO
        } 
    }


    void Control::control_expense(std::shared_ptr<Expense> expense_ptr) {
        std::cout << "============== Expense session ================" << std::endl;
        //TODO
    }
}


