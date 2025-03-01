CFLAGS := -DGLEW_NO_GLU -DGLEW_STATIC

all:
	$(CC) $(CFLAGS) -isystem include -c src/glew.c -o $(DIR_OBJ)/glew.o
	@echo $(CFLAGS)

lib:
	@echo -lopengl32 -lgdi32 -luser32 -lkernel32

clean:
	@true