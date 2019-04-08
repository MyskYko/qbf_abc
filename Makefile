# simple makefile

MKDIR_P = mkdir -p

# main directories
BIN_DIR = ./bin
OBJ_DIR = ./build
SRC_DIR = ./src
INC_DIR = ./include


INC_DIRS = $(shell find $(INC_DIR) -type d)

# compiler and linker options
EXE_NAME = qbf_syn
EXE2_NAME = qbf_parse
SRC2_NAME = meshgen.cpp
CXX = g++

CXX_FLAGS = -Wall -g -std=c++11
INC_FLAGS = $(addprefix -I,$(INC_DIRS))

# collect sources ...
SRCS = $(shell find $(SRC_DIR) -name "*.cpp")
OBJS = $(SRCS:%.cpp=$(OBJ_DIR)/%.o)
#DEPS = $(OBJS:.o=.d)

all: $(BIN_DIR)/$(EXE_NAME) $(BIN_DIR)/$(EXE2_NAME)

# rules for c++ files
$(OBJ_DIR)/%.o: %.cpp
	$(MKDIR_P) $(dir $@)
	$(CXX) $(INC_FLAGS) $(CXX_FLAGS) -c $< -o $@

#rules for target
$(BIN_DIR)/$(EXE_NAME): $(OBJS)
	$(MKDIR_P) $(BIN_DIR)
	$(CXX) $(OBJS) -o $@

$(BIN_DIR)/$(EXE2_NAME): $(SRC2_NAME)
	$(CXX) $(CXX_FLAGS) $(SRC2_NAME) -o $@

.PHONY: clean

clean:
	rm -rf $(OBJ_DIR)/*
	rm -rf $(BIN_DIR)/*
