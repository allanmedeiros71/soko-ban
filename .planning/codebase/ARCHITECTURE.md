# Architecture

## Overview

Single-file C game built on SDL2 + SDL2_ttf. All logic, rendering, and state live in `sokoban.c` (391 lines). The game follows a classic event-poll → update → render loop with three scenes: title screen, gameplay, and win screen. Board state is a flat character grid that encodes all entity types as ASCII characters, matching the original Sokoban file format convention.

## Components

| Component | Lines | Responsibility |
|-----------|-------|----------------|
| Color palette | 26–34 | CGA-style color constants as `Color` structs |
| Tile constants | 37–48 | ASCII char defines for WALL, FLOOR, GOAL, BOX, PLAYER variants |
| Level data | 51–98 | 4 hardcoded level strings in classic Sokoban `.sok` format |
| Board state | 102–113 | `Board` struct + global undo stack |
| Draw helpers | 119–126 | `set_color`, `fill` — thin SDL wrappers |
| Level loader | 129–144 | `load_level()` — parses level string into grid |
| Win check | 146–151 | `is_won()` — scans grid for un-placed boxes |
| Undo system | 154–165 | `push_undo` / `do_undo` — snapshot stack |
| Movement | 168–196 | `move_player()` — collision, box push, tile rewriting |
| Tile renderers | 199–259 | `draw_wall`, `draw_floor`, `draw_box`, `draw_player`, `draw_text` |
| Main loop | 266–391 | SDL init, event handling, scene dispatch, render, cleanup |

## Data Flow

```
SDL_PollEvent → keydown handler → move_player() / load_level() / do_undo()
                                         ↓
                                  Board.grid mutated in place
                                         ↓
                              is_won() → scene transition
                                         ↓
                              render loop reads Board.grid → draw_* calls
```

## Game Loop

Standard SDL event-poll loop at ~60 FPS (`SDL_Delay(16)`):

1. Poll all pending `SDL_Event`s; dispatch by scene and key
2. Clear renderer with `C_BLACK`
3. Render active scene (title / board + HUD / win overlay)
4. `SDL_RenderPresent`

No fixed-timestep; timing is purely event-driven (no animation, no held-key repeat).

## Key Data Structures

```c
typedef struct { Uint8 r, g, b; } Color;   // RGB color triple

typedef struct {
    char grid[MAXH][MAXW];     // 24×32 char grid; encodes all tiles
    int  w, h;                 // actual level dimensions
    int  px, py;               // player position
    int  moves, pushes;        // stats counters
    Uint32 start_ms;           // SDL tick at level load (timer)
    Uint32 elapsed_paused;     // dead field — never written after init
} Board;

typedef struct {
    char grid[MAXH][MAXW];
    int  px, py, moves, pushes;
} Snapshot;                    // undo stack entry (full grid copy)

typedef enum { SC_TITLE, SC_PLAY, SC_WIN } Scene;
```

Tile encoding (ASCII chars in `Board.grid`):

| Char | Meaning |
|------|---------|
| `#`  | Wall |
| ` `  | Empty floor |
| `.`  | Goal cell |
| `$`  | Box |
| `*`  | Box on goal |
| `@`  | Player |
| `+`  | Player on goal |
