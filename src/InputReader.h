#ifndef __BALANCE_INPUTREADER_H
#define __BALANCE_INPUTREADER_H

#include <iostream>
#include <map>

#include "BalanceMap.h"
namespace AccountBalancer {
    enum Status {OK, ERROR, UNCHANGE};
    enum Option {Commit, Peek, Undo, Abort, Help};

    class InputReader {
    public:
        using WeightCommit = std::pair<std::vector<std::string>, int>;

    private:
        struct InputReaderImpl;

        InputReaderImpl* pimpl;

        InputReader();

        void printWeightMap(const std::map<std::string, int>&) const;

        void printWeightHint() const;

        void undoWeightCommit() const;
        
        Status parseCommit(const std::string& input, WeightCommit&) const;

    public:

        InputReader(const InputReader&) = delete;
        InputReader& operator=(const InputReader&) = delete;

        ~InputReader();

        //singleton method
        static InputReader& getReader();

        //accessors
        bool hasHint() const;

        int startWeight() const;

        //modifiers
        void diableHint();

        void enableHint();

        //read into map
        Status readAllNames(BalanceMap&) const;

        Status readSharedExpense(BalanceMap&) const;

    };
}
#endif
