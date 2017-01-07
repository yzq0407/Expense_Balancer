//This class optimize balance transfers given bunch of expenses
//Created by Theodore Yang on 1/5/2017
#include <chrono>
#include <algorithm>

#include "Optimizer.h"
#include "Expense.h"

namespace {
    //helper method to calculate the gaps
    //which is defined to be the absolute different of payment being made by a participant
    //and the amount he/she should spend
    void getExpenseGaps (
            const std::map<std::string, AccountBalancer::TransferSummary>& personalExpenses,
            std::vector<std::pair<std::string, double>>& creditor_gaps,
            std::vector<std::pair<std::string, double>>& debtor_gaps) {
        //process each participant
        for (auto it = personalExpenses.cbegin(), last = personalExpenses.cend();
                it != last; ++it) {
            auto name = it->first;
            double payment_made = it->second.getPaymentMadeValue();
            double total_expense = it->second.getTotalExpense();
            double gap = payment_made - total_expense;
            if (AccountBalancer::Utils::isGreater(gap, 0.0)) {
                creditor_gaps.push_back(std::make_pair(name, gap));
            }
            else if (AccountBalancer::Utils::isLess(gap, 0.0)) {
                debtor_gaps.push_back(std::make_pair(name, -gap));
            }
            //ignore person whose gap is 0, they do not need to make transfers
        }
        //sort the gaps, based on the gap value
        auto comparator = [](const std::pair<std::string, double>& p1, 
                const std::pair<std::string, double>& p2) -> bool {
            return p1.second < p2.second;
        };
        std::sort(creditor_gaps.begin(), creditor_gaps.end(), comparator);
        std::sort(debtor_gaps.begin(), debtor_gaps.end(), comparator);
    }

    bool findSubsetSum(double target, std::vector<std::pair<std::string, double>>& pool, 
            int pos, long long& mask) {
        //if the target is already 0, apparently the subset represent by mask is a valid answer
        if (AccountBalancer::Utils::isEqual(target, 0.0)) return true;
        //since the pool is sorted, we can stop search faster
        if (pos >= pool.size() || AccountBalancer::Utils::isGreater(pool[pos].second, target)) return false;
        //either we take pool[pos], or not
        if (!AccountBalancer::Utils::isEqual(pool[pos].second, 0.0)) {
            //search taking pool[pos]
            double temp = pool[pos].second;
            pool[pos].second = 0.0;
            //flip mask value
            mask |= (1 << pos);
            if (findSubsetSum(target - temp, pool, pos + 1, mask)) {
                return true;
            }
            //back track
            mask |= (1 << pos);
            pool[pos].second = temp;
        }
        //search not taking pool[pos]
        return findSubsetSum(target, pool, pos + 1, mask);
    }

    //transfer comparator, used to sort all the transfers
    /* auto transfer_comparator_out_first = [] (const AccountBalancer::Transfer& t1, */ 
    /*         const AccountBalancer::Transfer& t2) -> bool { */
    /*     return t1.amount != t2.amount? t1.amount < t2.amount: t1.other < t2.other; */
    /* }; */

    /* auto transfer_comparator_in_first = [] (const AccountBalancer::Transfer& t1, */ 
    /*         const AccountBalancer::Transfer& t2) -> bool { */
    /*     return t2.amount != t1.amount? t2.amount < t1.amount: t1.other < t2.other; */
    /* }; */

    void printTransfer(const AccountBalancer::Transfer& transfer) {
        if (transfer.amount < 0) {
            printf("Send       $%-8.2fto%20s\n", -transfer.amount, transfer.other.c_str());
        }
        else if (transfer.amount > 0) {
            printf("Receive    $%-8.2ffrom%18s\n", transfer.amount, transfer.other.c_str());
        }
    }

} //annoymous namespace

namespace AccountBalancer {
    //transfer operator

    //optimizer implimentations
    struct BalanceOptimizer::BalanceOptimizerImpl {
        bool verbose = false;
        //the time point with high resolution, which is kept at record
        //to see if we need to update optimization
        std::chrono::time_point<std::chrono::system_clock> last_optimize_time = 
            std::chrono::system_clock::now();

