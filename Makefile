CC = clang++
CFLAGS = -Wall -O2 -std=c++14

EXECUTABLES = exp_bal
OBJECTS = src/control.o src/balance_map.o src/utils.o src/expense.o src/main.o

$(EXECUTABLES): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(EXECUTABLES) $(OBJECTS)

src/main.o: src/main.cpp
	$(CC) $(CFLAGS) -o src/main.o -c src/main.cpp

src/control.o: src/Control.cpp
	$(CC) $(CFLAGS) -o src/control.o -c src/Control.cpp

src/balance_map.o: src/BalanceMap.cpp
	$(CC) $(CFLAGS) -o src/balance_map.o -c src/BalanceMap.cpp

src/utils.o: src/utils.cpp
	$(CC) $(CFLAGS) -o src/utils.o -c src/utils.cpp

src/expense.o: src/Expense.cpp
	$(CC) $(CFLAGS) -o src/expense.o -c src/Expense.cpp

all: $(EXECUTABLES)
	echo All done
clean:
	rm $(EXECUTABLES) $(OBJECTS)
