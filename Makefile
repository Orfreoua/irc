# This Makefile automates the process of building, compiling, and cleaning the cub3D project.
# It is structured to work with a specific project directory layout and utilizes various tools and libraries.

# Project Configuration:
# - PROJECT_NAME: Name of the project.
# - BIN_NAME: Name of the executable to be generated.
# - CC: Compiler used (gcc).
# - Directory Paths: Variables for source, header, library, build, and binary directories.

# Compiling and Linking:
# - SOURCES: Finds all '.c' source files in the source directory.
# - HEADERS: Finds all '.h' header files in the header directory.
# - LIB: Specifies libraries to link during compilation.
# - OBJECTS, DEPS: Generates object and dependency files from source files.
# - CFLAGS: Compiler flags for error handling, warnings, and defining macros.
# - INC: Include paths for header files.

# Output Formatting:
# - Variables for color formatting in terminal outputs.

# Functions:
# - print_yellow, print_green, print_red: Functions to print messages in different colors in the terminal.

# Build Targets:
# - all: Default build target that compiles libraries, creates directories, compiles object files, and creates the executable.
# - libs: Compiles the required libraries.
# - bonus, clean, fclean, re, prod, fre, debug, release: Utility targets for various build and clean-up processes.

# Directory Creation:
# - Targets to create necessary directories if they do not exist.

# Compiling Source Files
# The rule $(BUILDDIR)/%.o: $(SRCDIR)/%.c compiles each .c file into an .o object file and manages dependencies:
# @mkdir -p $(dir $@): Creates the necessary directories for object files.
# $(CC) $(CFLAGS) $(INC) -c -o $@ $<: Compiles the .c file into an .o file.
# Dependency management commands create and handle dependency files (.d and .td files) to ensure that changes in header files trigger recompilation of dependent source files.

# Including Dependencies:
# - Ensures recompilation of source files when dependencies change.

# Special Targets:
# - .PHONY: Declares certain targets as not file-associated.

# Usage:
# - To build the project: make all
# - To clean built files: make clean
# - To rebuild everything: make re
# - For a debug build: make debug
# - For a release build: make release

PROJECT_NAME := FT_IRC
BIN_NAME  := irc
CC        := c++
ROOTDIR   := .
SRCDIR    := src
HEADERDIR := inc
BUILDDIR  := build
BINDIR    := bin
TARGET    := $(BINDIR)/$(BIN_NAME)
SOURCES   := $(shell find $(SRCDIR) -type f -name '*.cpp' | grep -v tests)
HEADERS   := $(shell find $(HEADERDIR) -type f -name '*.hpp' | grep -v tests)
OBJECTS   := $(patsubst $(SRCDIR)/%,$(BUILDDIR)/%,$(addsuffix .o,$(basename $(SOURCES))))
DEPS      := $(patsubst $(SRCDIR)/%,$(BUILDDIR)%,$(addsuffix .d,$(basename $(SOURCES))))
LDFLAGS   := -lpthread
CFLAGS    := -g3 -Wall -Wextra -Werror -std=c++98

INC       := -Iinc -Isrc

LIB_DIRS  := $(shell find $(LIBSRCDIR) -type d -exec test -e '{}/Makefile' ';' -print)

RESET=`tput sgr0`
GREEN=`tput setaf 2`
RED=`tput setaf 1`
YELLOW=`tput setaf 3`
BLUE=`tput setaf 4`
MAGENTA=`tput setaf 5`

define print_yellow
	echo "$(YELLOW)$(1)$(RESET)"
endef

define print_green
	echo "$(GREEN)$(1)$(RESET)"
endef

define print_red
	echo "$(RED)$(1)$(RESET)"
endef

debug: CFLAGS += -g
release: CFLAGS += -O3

all:	$(BINDIR) $(BUILDDIR) $(OBJECTS)
	@$(call print_green,"Linking object files")
	@$(CC) -o $(TARGET) $(OBJECTS) $(LDFLAGS)
	@printf "$(RED)$(TARGET)$(RESET)$(GREEN) has been created!\n$(RESET)";

clean:
	@printf "$(BLUE)Deleting the $(RESET)$(BUILDDIR) directory in $(RED)$(PROJECT_NAME)\n$(RESET)";
	@rm -rf $(BUILDDIR)

fclean: clean
	@printf "$(BLUE)Deleting the $(RESET)$(BINDIR) directory in $(RED)$(PROJECT_NAME)\n$(RESET)";
	@rm -rf $(BINDIR)

re: fclean all
release: 
	@printf "$(MAGENTA)Release mode enabled\n$(RESET)";
	@$(MAKE) all

$(LIBDIR) :
	@mkdir -p $(LIBDIR)

$(BUILDDIR) :
	@mkdir -p $(BUILDDIR)

$(BINDIR):
	@mkdir -p $(BINDIR)
	
$(BUILDDIR)/%.o: $(SRCDIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(INC) -c -o $@ $<
	@$(CC) $(CFLAGS) $(INC) -M $< -MT $@ > $(@:.o=.td)
	@cp $(@:.o=.td) $(@:.o=.d); 
	@sed -e 's/#.*//' -e 's/^[^:]*: *//' -e 's/ *\\$$//' \
	-e '/^$$/ d' -e 's/$$/ :/' < $(@:.o=.td) >> $(@:.o=.d); 
	@rm -f $(@:.o=.td)

-include $(DEPS)

.PHONY: clean fclean all remove-empty-bin-dirs

