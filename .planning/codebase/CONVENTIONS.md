# Coding Conventions

**Analysis Date:** 2026-05-29

## Coding Style

- **Language:** C (C99/C11 with `stdbool.h`)
- **Indentation:** 4 spaces (no tabs)
- **Braces:** K&R style — opening brace on the same line as the statement
- **Line length:** No enforced limit; most lines stay under 100 characters
- **Compiler flags:** `-O2 -Wall` (warnings treated seriously, optimizations on)
- **Single source file:** All code lives in `sokoban.c`; no headers or separate translation units

## Naming Conventions

**Constants / macros:**
- `#define` names are ALL_CAPS with underscores: `WALL`, `FLOOR`, `MAXW`, `UNDO_MAX`, `NLEVELS`
- Tile character values use short memorable names: `BOX`, `BOXG`, `PLAYER`, `PLYRG`

**Types:**
- `typedef struct { … } TypeName;` — PascalCase for struct typedefs: `Color`, `Board`, `Snapshot`
- `typedef enum { … } TypeName;` — PascalCase enum typedef: `Scene`
- Enum values are SCREAMING_SNAKE_CASE with a common prefix: `SC_TITLE`, `SC_PLAY`, `SC_WIN`

**Variables:**
- Local variables: lowercase, short, often single-letter for coordinates (`x`, `y`, `dx`, `dy`, `nx`, `ny`)
- Struct fields: lowercase with underscores (`start_ms`, `elapsed_paused`, `undo_top`)
- Global state: short uppercase names (`B` for the main board, `level_index`, `undo_top`)
- SDL objects: short lowercase with type hint (`r` for renderer, `win` for window, `ren` for renderer)

**Color constants:**
- `static const Color C_NAME` pattern — prefix `C_` for all palette entries: `C_BLACK`, `C_CYAN`, `C_GOLD`, `C_DKMAG`

**Functions:**
- `snake_case` names: `load_level`, `move_player`, `push_undo`, `do_undo`, `is_won`, `is_goal_cell`
- Drawing helpers prefixed with `draw_`: `draw_wall`, `draw_floor`, `draw_box`, `draw_player`, `draw_text`, `draw_text_c`
- All functions declared `static` (file-local linkage); only `main` is non-static

## Code Patterns

**Global state:**
- Two globals manage game state: `B` (type `Board`, `sokoban.c:116`) and `level_index` (`sokoban.c:115`)
- Undo stack is a global fixed array: `undo_stack[UNDO_MAX]` with `undo_top` counter (`sokoban.c:112–113`)

**Memory management:**
- No heap allocation for game data; all state lives in stack or static arrays
- SDL/TTF resources are allocated and freed explicitly in `main`: `SDL_CreateRenderer`, `SDL_DestroyRenderer`, `TTF_OpenFont`, `TTF_CloseFont`, etc.
- Temporary SDL surfaces and textures inside `draw_text` are created and destroyed each call (`sokoban.c:254–259`) — no caching

**Error handling:**
- SDL/TTF init failures print to `stderr` and return 1 immediately (`sokoban.c:270–271`)
- Font loading tries a list of candidate paths; exits with error if none found (`sokoban.c:292`)
- Null guard in `draw_text`: `if (!s) return;` (`sokoban.c:255`)
- Bounds are checked before array access in `move_player` and `load_level`

**Grid representation:**
- The game board is a `char grid[MAXH][MAXW]` inside `Board`; cells contain the tile character literals directly (`#`, ` `, `.`, `$`, etc.)
- Level data is stored as plain multi-line C string literals in the `levels[]` array (`sokoban.c:51–98`)

**Scene/state machine:**
- Top-level game state is a `Scene` enum (`SC_TITLE`, `SC_PLAY`, `SC_WIN`) switched in the event loop and render section of `main`

**Helper abstractions:**
- `set_color` / `fill` are thin wrappers over SDL draw calls to reduce verbosity (`sokoban.c:119–126`)
- `is_goal_cell(char c)` centralizes the "is this tile a goal?" logic (`sokoban.c:168`)

## Comments / Documentation

- File-level block comment at the top documents purpose, compile command, and controls (`sokoban.c:1–15`)
- Section dividers use the pattern `/* ---------- Section Name ---------- */` to group related code
- Inline comments are in **Portuguese** (matching the project's language), placed at the end of lines or above short blocks
- No formal API documentation (Doxygen or similar); the single-file nature makes inline comments sufficient
- `README.md` serves as the user-facing documentation: dependencies, build steps, controls, and level format

---

*Convention analysis: 2026-05-29*
