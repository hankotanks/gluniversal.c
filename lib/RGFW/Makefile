CFLAGS := -fgnu89-inline 
ifeq ($(OS), Windows_NT)
    LDLIBS := -lopengl32 -lgdi32
else
    LDLIBS := -lX11 -lGL -lXrandr
endif

all:
	$(CC) $(CFLAGS) -isystem include -c src/RGFW.c -o $(DIR_OBJ)/RGFW.o
	@$(PUTS) $(CFLAGS)

lib:
	@$(PUTS) $(LDLIBS)

clean: