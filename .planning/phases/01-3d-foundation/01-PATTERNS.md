# Phase 1 Pattern Map

**Generated:** 2026-05-29
**Phase:** 01-3d-foundation

---

## Files to Create — Analog Mapping

| New File | Role | Closest Analog | Key Patterns to Reuse |
|----------|------|----------------|------------------------|
| `src/game_logic.c` | Board, move, undo, levels | `sokoban.c:99-196` | `Board` struct, `move_player`, `push_undo`/`do_undo`, `levels[]` |
| `src/game_logic.h` | Public API sem SDL | (new header) | Export `B`, `level_index`, tile `#define`s |
| `src/gl_context.c` | GL init, shaders, VAO | (new) | Error pattern: stderr + exit like `sokoban.c:327-328` |
| `src/renderer_3d.c` | Isometric tile drawing | `sokoban.c:199-284` | Replace `draw_wall`/`draw_box`/`draw_player`; keep color `C_*` values |
| `src/hud.c` | HUD + text overlay | `sokoban.c:286-298, 392-409` | `draw_text`, `draw_text_c`, HUD bar layout |
| `src/main.c` | Event loop, scenes | `sokoban.c:326-421` | `Scene` enum, `handle_key`, main loop structure |
| `assets/shaders/tile.vert` | MVP transform | (new) | GLSL 330 core |
| `assets/shaders/tile.frag` | Face color + shading | (new) | Pass RGB uniform per face (D-09) |

---

## Code Excerpts — Preserve Exactly

### Palette (game_logic.h or colors.h)

```c
static const Color C_BLACK   = {  12,  10,  24 };
static const Color C_CYAN    = {  40, 220, 220 };
static const Color C_MAGENTA = { 230,  60, 200 };
// ... rest from sokoban.c:26-34
static const Color C_BRICK   = { 180,  80,  50 };  // new per UI-SPEC
static const Color C_DKBRICK = { 120,  45,  30 };
```

### HUD format string

```c
snprintf(hud, sizeof hud, "FASE %d/%d   movimentos: %04d   empurroes: %04d   tempo: %u:%02u:%02u",
         level_index+1, NLEVELS, B.moves, B.pushes,
         frozen/3600, (frozen/60)%60, frozen%60);
```

### Scene enum

```c
typedef enum { SC_TITLE, SC_PLAY, SC_WIN } Scene;
```

---

## Integration Points

| From | To | Contract |
|------|-----|----------|
| `main.c` | `game_logic.h` | `move_player`, `load_level`, `is_won`, `do_undo` |
| `main.c` | `renderer_3d.h` | `r3d_draw_board(&B, ox, oy)` |
| `main.c` | `hud.c` | `hud_draw(Scene, win_w, win_h)` |
| `renderer_3d.c` | `game_logic.h` | Read-only `Board*` — never call move_player |
| `gl_context.c` | `renderer_3d.c` | `gl_use_tile_shader()`, unit cube VAO |

---

## Makefile Pattern

Extend existing `Makefile` — preserve macOS/Linux branching:

```makefile
SRCS = src/main.c src/game_logic.c src/gl_context.c src/renderer_3d.c src/hud.c
# Add -framework OpenGL on Darwin, -lGL on Linux
```
