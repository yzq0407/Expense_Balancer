#include <vector>
#include "InputReader.h"

namespace AccountBalancer {
    Status readAllNames(BalanceMap& map) {
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

    Status readSharedExpenses(BalanceMap& map) {
        std::cout << "========= Please type who made the expense ========" <<std::endl;
        std::string input;
        std::cin >> input;
        if (!map.validateName(input)) {
            std::cout << "****** cannot recognize the name: " << input << "******" << std::endl;
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
        std::cout << "========= Choose who should share ==========" <<std::endl;
        return Status::OK;
    }

} //AccountBalancer

