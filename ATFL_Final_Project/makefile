# Commands:
#  make - creates CFGtoPDA.exe
#  make clean - erases CFGtoPDA.exe in directory
CC = g++
CFLAGS = -std=c++11

default: CFGtoPDA

CFGtoPDA: Grammar.cpp test.cpp
	$(CC) $(CFLAGS) -o CFGtoPDA test.cpp Grammar.cpp
clean:
	$(RM) CFGtoPDA.exe