CC = clang++#g++
CPPFLAGS += -std=c++11 -lresolv #-Wall -Wextra -W -Werror -Wreturn-type -pedantic -pedantic-errors

EXE := router

SRC_DIR := src
OBJ_DIR := obj
TEST_DIR := tests

SRCS    = $(wildcard $(SRC_DIR)/*.cpp)
OBJS    = $(SRCS:$(SRC_DIR)%.cpp=$(OBJ_DIR)%.o)
TESTS   = $(wildcard $(TEST_DIR)/*.cpp)

#####################################################
.PHONY: all
all:	$(EXE)

$(EXE):	$(OBJS)
	$(CC) $(CPPFLAGS) $^ -o $@


$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp | $(OBJ_DIR)
	$(CC) $(CPPFLAGS) -c $^ -o $@


#$(OBJ_DIR)/myTraceroute.o: $(SRC_DIR)/myTraceroute.cpp $(OBJ_DIR)/receive.o $(OBJ_DIR)/send.o |$(OBJ_DIR)
#	$(CC) $(CPPFLAGS) -c $< -o $@


$(OBJ_DIR):
	mkdir $(OBJ_DIR)

#####################################################
.PHONY: test
test : $(TEST_DIR)/test

$(TEST_DIR)/test : $(TESTS) | $(TEST_DIR)
	$(CC) $(CPPFLAGS) $^ -o $@

.PHONY: run_tests
run_tests : test
	./$(TEST_DIR)/test

$(TEST_DIR):
	mkdir $(TEST_DIR)

#####################################################
.PHONY: clean
clean:
	$(RM) -r $(OBJ_DIR)

.PHONY: distclean
distclean: clean
	$(RM) $(EXE) $(TEST_DIR)/test 

