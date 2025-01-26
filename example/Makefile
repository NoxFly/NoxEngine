BINDIR		:= ./bin
BUILDDIR	:= ./build/$(TARGET)
LIB_NAME	:= NoxEngine

SRC 		:= $(wildcard $(TARGET)/*.cpp)
EXECUTABLE	:= $(BINDIR)/debug/$(TARGET)

ROOT_DIR	:=$(realpath $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))/..)
LIB_DIR		:= $(ROOT_DIR)/bin/lib

CFLAGS		:= -std=c++20 $(DFLAGS)
LDFLAGS		:= -std=c++20

LIBS		:= $(LIB_NAME) SDL2 SDL2main SDL2_image GL GLEW

INC			:= -I$(LIB_DIR)/include/$(LIB_NAME)
LIB_INC		:= -L$(LIB_DIR)


export LD_LIBRARY_PATH	:= $(LIB_DIR)

ifeq ($(CXX), g++)
	CC := g++
else
	CC := clang++
endif

LIBS		:= $(patsubst %, -l%, $(LIBS))
OBJECTS		:= $(patsubst $(TARGET)/%, $(BUILDDIR)/%, $(SRC:.cpp=.o))


$(EXECUTABLE): $(OBJECTS)
	@echo "Linking and executing..."
	@mkdir -p $(BINDIR)/debug
	@$(CC) $(LIB_INC) $(LDFLAGS) -o $(EXECUTABLE) $^ $(LIBS)

$(BUILDDIR)/%.o: $(TARGET)/%.cpp
	@echo "Compiling example $(TARGET): $<..."
	@mkdir -p $(BUILDDIR)
	@$(CC) $(INC) $(LIB_INC) $(CFLAGS) -c -o $@ $<

2D:
	@$(MAKE) -s TARGET=base_example_2D
	@$(BINDIR)/debug/base_example_2D

3D:
	@$(MAKE) -s TARGET=base_example_3D
	@$(BINDIR)/debug/base_example_3D

clean:
	rm -f $(BINDIR)/debug/*


.PHONY: clean 2D 3D