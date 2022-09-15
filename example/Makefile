TARGET		:= ./$(TARGET)
SRC 		:= $(wildcard $(TARGET)/*.cpp)
EXECUTABLE	:= $(TARGET)/$(TARGET)

ROOT_DIR	:=$(realpath $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))/..)
LIB_DIR		:= $(ROOT_DIR)/bin/lib

DFLAGS		?=
LDFLAGS		:= -std=c++20
LIBS 		:= -lNoxEngine -lGL -lGLEW -lSDL2main -lSDL2 -lSDL2_image
INC			:= -I$(LIB_DIR)/include/NoxEngine
LIB_INC		:= -L$(LIB_DIR)

export LD_LIBRARY_PATH	:= $(LIB_DIR)

# detect if compiler is gcc instead of clang. Not viewing for other compiler
ifeq ($(CXX), g++)
	CC := g++
else
	CC := clang++
endif # C++ : clang++ or g++

$(EXECUTABLE): clean
	$(CC) $(LDFLAGS) -o $(TARGET)/$(TARGET) $(SRC) $(DFLAGS) $(INC) $(LIB_INC) $(LIBS)
	$(EXECUTABLE)

2D:
	@$(MAKE) -s TARGET=base_example DFLAGS=-D__NOX_ENGINE_2D__

3D:
	@$(MAKE) -s TARGET=base_example DFLAGS=-D__NOX_ENGINE_3D__

clean:
	rm -f $(EXECUTABLE)


.PHONY: clean 2D 3D