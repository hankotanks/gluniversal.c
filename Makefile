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
CC := gcc
export CC
AR := ar
export AR

ROOT := $(patsubst %/,%, $(patsubst %\,%, $(dir $(abspath $(lastword $(MAKEFILE_LIST))))))

DIR_LIB_STUB := lib
DIR_LIB := $(ROOT)/$(DIR_LIB_STUB)
export DIR_LIB
DIR_LIB_LIST := $(dir $(wildcard $(DIR_LIB_STUB)/*/Makefile))
DIR_INC := include
DIR_SRC := src
DIR_OBJ_STUB := build
DIR_OBJ := $(ROOT)/$(DIR_OBJ_STUB)
export DIR_OBJ

ifeq ($(wildcard $(DIR_OBJ_STUB)/),)
    $(shell mkdir -p $(DIR_OBJ_STUB))
endif

uniq = $(if $1,$(firstword $1) $(call uniq,$(filter-out $(firstword $1),$1)))

CFLAGS := -I$(DIR_INC)
CFLAGS += $(addprefix -isystem , $(wildcard $(DIR_LIB_STUB)/*/include))
CFLAGS += $(foreach dir, $(DIR_LIB_LIST), $(shell $(MAKE) -C $(dir) | tail -n 2 | head -n 1))
LIBS := $(foreach dir, $(DIR_LIB_LIST), $(shell $(MAKE) lib -C $(dir) | tail -n 2 | head -n 1))
LIBS_DEDUP := $(call uniq, $(LIBS))
LDLIBS :=
LDLIBS += $(LIBS_DEDUP)
LDFLAGS := -L$(DIR_LIB_STUB)

all: $(TARGET) 

SRC = $(wildcard $(DIR_SRC)/*.c)
OBJ = $(patsubst $(DIR_SRC)/%.c, $(DIR_OBJ_STUB)/%.o, $(SRC))

$(TARGET): $(OBJ)
	$(CC) $(CFLAGS) $(LDFLAGS) -o $@ $(wildcard $(DIR_OBJ_STUB)/*.o) $(LDLIBS)

$(DIR_OBJ_STUB)/%.o: $(DIR_SRC)/%.c
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@ $(LDLIBS)

clean:
	$(foreach dir, $(DIR_LIB_LIST), $(MAKE) clean -C $(dir);)
	rm -rf $(DIR_OBJ)