        //a map from name to tansferSummary, support orderer output
        std::map<std::string, TransferSummary> result;
    };

    //helper functions
    OptimizerStatus BalanceOptimizer::leastTransferOptimize() {
        //get the gaps for both creditors and debtors
        //for definition of gaps, see function definition
        std::vector<std::pair<std::string, double>> creditor_gaps;
        std::vector<std::pair<std::string, double>> debtor_gaps;
        getExpenseGaps(pimpl->result, creditor_gaps, debtor_gaps);
        //least transfers require us to find whether there is a subset sum 
        //from debtor_gaps to each gap values in creditor_gaps
        //and vice versa, in this case, generate a bit mask to represent whether there is an 
        //subset 
        long long mask = 0;
        for (int pos = 0; pos < creditor_gaps.size(); ++pos) {
            if (findSubsetSum(creditor_gaps[pos].second, debtor_gaps, 0, mask)) {
                //put the subset represented by mask into transfers
                creditor_gaps[pos].second = 0.0;
                const std::string creditor = creditor_gaps[pos].first;
                for (int pos_debtor = 0; pos_debtor < debtor_gaps.size() && mask != 0; ++pos_debtor) {
                    if ((1 << pos_debtor) & mask) {
                        const std::string debtor = debtor_gaps[pos_debtor].first;
                        double transfer_amount = debtor_gaps[pos_debtor].second;
                        mask |= (1<< pos_debtor);
                        pimpl->result.at(creditor).addTransfer(Transfer(debtor,
                                    transfer_amount));
                        pimpl->result.at(debtor).addTransfer(Transfer(creditor,
                                    -transfer_amount));
                    }
                }
            }
        }
        for (int pos = 0; pos < debtor_gaps.size(); ++pos) {
            if (!Utils::isEqual(debtor_gaps[pos].second, 0.0) &&
                    findSubsetSum(debtor_gaps[pos].second, creditor_gaps, 0, mask)) {
                debtor_gaps[pos].second = 0.0;
                const std::string debtor = debtor_gaps[pos].first;
                for (int pos_creditor = 0; pos_creditor < creditor_gaps.size() && mask != 0; ++pos_creditor) {
                    if ((1 << pos_creditor) & mask) {
                        mask |= (1 << pos_creditor);
                        const std::string creditor = creditor_gaps[pos_creditor].first;
                        double transfer_amount = creditor_gaps[pos_creditor].second;
                        pimpl->result.at(creditor).addTransfer(Transfer(debtor, 
                                    transfer_amount));
                        pimpl->result.at(debtor).addTransfer(Transfer(creditor,
                                    -transfer_amount));
                    }
                }
            }
        }
        //now doing lazy matching
        int pos_c = 0, pos_d = 0;
        while (pos_c < creditor_gaps.size() && pos_d < debtor_gaps.size()) {
            const std::string& creditor = creditor_gaps[pos_c].first;
            double creditor_gap = creditor_gaps[pos_c].second;

            const std::string& debtor = debtor_gaps[pos_d].first;
            double debtor_gap = debtor_gaps[pos_d].second;
            if (Utils::isEqual(creditor_gap, 0.0)) {
                ++pos_c;
                continue;
            }
            if (Utils::isEqual(debtor_gap, 0.0)) {
                ++pos_d;
                continue;
            }
            if (Utils::isGreater(creditor_gap, debtor_gap)) {
                pimpl->result.at(creditor).addTransfer(Transfer(debtor, debtor_gap));
                pimpl->result.at(debtor).addTransfer(Transfer(creditor, -debtor_gap));
                creditor_gaps[pos_c].second -= debtor_gaps[pos_d++].second;
            }
            else if (Utils::isGreater(debtor_gap, creditor_gap)) {
                pimpl->result.at(creditor).addTransfer(Transfer(debtor, creditor_gap));
                pimpl->result.at(debtor).addTransfer(Transfer(creditor, -creditor_gap));
                debtor_gaps[pos_d].second -= creditor_gaps[pos_c++].second;
            }
            else {
                pimpl->result.at(creditor).addTransfer(Transfer(debtor, creditor_gap));
                pimpl->result.at(debtor).addTransfer(Transfer(creditor, -debtor_gap));
                ++pos_c;
                ++pos_d;
            }
        }
        return OptimizerStatus::SUCCESS;
    }

