
ROOT		= /opt/clang_llvm_3.9.0
CXX			= $(ROOT)/bin/clang++
CXXFLAGS	= -fcolor-diagnostics -std=c++14 -Wall -Wpedantic
LDFLAGS		= -lncurses
TARGET		= calc

.PHONY: clean

SRC		= $(wildcard $(PWD)/*.cpp)
OBJS 	= $(SRC:.cpp=.o)
DEPS	= $(SRC:.cpp=.d)


$(TARGET): $(OBJS)
	@rm -rf $@
	@echo [linking] $@
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LDFLAGS)

-include $(DEPS)

%.o : %.cpp
	@echo [build] $@
	@$(CXX) $(CXXFLAGS) -MMD -c $< -o $@ 


clean:
	@-rm -rf $(TARGET) $(OBJS) $(DEPS)


# add headers later
