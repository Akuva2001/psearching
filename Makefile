CXX   = g++

EXE = psearch
CXXFLAGS = -pthread

SRC = ./src
OBJ = ./obj

SOURCES := $(wildcard src/*)
OBJECTS := ${SOURCES:%=$(OBJ)/%.o}

.PHONY: all

all: $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(OBJECTS) -o $(EXE)

$(OBJ)/%.cpp.o: %.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CXXFLAGS) -I./include -c $< -o $@

.PHONY: clean
clean:
	rm -rf obj/*
	rm -f $(EXE)