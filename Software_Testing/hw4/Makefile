USER_DIR = .

CXX = /usr/local/bin/afl-g++
# Flags passed to the C++ compiler.
CXXFLAGS += -g -Wall -Wextra -pthread

# All tests produced by this Makefile.  Remember to add new tests you
# created to the list.
TESTS = main

# House-keeping build targets.

all : $(TESTS) afl

clean :
	rm -f $(TESTS)

main : $(USER_DIR)/main.cc $(USER_DIR)/main.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -lpthread $^ -o $@

afl : main
	afl-fuzz -i afl_in -o afl_out ./main