    //the lazy optimization, do not try to perfectly match participants expense
    OptimizerStatus BalanceOptimizer::lazyOptimize() {
        std::vector<std::pair<std::string, double>> creditor_gaps;
        std::vector<std::pair<std::string, double>> debtor_gaps;
        getExpenseGaps(pimpl->result, creditor_gaps, debtor_gaps);
        //since this is a lazy optimization, match each pair greedily
        int pos_c = 0, pos_d = 0;
        while (pos_c < creditor_gaps.size() && pos_d < debtor_gaps.size()) {
            const std::string& creditor = creditor_gaps[pos_c].first;
            double creditor_gap = creditor_gaps[pos_c].second;

            const std::string& debtor = debtor_gaps[pos_d].first;
            double debtor_gap = debtor_gaps[pos_d].second;
            if (Utils::isEqual(creditor_gap, 0.0)) {
                ++pos_c;
                continue;
            }
            if (Utils::isEqual(debtor_gap, 0.0)) {
                ++pos_d;
                continue;
            }
            if (Utils::isGreater(creditor_gap, debtor_gap)) {
                pimpl->result.at(creditor).addTransfer(Transfer(debtor, debtor_gap));
                pimpl->result.at(debtor).addTransfer(Transfer(creditor, -debtor_gap));
                creditor_gaps[pos_c].second -= debtor_gaps[pos_d++].second;
            }
            else if (Utils::isGreater(debtor_gap, creditor_gap)) {
                pimpl->result.at(creditor).addTransfer(Transfer(debtor, creditor_gap));
                pimpl->result.at(debtor).addTransfer(Transfer(creditor, -creditor_gap));
                debtor_gaps[pos_d].second -= creditor_gaps[pos_c++].second;
            }
            else {
                pimpl->result.at(creditor).addTransfer(Transfer(debtor, creditor_gap));
                pimpl->result.at(debtor).addTransfer(Transfer(creditor, -debtor_gap));
                ++pos_c;
                ++pos_d;
            }
        }
        return OptimizerStatus::SUCCESS;
    }

    BalanceOptimizer::BalanceOptimizer(): pimpl(std::make_unique<BalanceOptimizerImpl>()) {}

    BalanceOptimizer::~BalanceOptimizer() = default;

    bool BalanceOptimizer::isUpToTime (
            const std::chrono::time_point<std::chrono::system_clock>& time_point) const {
        return pimpl->last_optimize_time > time_point;
    }

    OptimizerStatus BalanceOptimizer::optimizeExpenses(
            const std::vector<std::shared_ptr<Expense>>& expenses,
            OptimizerStrategy strategy) {
        pimpl->result.clear();
        //process each expenses
        for (auto& expense: expenses) {
            auto weight_map = expense->getWeightsMap();
            double amount = expense->getAmount();
            double weight_sum = static_cast<double> (expense->getWeightSum());
            for (auto it = weight_map.begin(), last = weight_map.end();
                    it != last; ++it) {
                std::string name = it->first;
                int share = it->second;
                
                if (pimpl->result.find(name) == pimpl->result.end()) {
                    pimpl->result.emplace(name, name);
                }
                //this method might throw exception, but since we construct it before,
                //it should be fine
                pimpl->result.at(name).addExpense(expense);

                //the expense need to add to everybody's account
                pimpl->result.at(name).getTotalExpense() += amount * share / weight_sum;
            }
            //now the creditor of this expense has to be added into payment
            auto creditor = expense->getCreditor();
            if (pimpl->result.find(creditor) == pimpl->result.end()) {
                pimpl->result.emplace(creditor, creditor);
            }
            pimpl->result.at(creditor).getPaymentMadeValue() += amount;
            pimpl->result.at(creditor).addPayment(expense);
        }
        switch (strategy) {
            case OptimizerStrategy::LEAST_TRANSFER:
                return leastTransferOptimize();
            case OptimizerStrategy::LAZY:
                return lazyOptimize();
        }
    }

