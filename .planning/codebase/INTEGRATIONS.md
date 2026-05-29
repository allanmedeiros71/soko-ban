# Integrations

**Analysis Date:** 2026-05-29

## External Libraries

**SDL2 (Simple DirectMedia Layer 2):**
- Purpose: Window creation, hardware-accelerated 2D rendering, keyboard event polling, and game timer
- Used via: `#include <SDL2/SDL.h>`
- Key APIs used:
  - `SDL_Init` / `SDL_Quit` — subsystem lifecycle
  - `SDL_CreateWindow` / `SDL_CreateRenderer` — display setup with `SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC`
  - `SDL_RenderFillRect`, `SDL_RenderDrawLine`, `SDL_RenderDrawLines`, `SDL_RenderCopy` — pixel-art tile drawing
  - `SDL_PollEvent` — keyboard input loop
  - `SDL_GetTicks` — elapsed time for HUD timer
- Install: `libsdl2-dev` (Debian/Ubuntu), `SDL2-devel` (Fedora), `sdl2` (Arch)

**SDL2_ttf:**
- Purpose: TrueType font rendering for all on-screen text (HUD stats, title screen, win screen)
- Used via: `#include <SDL2/SDL_ttf.h>`
- Key APIs used:
  - `TTF_Init` / `TTF_Quit` — library lifecycle
  - `TTF_OpenFont` — loads font at sizes 18px (body) and 40px (titles)
  - `TTF_RenderText_Solid` — renders text to an SDL_Surface
  - `TTF_SizeText` — measures text width for centered layout
- Font files: resolved at runtime from system font paths (no embedded font asset)
- Install: `libsdl2-ttf-dev` (Debian/Ubuntu), `SDL2_ttf-devel` (Fedora), `sdl2_ttf` (Arch)

## System APIs

**C Standard Library:**
- `<stdio.h>` — `fprintf` for error output to stderr, `snprintf` for HUD string formatting
- `<stdlib.h>` — standard utilities
- `<string.h>` — `memset`, `memcpy` for board state management and undo stack
- `<stdbool.h>` — boolean type used throughout game logic
- `<math.h>` (via `-lm`) — `abs()` used in diamond/goal tile rendering

**Filesystem (indirect):**
- Font files are loaded from hardcoded absolute paths on the local filesystem
- No other file I/O — level data is embedded as string literals in `sokoban.c`

## No Integrations

- No network calls, HTTP clients, or remote APIs
- No database or persistent storage — game state is in-memory only, no save/load
- No audio subsystem (SDL audio not initialized)
- No cloud services, telemetry, or analytics
- No external game engines or asset pipelines

---

*Integration audit: 2026-05-29*
