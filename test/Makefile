CC = clang++
CFLAGS = -Wall -O2 -std=c++14
OBJ_PATH = ../obj/

EXECUTABLES = main
OBJECTS = $(OBJ_PATH)utils.o $(OBJ_PATH)expense.o $(OBJ_PATH)test.o $(OBJ_PATH)optimizer.o

$(EXECUTABLES): $(OBJECTS)
	$(CC) $(CFLAGS) -o $(EXECUTABLES) $(OBJECTS)

$(OBJ_PATH)utils.o: ../src/utils.cpp
	$(CC) $(CFLAGS) -o $(OBJ_PATH)utils.o -c ../src/utils.cpp

$(OBJ_PATH)expense.o: ../src/Expense.cpp
	$(CC) $(CFLAGS) -o $(OBJ_PATH)expense.o -c ../src/Expense.cpp

$(OBJ_PATH)optimizer.o: ../src/Optimizer.cpp
	$(CC) $(CFLAGS) -o $(OBJ_PATH)optimizer.o -c ../src/Optimizer.cpp

$(OBJ_PATH)test.o: SkiTest.cpp
	$(CC) $(CFLAGS) -o $(OBJ_PATH)test.o -c SkiTest.cpp

all: $(EXECUTABLES)
	echo All done
clean:
	rm $(EXECUTABLES) $(OBJECTS)