    OptimizerStatus BalanceOptimizer::printParticipantTransfers(const std::string& name) const {
        if (pimpl->result.find(name) == pimpl->result.end()) {
            return OptimizerStatus::NAME_NOT_FOUND;
        }
        const TransferSummary summary = pimpl->result.at(name);
        if (summary.getTransfers().empty()) {
            std::cout << "No Money Transfer Needed" << std::endl;
        }
        else {
            std::cout << "Suggested Transfers: " << std::endl;
            for (const Transfer& transfer: summary.getTransfers()) {
                printTransfer(transfer);
            }
        }
        return OptimizerStatus::SUCCESS;
    }


    OptimizerStatus BalanceOptimizer::printParticipantExpenses(const std::string& name) const {
        if (pimpl->result.find(name) == pimpl->result.end()) {
            return OptimizerStatus::NAME_NOT_FOUND;
        }
        const TransferSummary summary = pimpl->result.at(name);
        
        std::cout << "Expense Breakdown " << std::endl;
        for (auto expense_wptr: summary.getExpenses()) {
            if (expense_wptr.expired())  return OptimizerStatus::OUT_OF_TIME;
            auto expense_sptr = expense_wptr.lock();
            //all the attributes
            double total_amount = expense_sptr->getAmount();
            int share =expense_sptr->getWeight(name);
            int total_weight = expense_sptr->getWeightSum();
            double amount = total_amount * static_cast<double>(share) / static_cast<double>(total_weight);
            printf("$%-8.2f%-30s(%d out of %d)\n", amount, expense_sptr->getNote().c_str()
                    , share, total_weight);
        }
        printf("Total amount of expense:    $%-.2f\n", summary.getTotalExpense());
        std::cout << std::endl;

        auto expense_paid = summary.getPayments();
        if (!expense_paid.empty()) {
            std::cout << "Expense paid by " << name << std::endl;
            for (auto& expense_wptr: expense_paid) {
                auto expense_sptr = expense_wptr.lock();
                double total_amount = expense_sptr->getAmount();
                printf("$%-8.2f%-15s\n", total_amount, expense_sptr->getNote().c_str());
            }
            printf("Total payment made:         $%-.2f\n", summary.getPaymentMadeValue());
        }
        else {
            std::cout << "No payment was made by " << name << std::endl;
        }
        return OptimizerStatus::SUCCESS;
    }

    OptimizerStatus BalanceOptimizer::printParticipantSummary(const std::string& name) const {
        if (pimpl->result.find(name) == pimpl->result.end()) {
            return OptimizerStatus::NAME_NOT_FOUND;
        }
        std::cout << std::string(60, '-') << std::endl;
        int padding = (60 - name.size()) / 2 - 4;
        if (padding < 0)    padding = 0;
        std::string leftPadding = std::string(padding, '-');
        std::string rightPadding = name.size() % 2? std::string(padding + 1, '-'): leftPadding;
        std::cout << leftPadding<< std::string(4, ' ') << name 
            << std::string(4, ' ') << rightPadding << std::endl;
        std::cout << std::endl;
        OptimizerStatus return_code = printParticipantExpenses(name);
        if (return_code != OptimizerStatus::SUCCESS) {
            return return_code;
        }
        std::cout << std::endl;
        return_code = printParticipantTransfers(name);
        if (return_code != OptimizerStatus::SUCCESS)
            return return_code;
        std::cout << std::endl;
        return OptimizerStatus::SUCCESS;
    }



