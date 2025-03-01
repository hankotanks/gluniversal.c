CFLAGS := -fgnu89-inline 

all:
	$(CC) $(CFLAGS) -isystem include -c src/RGFW.c -o $(DIR_OBJ)/RGFW.o
	@echo $(CFLAGS)

lib:
	@echo -lopengl32 -lgdi32

clean:
	@true