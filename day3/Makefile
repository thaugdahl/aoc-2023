.PHONY: clean

SOURCES:=$(wildcard *.cpp) $(wildcard ../common/*.cpp)
OBJECTS:=$(SOURCES:.cpp=.o)

CXX :=g++

OPTFLAGS:=-g -O0 -fsanitize=address,undefined

ifeq ($(DEBUG),true)
OPTFLAGS:=-g -O0
else
OPTFLAGS:=-O2
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

