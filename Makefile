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

ROOT := $(patsubst %/,%, $(patsubst %\,%, $(dir $(abspath $(lastword $(MAKEFILE_LIST))))))

DIR_INC := include
DIR_SRC := src
DIR_OBJ_STUB := build
export DIR_OBJ := $(ROOT)/$(DIR_OBJ_STUB)
DIR_LIB_STUB := lib
DIR_LIB_LIST := $(dir $(wildcard $(DIR_LIB_STUB)/*/Makefile))
export DIR_LIB := $(ROOT)/$(DIR_LIB_STUB)

ifeq ($(wildcard $(DIR_OBJ_STUB)/),)
    $(shell mkdir $(DIR_OBJ_STUB))
endif

uniq = $(if $1,$(firstword $1) $(call uniq,$(filter-out $(firstword $1),$1)))

CFLAGS := -I$(DIR_INC)
CFLAGS += $(addprefix -isystem , $(wildcard $(DIR_LIB_STUB)/*/include))
ENV := CC=$(CC) AR=$(AR) DIR_OBJ=$(DIR_OBJ) DIR_LIB=$(DIR_LIB)
CFLAGS += $(foreach dir, $(DIR_LIB_LIST), $(shell $(MAKE) $(ENV) -C $(dir) | tail -n 2 | head -n 1))
LIBS := $(foreach dir, $(DIR_LIB_LIST), $(shell $(MAKE) $(ENV) lib -C $(dir) | tail -n 2 | head -n 1))
LDLIBS :=
LDLIBS += $(call uniq, $(LIBS))
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
	$(RM) -r $(DIR_OBJ_STUB)