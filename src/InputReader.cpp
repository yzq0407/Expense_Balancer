#include <vector>
#include <map>
#include <stack>
#include "InputReader.h"

namespace AccountBalancer {

    struct InputReader::InputReaderImpl {
        bool hasHint = false;
        int startWeight = 1;
    };

    InputReader::InputReader(): pimpl(new InputReaderImpl) {}

    InputReader::~InputReader() {
        delete pimpl;
    }

    //singleton method
    InputReader& InputReader::getReader() {
        static InputReader reader;
        return reader;
    }

    void InputReader::printWeightMap(const std::map<std::string, int>& weightMap) const {
        for (auto it  = weightMap.begin(), last = weightMap.end();
                it != last; ++it) {
            std::cout << it->first << " (" << it->second << ")  ";
        }
    }

    //helper methods
    void InputReader::printWeightHint() const {
        std::cout << "======================  Help Info  =========================" << std::endl;
        std::cout << "To change a person's share expense weight, type: name,weight" << std::endl;
        std::cout << "  For example, the original weight is: " << std::endl;
        std::cout << "  Andy(1), John(1), Moody(1), Jonathan(1) " << std::endl;
        std::cout << "  Enter: Andy,2" << std::endl;
        std::cout << "  Enter: Jonathan,0" << std::endl;
        std::cout << "  Now the weight map becomes: " << std::endl;
        std::cout << "  Andy(2), John(1), Moody(1), Jonathan(0) " << std::endl;
        std::cout << std::endl;
        std::cout << "Other options:" << std::endl;
        std::cout << "  --commit:   to finish and commit all change" << std::endl;
        std::cout << "  --peek:     see the current weight map" << std::endl;
        std::cout << "  --undo:     undo the previous change" << std::endl;
        std::cout << "  --abort:    abort this expense" << std::endl;
        std::cout << "  --help:     print this message" << std::endl;
        std::cout << std::endl;
        std::cout << "Tips: " << std::endl;
        std::cout << "1. To change every body's weight, type all,weight" << std::endl;
        std::cout << "  For example, enter all,3 will lift everyone's weight to 3 " << std::endl;
        std::cout << "2. To change multiple person's weight at the same time, " <<
            "use brackets to enclose everyone's name" <<std::endl;
        std::cout << "  For example, Enter [Andy John],3 will raise both Andy and John's weight"
            << std::endl;
        std::cout << "===================  Help Info(end) ========================" << std::endl;
    }

    //accessors
    bool InputReader::hasHint() const {
        return pimpl->hasHint;
    }

    int InputReader::startWeight() const {
        return pimpl->startWeight;
    }

    //modifiers
    void InputReader::diableHint() {
        getReader().pimpl->hasHint = false;
    }

    void InputReader::enableHint() {
        getReader().pimpl->hasHint = true;
    }
    
    //read into map
    Status InputReader::readAllNames(BalanceMap& map) const{
        std::cout << "========= Please type all the names, seperated by space ========" <<std::endl;
        std::string input;
        std::vector<std::string> names;
        std::cin >> input;
        int start = 0, end = 0;
        while (end < input.size()) {
            if ((input[end] == ' ' || end == input.size())&& start != end) {
                names.push_back(input.substr(start, end - start));
                start = end + 1;
            }
            ++end;
        }
        std::cout << "===== detected " << names.size() << " names, continue (Y/N) =====" 
            << std::endl;
        std::cin >> input;
        if (input == "Y" || input == "y") {
            map.addFolk(names);
            std::cout << "================= added ======================";
            return Status::OK;
        }
        else {
            std::cout << "================= aborted ====================";
            return Status::UNCHANGE;
        }
    }

    Status InputReader::readSharedExpense(BalanceMap& map) const{
        std::cout << "========= Please type who made the expense ========" <<std::endl;
        std::string input;
        std::cin >> input;
        if (!map.validateName(input)) {
            std::cout << "****** cannot recognize the name: " << input << " ******" << std::endl;
            return Status::UNCHANGE;
        }
        std::string creditor(input);

        std::cout << "========= Please type the amount ==========" <<std::endl;
        double amount;
        std::cin >> amount;
        if (amount < 0) {
            std::cout << "****** amount cannot be negative ********" << std::endl;
            return Status::UNCHANGE;
        }

        std::map<std::string, int> weightMap;
        std::vector<std::string> people = map.getFolks();

        //initialize all the weight 
        for (auto& name: people) {
            if (name != creditor) {
                weightMap[name] = startWeight();
            }
        }
        if (weightMap.size() == 0) {
            std::cout << "      Aborted: nobody can share the expense       " << std::endl;
            return Status::UNCHANGE;
        }
        std::cout << "============= Choose who should share ============" <<std::endl;
        std::cout << "      the current expense share weights are:      " << std::endl;
        //print the weight
        std::cout << std::endl;
        
        if (hasHint())
            printWeightHint();
        else 
            std::cout << "type --help   for more info" << std::endl;
        while (true) {
            std::cin >> input;
            if (input == "--help") {
                printWeightHint();
            }
            else if (input == "--peek") {
                printWeightMap(weightMap);
            }
            else if (input == "--undo") {
                undoWeightCommit();
            }
            else if (input == "--commit") {
            }
            else if (input == "--abort") {
            }
            //not an option
            else {
            }

        }
        return Status::OK;
    }

} //AccountBalancer

