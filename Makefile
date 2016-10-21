
ROOT=/opt/clang_llvm_3.9.0
CLANG=$(ROOT)/bin/clang++
FLAGS= -fcolor-diagnostics -std=c++14

calc: calc.cpp
	@rm -rf $@
	$(CLANG) $(FLAGS) -lncurses $^ -o $@

clean:
	@-rm -rf calc
