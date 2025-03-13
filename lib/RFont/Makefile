CFLAGS := -fgnu89-inline
LDLIBS := -lm
ifeq ($(OS), Windows_NT)
    LDLIBS += -lopengl32 -lgdi32
else
    LDLIBS += -lGL
endif

all:
	$(CC) $(CFLAGS) -isystem include -c src/RFont.c -o $(DIR_OBJ)/RFont.o
	@echo $(CFLAGS)

lib:
	@echo $(LDLIBS)

clean: