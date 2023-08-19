BINDIR		:= ./bin
BUILDDIR	:= ./build/$(TARGET)

SRC 		:= $(wildcard $(TARGET)/*.cpp)
EXECUTABLE	:= $(BINDIR)/debug/$(TARGET)

ROOT_DIR	:=$(realpath $(shell dirname $(realpath $(firstword $(MAKEFILE_LIST))))/..)
LIB_DIR		:= $(ROOT_DIR)/bin/lib

CFLAGS		:= -std=c++20 $(DFLAGS)
LDFLAGS		:= -std=c++20

LIBS			:= NoxEngine SDL3 SDL3_image
UNIX_LIBS 		:= GL GLEW 
WIN_LIBS 		:= mingw32 opengl32 glew32

INC			:= -I$(LIB_DIR)/include/NoxEngine
LIB_INC		:= -L$(LIB_DIR)

export LD_LIBRARY_PATH	:= $(LIB_DIR)

# detect if compiler is gcc instead of clang. Not viewing for other compiler
ifeq ($(CXX), g++)
	CC := g++
else
	CC := clang++
endif # C++ : clang++ or g++

# OS DETECTION
SYS := $(shell $(CC) -dumpmachine)
OS	= WINDOWS

ifneq (, $(findstring linux, $(SYS)))
	OS := LINUX
else ifneq (, $(findstring mingw, $(SYS)))
	OS := WINDOWS
else ifneq (, $(findstring cygwin, $(SYS)))
	OS := MACOS
endif


# Add window's extra libs paths
ifeq (WINDOWS, $(OS))
	INC += -I$(ROOT_DIR)/libs/WIN32/include
	LIB_INC += -L$(ROOT_DIR)/libs/WIN32/lib

	LIBS := $(WIN_LIBS) $(LIBS)
	EXECUTABLE := $(EXECUTABLE).exe
else
	LIBS := $(UNIX_LIBS) $(LIBS)
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
	@cd $(BINDIR); ./debug/base_example_2D

3D:
	@$(MAKE) -s TARGET=base_example_3D
	@cd $(BINDIR); ./debug/base_example_3D

clean:
	rm -f $(BINDIR)/debug/*


.PHONY: clean 2D 3D