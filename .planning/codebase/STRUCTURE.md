# Project Structure

## Directory Layout

```
sokoban/
├── sokoban.c          # Entire game — ~391 lines, single translation unit
├── Makefile           # Build, run, clean targets
├── README.md          # Project description
├── preview.webp       # Screenshot/preview image
├── preview (1).webp   # Additional preview
├── preview (2).webp   # Additional preview
└── sokoban            # Compiled binary (gitignored? currently committed)
```

## Module Breakdown

Everything is in `sokoban.c`. Logical sections in order:

| Section | Lines | Description |
|---------|-------|-------------|
| Includes & palette | 1–34 | SDL2 headers, Color struct, CGA color constants |
| Tile defines | 37–48 | ASCII char macros (WALL, FLOOR, GOAL, BOX, etc.) and grid/display constants |
| Level data | 51–99 | 4 hardcoded level strings; `NLEVELS` macro |
| Game state | 102–116 | `Board` struct, `Snapshot` undo struct, global state vars |
| Draw helpers | 119–126 | `set_color`, `fill` — SDL color/rect wrappers |
| Level loading | 129–144 | `load_level(int idx)` — parses string → grid |
| Win detection | 146–151 | `is_won()` |
| Undo | 154–165 | `push_undo()`, `do_undo()` |
| Movement | 168–196 | `move_player(int dx, int dy)` |
| Tile drawing | 199–259 | `draw_wall`, `draw_floor`, `draw_box`, `draw_player`, `draw_text`, `draw_text_c` |
| main() | 266–391 | SDL init, window/renderer/font setup, game loop, cleanup |

## Entry Point

`main()` at line 269. Initializes SDL2 and SDL2_ttf, opens the window (896×712px fixed), loads the first font found from a hardcoded Linux path list, loads level 0, then enters the event/render loop.

## Adding New Code

- **New level**: append a string to `levels[]` (line 51) — `NLEVELS` updates automatically.
- **New tile type**: add a `#define` near line 37, handle in `move_player` and the render switch.
- **New scene**: add a value to `Scene` enum (line 267), add branches in the event handler and render section.
- **Extracted logic**: the file has no headers — splitting would require creating `game.h`/`game.c` and updating the Makefile.
