# MODIFIABLE
CFLAGS 		:= -Werror -Wall -Wextra
LDFLAGS		:=
LIBS		:= SDL2 SDL2main SDL2_image SDL2_ttf
UNIX_LIBS 	:= GL GLEW 
WIN_LIBS 	:= mingw32 opengl32 glew32

# NOT MODIFIABLE
# all what's below must not be modified
# the run.sh is passing all the needed arguments
# you have to do the configuration through the run.sh

# type of source files
# c or cpp (make sure to not have space after)
SRCEXT		?= cpp
HDREXT		?= hpp
CVERSION	?= 17
CPPVERSION	?= 20

LIB_STATIC_EXT := .a
LIB_SHARED_EXT := .so
LIB_SHARED_WIN_EXT := .dll

LIB_STATIC_CFLAGS := 
LIB_SHARED_CFLAGS := -fPIC

LIB_STATIC_LDFLAGS := r
LIB_SHARED_LDFLAGS := -shared -fPIC -o

# program location
OUT			?= ./bin
OUTLIB		:= $(OUT)/lib
EXT			:=


USR_INCDIR := /usr/local/include
USR_LIBDIR := /usr/local/lib

# detect if compiler is gcc instead of clang. Not viewing for other compiler
# C
ifeq ($(SRCEXT), c)
	ifeq ($(CC), gcc)
		CC := gcc
		LDCC := gcc
	else
		CC := clang
		LDCC := clang
	endif # C : clang or gcc
	CFLAGS += -std=c$(CVERSION)
# C++
else
	ifeq ($(CXX), g++)
		CC := g++
		LDCC := g++
	else
		CC := clang++
		LDCC := clang++
	endif # C++ : clang++ or g++
	CFLAGS += -std=c++$(CPPVERSION)
endif

# executable name
ifdef PGNAME
	EXECUTABLE := $(PGNAME)
else
	EXECUTABLE 	:= program
endif # pgname

# compilation mode
ifdef LIB
TARGETDIR := $(OUTLIB)

ifneq ($(OS), WINDOWS)
EXECUTABLE := lib$(EXECUTABLE)
endif

# shared
	ifeq ($(LIB), SHARED)
		EXT = $(LIB_SHARED_EXT)
		CFLAGS += $(LIB_SHARED_CFLAGS)
		LDFLAGS = $(LIB_SHARED_LDFLAGS)

		ifeq ($(OS), WINDOWS)
			EXT = $(LIB_SHARED_WIN_EXT)
		endif
# static
	else ifeq ($(LIB), STATIC)
		EXT = $(LIB_STATIC_EXT)
		CFLAGS += $(LIB_STATIC_CFLAGS)
		LDFLAGS = $(LIB_STATIC_LDFLAGS)
		LDCC := $(AR)
	else
		$(error incorrect LIB value)
	endif
else
# targetdir
	ifdef DEBUG
		TARGETDIR = $(OUT)/debug
		CFLAGS += -g
		LDFLAGS += -o
	else
		TARGETDIR = $(OUT)/release
	endif # debug

# extension
	ifeq ($(OS), MACOS)
		EXT := .app
	else ifeq ($(OS), WINDOWS)
		EXT := .exe
	endif
endif

# final full executable location
TARGET 		:= $(TARGETDIR)/$(EXECUTABLE)$(EXT)
# .o location
BUILDDIR	?= ./build
# source files location
SRCDIR		?= ./src
# header files location
INCDIR		?= ./include

SOURCES 	:= $(shell find $(SRCDIR)/** -type f -name *.$(SRCEXT))

INCDIRS		:=
INCLIST		:=
BUILDLIST	:=
INC			:= -I$(INCDIR)

ifdef MACRO
	CFLAGS		+= $(MACRO)
endif


ifneq (, $(firstword $(wildcard $(INCDIR)/**/*)))
	INCDIRS 	:= $(shell find $(INCDIR)/*/** -type f -name '*.$(HDREXT)' -exec dirname {} \;)
	INCLIST 	:= $(patsubst $(INCDIR)/%, -I$(INCDIR)/%, $(INCDIRS))
	BUILDLIST 	:= $(patsubst $(INCDIR)/%, $(BUILDDIR)/%, $(INCDIRS))
	INC 		+= $(INCLIST)
endif # incdir

# project specific
# windows libs include
ifeq ($(OS), WINDOWS)
	INC += -I./libs/WIN32/include
	LDFLAGS := -L./libs/WIN32/lib $(LDFLAGS)
	LIBS := $(WIN_LIBS) $(LIBS)
else
	LIBS := $(UNIX_LIBS) $(LIBS)
endif


LIBS := $(patsubst %, -l%, $(LIBS))



ifdef DEBUG
OBJECTS 	:= $(patsubst $(SRCDIR)/%, $(BUILDDIR)/%, $(SOURCES:.$(SRCEXT)=.o))

build: $(OBJECTS)
	@mkdir -p $(TARGETDIR)
ifeq ($(VERBOSE), 1)
	@echo "Linking $(TARGET)..."
endif
	@$(LDCC) $(LDFLAGS) $(TARGET) $^ $(LIBS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
ifdef BUILDLIST
	@mkdir -p $(BUILDLIST)
endif
ifeq ($(VERBOSE), 1)
	@echo "Compiling $<...";
endif
	@$(CC) $(INC) $(CFLAGS) -c -o $@ $<

else # RELEASE

build:
	@mkdir -p $(TARGETDIR)
ifeq ($(VERBOSE), 1)
	@echo "Linking..."
endif
	@$(CC) $(INC) -o $(TARGET) $(SOURCES) $(LIBS) $(LDFLAGS)

endif #debug / release targets


# shared
shared:
ifeq ($(VERBOSE), 1)
	@echo "Building shared library..."
endif
	@$(MAKE) -s build LIB=SHARED

static:
ifeq ($(VERBOSE), 1)
	@echo "Building static library..."
endif
	@$(MAKE) -s build LIB=STATIC

install: shared
	@mv $(OUTLIB)/include/ $(USR_INCDIR)
	@mv $(OUTLIB)/lib$(EXECUTABLE).so $(USR_LIBDIR)

clean:
	@rm -f -r $(BUILDDIR)/**
	@echo "All objects removed"

.PHONY: clean clear build shared static