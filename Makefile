# To add a library:
#   Make a new folder for the library /lib/{NAME}
#   Add /lib/{NAME}/include for any headers that need to be accessible
#   Add a Makefile
#    - all: must @echo requisite CFLAGS
#    - lib: must @echo any libraries
#   If object files are produced: output to $(DIR_OBJ)
#   If static libraries are produced:
#      output to the base directory $(DIR_LIB_STUB)

TARGET := main
export CC := gcc
export AR := ar
CFLAGS :=
LDLIBS :=

ROOT := $(patsubst %/,%, $(patsubst %\,%, $(dir $(abspath $(lastword $(MAKEFILE_LIST))))))

DIR_INC := include
DIR_SRC := src
DIR_BIN := bin
DIR_OBJ_STUB := build
export DIR_OBJ := $(ROOT)/$(DIR_OBJ_STUB)
DIR_LIB_STUB := lib
DIR_LIB_LIST := $(dir $(wildcard $(DIR_LIB_STUB)/*/Makefile))
export DIR_LIB := $(ROOT)/$(DIR_LIB_STUB)

ifeq ($(OS),Windows_NT)
    $(shell echo on)
    export PATH:=$(ROOT)/$(DIR_BIN);$(PATH)
    RM := rm.exe -f
    TAIL := tail.exe
    HEAD := head.exe
else
    TAIL := tail
    HEAD := head
endif

uniq = $(if $1,$(firstword $1) $(call uniq,$(filter-out $(firstword $1),$1)))

ifeq ($(filter clean,$(MAKECMDGOALS)),)
    $(info Checking status of libraries.)
    CFLAGS += -I$(DIR_INC)
    CFLAGS += $(addprefix -isystem , $(wildcard $(DIR_LIB_STUB)/*/include))
    ENV := CC=$(CC) AR=$(AR) DIR_OBJ=$(DIR_OBJ) DIR_LIB=$(DIR_LIB)
    CFLAGS += $(foreach dir, $(DIR_LIB_LIST), $(shell $(MAKE) $(ENV) -C $(dir) | $(TAIL) -n 2 | $(HEAD) -n 1))
    CFLAGS := $(filter-out %ECHO is off.%, $(CFLAGS))
    LIBS := $(foreach dir, $(DIR_LIB_LIST), $(shell $(MAKE) $(ENV) lib -C $(dir) | $(TAIL) -n 2 | $(HEAD) -n 1))
    LIBS := $(filter-out %ECHO is off.%, $(LIBS))
    LDLIBS += $(call uniq, $(LIBS))
    LDFLAGS := -L$(DIR_LIB_STUB)
endif

all: $(TARGET) 

SRC = $(wildcard $(DIR_SRC)/*.c)
OBJ = $(patsubst $(DIR_SRC)/%.c, $(DIR_OBJ_STUB)/%.o, $(SRC))

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(wildcard $(DIR_OBJ_STUB)/*.o) $(LDLIBS)
	@echo Assembled executable ($@).

$(DIR_OBJ_STUB)/%.o: $(DIR_SRC)/%.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@ $(LDLIBS)
	@echo Compiled $< to $@.

clean:
	-$(foreach dir, $(DIR_LIB_LIST), $(MAKE) clean -C "$(ROOT)/$(dir)" && call)
	$(RM) -r $(wildcard $(DIR_OBJ)/*.o)