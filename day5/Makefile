.PHONY: clean

SOURCES:=$(wildcard *.cpp) $(wildcard ../common/*.cpp)
OBJECTS:=$(SOURCES:.cpp=.o)

CXX :=g++

CXXFLAGS:=-std=c++20

OPTFLAGS:=-g -O2 -fsanitize=address $(CXXFLAGS)

ifeq ($(DEBUG),true)
OPTFLAGS:=-g -O0 $(CXXFLAGS)
else
OPTFLAGS:=-O2 $(CXXFLAGS)
endif

INCLUDEFLAGS:=-I$(realpath ../common/)

main: $(OBJECTS)
	$(CXX) $(OPTFLAGS) $^ -o $@

main.S: main.cpp
	$(CXX) $(OPTFLAGS) -S -fverbose-asm -o $@ $^

%.o: %.cpp
	$(CXX) $(OPTFLAGS) $(INCLUDEFLAGS) -c -o $@ $<

clean:
	rm *.o

print:
	echo $(SOURCES)
	echo $(OBJECTS)

run: main
	./main input.txt

test: main
	./main testinput.txt

dbgt: main
	gdb --args ./main testinput.txt
