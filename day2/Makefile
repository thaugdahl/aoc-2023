.PHONY: clean

SOURCES:=$(wildcard *.cpp)
OBJECTS:=$(SOURCES:.cpp=.o)

CXX :=g++

OPTFLAGS:=-g -O0

# ifeq ($(DEBUG),true)
# OPTFLAGS:=-g -O0
# else
# OPTFLAGS:=-O2
# endif

main: $(OBJECTS)
	$(CXX) $(OPTFLAGS) $^ -o $@

main.S: main.cpp
	$(CXX) $(OPTFLAGS) -S -fverbose-asm -o $@ $^

%.o: %.cpp
	$(CXX) $(OPTFLAGS) -c -o $@ $<

clean:
	rm *.o

print:
	echo $(SOURCES)
	echo $(OBJECTS)

