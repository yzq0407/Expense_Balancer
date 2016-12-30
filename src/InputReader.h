#ifndef __BALANCE_INPUTREADER_H
#define __BALANCE_INPUTREADER_H

#include <iostream>

#include "BalanceMap.h"
namespace AccountBalancer {
    enum Status {OK, ERROR, UNCHANGE};

    Status readAllNames(BalanceMap&); 

    Status readSharedExpense(BalanceMap&);
}
#endif
