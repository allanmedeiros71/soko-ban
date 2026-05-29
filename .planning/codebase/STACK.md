# Tech Stack

**Analysis Date:** 2026-05-29

## Languages

**Primary:**
- C (C99/C11) — entire game implementation in `sokoban.c`

## Build System

**Tool:** GNU Make
- Config: `Makefile`
- Compiler: `gcc`
- Compiler flags: `-O2 -Wall` plus SDL2 flags via `sdl2-config --cflags`
- Linker flags: `sdl2-config --libs` plus `-lSDL2_ttf -lm`

**Makefile targets:**
- `make` — compiles `sokoban.c` into the `sokoban` binary
- `make run` — compiles and immediately runs the binary
- `make clean` — removes the compiled binary

**Direct build command (without Make):**
```bash
gcc sokoban.c -o sokoban $(sdl2-config --cflags --libs) -lSDL2_ttf -lm
```

## Runtime Dependencies

- **SDL2** — windowing, rendering (2D rect/line drawing), event handling, timer (`SDL_GetTicks`)
- **SDL2_ttf** — TrueType font rendering for HUD and UI text
- **libm** (`-lm`) — C math library (used for `abs()` in pixel rendering)
- **System TrueType fonts** — loaded at runtime from hardcoded paths:
  - `/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf`
  - `/usr/share/fonts/truetype/liberation/LiberationMono-Bold.ttf`
  - `/usr/share/fonts/TTF/DejaVuSansMono-Bold.ttf`

## Dev Dependencies

- **build-essential** (gcc, make) — compilation toolchain
- **libsdl2-dev** — SDL2 development headers
- **libsdl2-ttf-dev** — SDL2_ttf development headers
- No test framework — no automated tests present

## Platform / OS

**Target:** Linux (x86_64)
- Font paths are Linux-specific (`/usr/share/fonts/...`)
- Package install documented for Debian/Ubuntu/Mint, Fedora, Arch/Manjaro
- No Windows or macOS support indicated
- Single-file program: all logic in `sokoban.c` (~400 lines)

---

*Stack analysis: 2026-05-29*
