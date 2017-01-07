CC = clang++
CFLAGS = -Wall -O2 -std=c++14

EXECUTABLES = balance
OBJECTS = obj/control.o obj/balance_map.o obj/utils.o obj/expense.o obj/main.o

$(EXECUTABLES): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(EXECUTABLES) $(OBJECTS)

obj/main.o: src/main.cpp
	$(CC) $(CFLAGS) -o obj/main.o -c src/main.cpp

obj/control.o: src/Control.cpp
	$(CC) $(CFLAGS) -o obj/control.o -c src/Control.cpp

obj/balance_map.o: src/BalanceMap.cpp
	$(CC) $(CFLAGS) -o obj/balance_map.o -c src/BalanceMap.cpp

obj/utils.o: src/utils.cpp
	$(CC) $(CFLAGS) -o obj/utils.o -c src/utils.cpp

obj/expense.o: src/Expense.cpp
	$(CC) $(CFLAGS) -o obj/expense.o -c src/Expense.cpp

all: $(EXECUTABLES)
	echo All done
clean:
	rm $(EXECUTABLES) $(OBJECTS)
