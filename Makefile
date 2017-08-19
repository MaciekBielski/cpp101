
ROOT		= /opt/clang_llvm_3.9.0
CXX			= $(ROOT)/bin/clang++
CXXFLAGS	= -fcolor-diagnostics -std=c++14 -stdlib=libc++ -Wall -Wpedantic
LDFLAGS		= -lncurses -lpthread
TARGET		= calc

.PHONY: clean

SRC		:= $(wildcard *.cpp)
OBJS	:= $(SRC:.cpp=.o)
DEPS	:= $(SRC:.cpp=.d)

all: $(TARGET)

$(TARGET): $(OBJS)
	@/bin/rm -rf $@
	@echo [linking] $@
	@$(CXX) $(CXXFLAGS) $(OBJS) -o $@ $(LDFLAGS)

-include $(DEPS)

%.o : %.cpp
	@echo [build] $@
	$(CXX) $(CXXFLAGS) -MMD -c $< -o $@

clean:
	@echo [cleaning] $(SRC)
	@-/bin/rm -rf $(TARGET) $(OBJS) $(DEPS)

# add headers later