    //----------------------TransferSummary class--------------------//
    //transfer summay implementation
    struct TransferSummary::TransferSummaryImpl {
        //name of the person
        const std::string name;
        //all the expenses that is engaged in, use weak pointer since it might dangle
        std::vector<std::weak_ptr<Expense>> expenses;
        //a vector of payment made by this person
        std::vector<std::weak_ptr<Expense>> payments;
        //a list of transfers that are supposed to happen, it is a list of debt, also, use weak_ptr
        std::vector<Transfer> transfers;
        //total expense, this is the expense this person should make
        double total_expense;
        //total payment, this is the payment this person made at the first place
        double payment_made;

        //constructor
        TransferSummaryImpl(const std::string& _name): name(_name), 
                                                       total_expense(0.0),
                                                       payment_made(0.0) {}

        TransferSummaryImpl(std::string&& _name): name(std::move(_name)),
                                                  total_expense(0.0),
                                                  payment_made(0.0) {}
    };

    //ctors
    TransferSummary::TransferSummary(const std::string& name):
        pimpl(std::make_unique<TransferSummaryImpl>(name)) {}

    TransferSummary::TransferSummary(std::string&& name):
        pimpl(std::make_unique<TransferSummaryImpl>(std::move(name))) {}

    //copy ctor and copy assignment operator
    //note that this class have value semantics
    TransferSummary::TransferSummary(const TransferSummary& _other):
        pimpl(std::make_unique<TransferSummaryImpl>(*_other.pimpl)) {}

    TransferSummary& TransferSummary::operator=(const TransferSummary& _other) {
        //strong exception guarantee
        using std::swap;
        TransferSummary temp(_other);
        swap(*this, temp);
        return *this;
    }

    //dtor
    TransferSummary::~TransferSummary() = default;

    void TransferSummary::swap(TransferSummary& other) noexcept {
        using std::swap;
        swap(pimpl, other.pimpl);
    }

    //add one more expense
    void TransferSummary::addExpense(std::shared_ptr<Expense> expense) {
        if (expense) {
            pimpl->expenses.push_back(std::weak_ptr<Expense>(expense));
        }
    }

    //add one more payment made
    void TransferSummary::addPayment(std::shared_ptr<Expense> payment) {
        if (payment) {
            pimpl->payments.push_back(std::weak_ptr<Expense>(payment));
        }
    }

    //add one more transfer
    void TransferSummary::addTransfer(const Transfer& transfer) {
        pimpl->transfers.push_back(transfer);
    }

    void TransferSummary::addTransfer(Transfer&& transfer) {
        pimpl->transfers.push_back(std::move(transfer));
    }

    //get total expense
    double& TransferSummary::getTotalExpense() {
        return pimpl->total_expense;
    }

    const double& TransferSummary::getTotalExpense() const {
        return pimpl->total_expense;
    }

    //get payment made
    double& TransferSummary::getPaymentMadeValue() {
        return pimpl->payment_made;
    }

    const double& TransferSummary::getPaymentMadeValue() const {
        return pimpl->payment_made;
    }

    //get transfers
    std::vector<Transfer>& TransferSummary::getTransfers() {
        return pimpl->transfers;
    }

    const std::vector<Transfer>& TransferSummary::getTransfers() const {
        return pimpl->transfers;
    }

    //get payments
    std::vector<std::weak_ptr<Expense>>& TransferSummary::getPayments() {
        return pimpl->payments;
    }

    const std::vector<std::weak_ptr<Expense>>& TransferSummary::getPayments() const {
        return pimpl->payments;
    }
    //get expenses
    std::vector<std::weak_ptr<Expense>>& TransferSummary::getExpenses() {
        return pimpl->expenses;
    }
    
    const std::vector<std::weak_ptr<Expense>>& TransferSummary::getExpenses() const{
        return pimpl->expenses;
    }
} //AccountBalancer

namespace std {
    template<>
    void swap<AccountBalancer::TransferSummary> (AccountBalancer::TransferSummary& left, 
            AccountBalancer::TransferSummary& right) noexcept {
        left.swap(right);
    }
} //std
