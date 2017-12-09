CPP_FILES := $(wildcard src/*.cpp)
OBJ_FILES := $(addprefix bin/,$(notdir $(CPP_FILES:.cpp=.o)))
CC_FLAGS := -std=c++11 -O3
RM := rm -f

bin:
	mkdir -p bin
	g++ $(CC_FLAGS) src/ipmt.cpp -o bin/ipmt.o

clean: ; $(RM) $(OBJ_FILES)

.PHONY: bin clean
