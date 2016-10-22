
ROOT=/opt/clang_llvm_3.9.0
CLANG=$(ROOT)/bin/clang++
FLAGS= -fcolor-diagnostics -std=c++14

.PHONY: clean

CALC_SRC		= $(wildcard $(PWD)/*.cpp)
CALC_HEADERS 	= $(wildcard $(PWD)/*.hpp)
CALC_OBJS 		= $(patsubst %.cpp, %.o,$(CALC_SRC))

%.o : %.cpp $(shell readlink %.hpp)
	@echo [build] $@
	@$(CLANG) $(FLAGS) -o $@ -c $<

calc: $(CALC_OBJS)
	@rm -rf $@
	@echo [linking] $@
	@$(CLANG) $(FLAGS) -lncurses $(CALC_OBJS) -o $@

clean:
	@-rm -rf calc $(CALC_OBJS)

# add headers later
