# modify these 3 lines depending of what you want
CFLAGS 		:= -Werror -Wall -Wextra
LDFLAGS		:= -lmingw32
LIBS 		:= -lopengl32 -lglew32 -lSDL2main -lSDL2 -lSDL2_image
# type of source files
# c or cpp (make sure to not have space after)
SRCEXT 		?= cpp
CVERSION	?= 11
CPPVERSION	?= 17

OSX ?= linux

# detect if compiler is gcc instead of clang. Not viewing for other compiler
# C
ifeq ($(SRCEXT), c)
	ifeq ($(CC), gcc)
		CC := gcc
	else
		CC := clang
	endif # C : clang or gcc
	CFLAGS += -std=c$(CVERSION)
	LDFLAGS += -std=c$(CVERSION)
# C++
else
	ifeq ($(CXX), g++)
		CC := g++
	else
		CC := clang++
	endif # C++ : clang++ or g++
	CFLAGS += -std=c++$(CPPVERSION)
	LDFLAGS += -std=c++$(CPPVERSION)
endif

CFLAGS += -D$(OSX)

# executable name
ifdef PGNAME
	EXECUTABLE = $(PGNAME)
else
	EXECUTABLE 	:= program
endif # pgname

# program name location
OUT 		?= ./bin

# compilation mode
ifdef DEBUG
	TARGETDIR = $(OUT)/debug
else
	TARGETDIR = $(OUT)/release
endif # debug

# final full executable location
TARGET 		:= $(TARGETDIR)/$(EXECUTABLE)
# .o location
BUILDDIR 	?= ./build
# source files location
SRCDIR 		?= ./src
# header files location
INCDIR 		?= ./include

LIBDIR		?= ./libs

SOURCES 	:= $(shell find $(SRCDIR)/** -type f -name *.$(SRCEXT))

BUILDLIST	:=
INC			:=

ifneq (, $(firstword $(wildcard $(INCDIR)/*)))
	INCDIRS 	:= $(shell find $(INCDIR)/** \( -name '*.h' -o -name '*.hpp' \) -exec dirname {} \; | sort | uniq)
	INCLIST 	:= $(patsubst $(INCDIR)/%, -I $(INCDIR)/%, $(INCDIRS))
	BUILDLIST 	:= $(patsubst $(INCDIR)/%, $(BUILDDIR)/%, $(INCDIRS))
	INC 		:= $(INCLIST)
endif # incdir

ifneq ($(SRCDIR), $(INCDIR))
	INC += -I $(INCDIR)
endif

ifneq (, $(LIBDIR))
	ifneq ($(wildcard $(LIBDIR)/include),)
		INC += -I $(LIBDIR)/include
	endif

	ifneq ($(wildcard $(LIBDIR)/lib),)
		LDFLAGS += -L $(LIBDIR)/lib
	endif
endif



ifdef DEBUG
OBJECTS 	:= $(patsubst $(SRCDIR)/%, $(BUILDDIR)/%, $(SOURCES:.$(SRCEXT)=.o))

$(TARGET): $(OBJECTS)
	@mkdir -p $(TARGETDIR)
	@echo -e "\033[0;35mLinking...\033[0;90m"
	@echo "  Linking $(TARGET)"
	@$(CC) -g -o $(TARGET) $^ $(LDFLAGS) $(LIBS)

$(BUILDDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@mkdir -p $(BUILDDIR)
ifdef BUILDLIST
	@mkdir -p $(BUILDLIST)
endif
	@echo "Compiling $<...";
	@$(CC) $(CFLAGS) -DDEBUG $(INC) -c $< -o $@

else # RELEASE

$(TARGET):
	@mkdir -p $(TARGETDIR)
	@echo "Linking $(TARGET)..."
	@$(CC) $(INC) -o $(TARGET) $(SOURCES) $(LIBS) $(LDFLAGS)

endif #debug / release targets


clean:
	rm -f -r $(BUILDDIR)/**
	@echo "All objects removed"

clear: clean
	rm -f -r  $(OUT)/**/$(EXECUTABLE)
	@echo "$(EXECUTABLE) removed from $(OUT) folder"

.PHONY: clean clear