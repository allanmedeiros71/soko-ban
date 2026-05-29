CC      = gcc
CFLAGS  = -O2 -Wall $(shell sdl2-config --cflags)
LIBS    = $(shell sdl2-config --libs) -lSDL2_ttf -lm
TARGET  = sokoban

$(TARGET): sokoban.c
	$(CC) $(CFLAGS) sokoban.c -o $(TARGET) $(LIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: run clean
