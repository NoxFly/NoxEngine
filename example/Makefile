BINDIR		:= ./bin
BUILDDIR	:= ./build/$(TARGET)

SRC 		:= $(wildcard $(TARGET)/*.cpp)
EXECUTABLE	:= $(BINDIR)/$(TARGET)

ROOT_DIR	:=$(realpath $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))/..)
LIB_DIR		:= $(ROOT_DIR)/bin/lib

CFLAGS		:= -std=c++20 $(DFLAGS)
LDFLAGS		:= -std=c++20

LIBS 		:= NoxEngine GL GLEW SDL2main SDL2 SDL2_image

INC			:= -I$(LIB_DIR)/include/NoxEngine
LIB_INC		:= -L$(LIB_DIR)

export LD_LIBRARY_PATH	:= $(LIB_DIR)

# detect if compiler is gcc instead of clang. Not viewing for other compiler
ifeq ($(CXX), g++)
	CC := g++
else
	CC := clang++
endif # C++ : clang++ or g++



OBJECTS		:= $(patsubst $(TARGET)/%, $(BUILDDIR)/%, $(SRC:.cpp=.o))



$(EXECUTABLE): $(OBJECTS)
	@mkdir -p $(BINDIR)
	@$(CC) $(LIB_INC) $(LDFLAGS) -o $(EXECUTABLE) $^ $(patsubst %, -l%, $(LIBS))
	$(EXECUTABLE)

$(BUILDDIR)/%.o: $(TARGET)/%.cpp
	@mkdir -p $(BUILDDIR)
	@$(CC) $(INC) $(LIB_INC) $(CFLAGS) -c -o $@ $<

2D:
	@$(MAKE) -s TARGET=base_example_2D

3D:
	@$(MAKE) -s TARGET=base_example_3D

clean:
	rm -f $(EXECUTABLE)


.PHONY: clean 2D 3D