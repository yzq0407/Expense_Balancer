#include "../src/Expense.h"
#include "../src/Optimizer.h"

using namespace AccountBalancer;
namespace {
    std::string YJ = "You_Jie";
    std::string LYM = "Lu_YiMing";
    std::string NM = "Ni_Min";
    std::string LQ = "Li_Qiao";
    std::string OP = "Sun_HaoChen";
    std::string ZCJ = "Zheng_ChengJian";
    std::string MY = "MingYan";
    std::string YZQ = "Yang_ZheQin";
    std::string GJ = "Gao_Jian";
    std::string HYS = "Hao_YingShuai";
    std::string JT = "Jian_Tong";
    const std::vector<std::string> all_people = {YJ, LYM, NM, LQ, OP, ZCJ, MY, YZQ, GJ, HYS
        , JT};

    inline std::vector<std::shared_ptr<Expense>> createExpenses() {
        std::vector<std::shared_ptr<Expense>> res;


        auto day_1_housing = std::make_shared<Expense>(NM, 346.0, "Day 1 Housing");
        day_1_housing->addParticipant({YJ, LYM, NM, LQ, OP, ZCJ, MY, YZQ, GJ});

        auto day_2_housing = std::make_shared<Expense>(NM, 346.0, "Day 2 Housing");
        day_2_housing->addParticipant({YJ, LYM, NM, LQ, OP, ZCJ, MY, YZQ, HYS, JT});

        auto day_3_housing = std::make_shared<Expense>(NM, 346.0, "Day 3 Housing");
        day_3_housing->addParticipant({YJ, LYM, NM, LQ, OP, YZQ, HYS, JT});
        
        auto day_1_lift_ticket = std::make_shared<Expense>(LYM, 475.0, "Day 1 Ski Lift Ticket");
        day_1_lift_ticket->addParticipant({YJ, LYM, NM, LQ, OP, ZCJ, MY});

        auto day_2_lift_ticket = std::make_shared<Expense>(LYM, 95.0, "Day 2 Ski Lift Ticket");
        day_2_lift_ticket->addParticipant({LYM, NM, ZCJ, MY, HYS, JT});

        auto day_3_lift_ticket = std::make_shared<Expense>(LYM, 182.0, "Day 3 Ski Lift Ticket");
        day_3_lift_ticket->addParticipant({LYM, NM, ZCJ, MY, HYS, JT, OP, YJ});

        const std::vector<std::pair<std::string, int>> ski_days ({{YJ, 2},
                {LYM, 3}, {NM, 3}, {LQ, 1}, {OP, 2}, {ZCJ, 3}, {MY, 3}, 
                {HYS, 2}, {JT, 2}});

        auto season_pass = std::make_shared<Expense>(YZQ, 180.0, "Season passes rental");
        season_pass->addParticipant({YJ, LYM, NM, LQ, OP, ZCJ, MY});
        season_pass->removeParticipant({YZQ});
        season_pass->changeWeights(ski_days);
        

        auto car_rental = std::make_shared<Expense>(YZQ, 652.72, "Car rental");
        car_rental->addParticipant({YZQ, LYM, NM, OP, YJ, LQ});

        auto gas_car_rental = std::make_shared<Expense>(YZQ, 45.46 + 52.66, "Gas of rental car");
        gas_car_rental->addParticipant({YZQ, LYM, NM, OP, YJ, LQ});

        auto season_pass_mail = std::make_shared<Expense>(LYM, 9.28, "Season passes mailing");
        season_pass_mail->addParticipant({YJ, LYM, NM, LQ, OP, ZCJ, MY});
        season_pass_mail->changeWeights(ski_days);
        
        auto ben_jerry_ticket = std::make_shared<Expense>(LYM, 32.0, "Ben&Jerry tickets");
        ben_jerry_ticket->addParticipant({YJ, LYM, NM, LQ, OP, YZQ, HYS, JT});

        std::vector<std::pair<std::string, int>> stays_weight = 
        {{YJ, 3}, {LYM, 3}, {NM, 3}, {LQ, 3}, {OP, 3}, {ZCJ, 2}, {MY, 2}, {YZQ, 3}, {GJ, 1}
            , {HYS, 2}, {JT, 2}};

        auto sams = std::make_shared<Expense>(ZCJ, 129.85, "Sam's purchases");
        sams->addParticipant(all_people);
        sams->changeWeights(stays_weight);
        
        auto water = std::make_shared<Expense>(HYS, 3.79, "Drink water purchase");
        water->addParticipant(all_people);
        water->changeWeights(stays_weight);
        
        auto chinese_sm = std::make_shared<Expense>(LYM, 290.18+19.79, "Chinese Supermarket purchases");
        chinese_sm->addParticipant(all_people);
        chinese_sm->changeWeights(stays_weight);
        
        auto cai = std::make_shared<Expense>(YJ, 61.90, "Groceries purchases");
        cai->addParticipant(all_people);
        cai->changeWeights(stays_weight);

        res.push_back(day_1_housing);
        res.push_back(day_2_housing);
        res.push_back(day_3_housing);
        res.push_back(day_1_lift_ticket);
        res.push_back(day_2_lift_ticket);
        res.push_back(day_3_lift_ticket);
        res.push_back(season_pass);
        res.push_back(car_rental);
        res.push_back(gas_car_rental);
        res.push_back(season_pass_mail);
        res.push_back(ben_jerry_ticket);
        res.push_back(sams);
        res.push_back(water);
        res.push_back(chinese_sm);
        res.push_back(cai);
        return res;
    }
} //anonymous namespace

int main() {
    auto expenses = createExpenses();
    AccountBalancer::BalanceOptimizer op;
    for (auto& expense: expenses) {
        expense->printExpenseSummary();
        std::cout << std::endl;
    }

    op.optimizeExpenses(expenses, OptimizerStrategy::LEAST_TRANSFER);

    for (auto& name: all_people) {
        op.printParticipantSummary(name);
    }
}
