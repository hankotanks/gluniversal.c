CFLAGS := -DGLEW_NO_GLU -DGLEW_STATIC
ifeq ($(OS), Windows_NT)
    LDLIBS := -lopengl32 -lgdi32 -luser32 -lkernel32
else
    LDLIBS := -lGL
endif


all:
	$(CC) $(CFLAGS) -isystem include -c src/glew.c -o $(DIR_OBJ)/glew.o
	@echo $(CFLAGS)

lib:
	@echo $(LDLIBS)

clean:
	@true