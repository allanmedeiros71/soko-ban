TARGET  = sokoban
UNAME   = $(shell uname -s)

ifeq ($(UNAME),Darwin)
  BREW_PREFIX  := $(shell brew --prefix 2>/dev/null)
  SDL2_CONFIG  := $(BREW_PREFIX)/bin/sdl2-config
  CC           = clang
  CFLAGS       = -O2 -Wall $(shell $(SDL2_CONFIG) --cflags)
  LIBS         = $(shell $(SDL2_CONFIG) --libs) -L$(BREW_PREFIX)/lib -lSDL2_ttf
else
  CC           = gcc
  CFLAGS       = -O2 -Wall $(shell sdl2-config --cflags)
  LIBS         = $(shell sdl2-config --libs) -lSDL2_ttf -lm
endif

$(TARGET): sokoban.c assets/fonts/RobotoMono-Bold.ttf
ifeq ($(UNAME),Darwin)
	@if [ ! -x "$(SDL2_CONFIG)" ]; then \
		echo "Instale as dependencias: brew install sdl2 sdl2_ttf"; \
		exit 1; \
	fi
endif
	$(CC) $(CFLAGS) sokoban.c -o $(TARGET) $(LIBS)

run: $(TARGET)
	./$(TARGET)

clean:
	rm -f $(TARGET)

.PHONY: run clean
