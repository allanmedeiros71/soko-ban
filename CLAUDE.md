<!-- GSD:project-start source:PROJECT.md -->
## Project

**Soko-ban — Primeira versão (releitura 3D)**

Releitura nostálgica do Sokoban original (Spectrum HoloByte, 1984), partindo de
uma implementação C/SDL2 funcional e evoluindo para uma experiência **inspirada** no
original: menus em formato de elevador, animação de abertura, cadastro local de
usuários, seleção de níveis como andares, e visual **3D** para paredes (tijolos),
caixotes e personagem. Referência visual: [vídeo YouTube](https://www.youtube.com/watch?v=Rjw_q9WH1Ao).

**Core Value:** O jogador sente que está jogando o Sokoban clássico — empurrar caixas até os alvos
com a estética retro e fluxo de menus do original — agora com presença 3D e
navegação por elevador.

### Constraints

- **Tech**: Manter C como linguagem principal; evoluir stack para 3D via SDL2 + OpenGL (decisão recomendada para brownfield)
- **Compatibilidade**: macOS desktop como target primário
- **Arquitetura**: Preservar lógica de tabuleiro/movement/undo existente; substituir camada de render
- **Perfis**: Armazenamento local (JSON ou similar), sem dependência de rede
- **Escopo**: Granularidade coarse (3–5 fases), modo MVP vertical
<!-- GSD:project-end -->

<!-- GSD:stack-start source:codebase/STACK.md -->
## Technology Stack

## Languages
- C (C99/C11) — entire game implementation in `sokoban.c`
## Build System
- Config: `Makefile`
- Compiler: `gcc`
- Compiler flags: `-O2 -Wall` plus SDL2 flags via `sdl2-config --cflags`
- Linker flags: `sdl2-config --libs` plus `-lSDL2_ttf -lm`
- `make` — compiles `sokoban.c` into the `sokoban` binary
- `make run` — compiles and immediately runs the binary
- `make clean` — removes the compiled binary
## Runtime Dependencies
- **SDL2** — windowing, rendering (2D rect/line drawing), event handling, timer (`SDL_GetTicks`)
- **SDL2_ttf** — TrueType font rendering for HUD and UI text
- **libm** (`-lm`) — C math library (used for `abs()` in pixel rendering)
- **System TrueType fonts** — loaded at runtime from hardcoded paths:
## Dev Dependencies
- **build-essential** (gcc, make) — compilation toolchain
- **libsdl2-dev** — SDL2 development headers
- **libsdl2-ttf-dev** — SDL2_ttf development headers
- No test framework — no automated tests present
## Platform / OS
- Font paths are Linux-specific (`/usr/share/fonts/...`)
- Package install documented for Debian/Ubuntu/Mint, Fedora, Arch/Manjaro
- No Windows or macOS support indicated
- Single-file program: all logic in `sokoban.c` (~400 lines)
<!-- GSD:stack-end -->

<!-- GSD:conventions-start source:CONVENTIONS.md -->
## Conventions

## Coding Style
- **Language:** C (C99/C11 with `stdbool.h`)
- **Indentation:** 4 spaces (no tabs)
- **Braces:** K&R style — opening brace on the same line as the statement
- **Line length:** No enforced limit; most lines stay under 100 characters
- **Compiler flags:** `-O2 -Wall` (warnings treated seriously, optimizations on)
- **Single source file:** All code lives in `sokoban.c`; no headers or separate translation units
## Naming Conventions
- `#define` names are ALL_CAPS with underscores: `WALL`, `FLOOR`, `MAXW`, `UNDO_MAX`, `NLEVELS`
- Tile character values use short memorable names: `BOX`, `BOXG`, `PLAYER`, `PLYRG`
- `typedef struct { … } TypeName;` — PascalCase for struct typedefs: `Color`, `Board`, `Snapshot`
- `typedef enum { … } TypeName;` — PascalCase enum typedef: `Scene`
- Enum values are SCREAMING_SNAKE_CASE with a common prefix: `SC_TITLE`, `SC_PLAY`, `SC_WIN`
- Local variables: lowercase, short, often single-letter for coordinates (`x`, `y`, `dx`, `dy`, `nx`, `ny`)
- Struct fields: lowercase with underscores (`start_ms`, `elapsed_paused`, `undo_top`)
- Global state: short uppercase names (`B` for the main board, `level_index`, `undo_top`)
- SDL objects: short lowercase with type hint (`r` for renderer, `win` for window, `ren` for renderer)
- `static const Color C_NAME` pattern — prefix `C_` for all palette entries: `C_BLACK`, `C_CYAN`, `C_GOLD`, `C_DKMAG`
- `snake_case` names: `load_level`, `move_player`, `push_undo`, `do_undo`, `is_won`, `is_goal_cell`
- Drawing helpers prefixed with `draw_`: `draw_wall`, `draw_floor`, `draw_box`, `draw_player`, `draw_text`, `draw_text_c`
- All functions declared `static` (file-local linkage); only `main` is non-static
## Code Patterns
- Two globals manage game state: `B` (type `Board`, `sokoban.c:116`) and `level_index` (`sokoban.c:115`)
- Undo stack is a global fixed array: `undo_stack[UNDO_MAX]` with `undo_top` counter (`sokoban.c:112–113`)
- No heap allocation for game data; all state lives in stack or static arrays
- SDL/TTF resources are allocated and freed explicitly in `main`: `SDL_CreateRenderer`, `SDL_DestroyRenderer`, `TTF_OpenFont`, `TTF_CloseFont`, etc.
- Temporary SDL surfaces and textures inside `draw_text` are created and destroyed each call (`sokoban.c:254–259`) — no caching
- SDL/TTF init failures print to `stderr` and return 1 immediately (`sokoban.c:270–271`)
- Font loading tries a list of candidate paths; exits with error if none found (`sokoban.c:292`)
- Null guard in `draw_text`: `if (!s) return;` (`sokoban.c:255`)
- Bounds are checked before array access in `move_player` and `load_level`
- The game board is a `char grid[MAXH][MAXW]` inside `Board`; cells contain the tile character literals directly (`#`, ` `, `.`, `$`, etc.)
- Level data is stored as plain multi-line C string literals in the `levels[]` array (`sokoban.c:51–98`)
- Top-level game state is a `Scene` enum (`SC_TITLE`, `SC_PLAY`, `SC_WIN`) switched in the event loop and render section of `main`
- `set_color` / `fill` are thin wrappers over SDL draw calls to reduce verbosity (`sokoban.c:119–126`)
- `is_goal_cell(char c)` centralizes the "is this tile a goal?" logic (`sokoban.c:168`)
## Comments / Documentation
- File-level block comment at the top documents purpose, compile command, and controls (`sokoban.c:1–15`)
- Section dividers use the pattern `/* ---------- Section Name ---------- */` to group related code
- Inline comments are in **Portuguese** (matching the project's language), placed at the end of lines or above short blocks
- No formal API documentation (Doxygen or similar); the single-file nature makes inline comments sufficient
- `README.md` serves as the user-facing documentation: dependencies, build steps, controls, and level format
<!-- GSD:conventions-end -->

<!-- GSD:architecture-start source:ARCHITECTURE.md -->
## Architecture

## Overview
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
```
## Game Loop
## Key Data Structures
```c
```
| Char | Meaning |
|------|---------|
| `#`  | Wall |
| ` `  | Empty floor |
| `.`  | Goal cell |
| `$`  | Box |
| `*`  | Box on goal |
| `@`  | Player |
| `+`  | Player on goal |
<!-- GSD:architecture-end -->

<!-- GSD:skills-start source:skills/ -->
## Project Skills

No project skills found. Add skills to any of: `.claude/skills/`, `.agents/skills/`, `.cursor/skills/`, `.github/skills/`, or `.codex/skills/` with a `SKILL.md` index file.
<!-- GSD:skills-end -->

<!-- GSD:workflow-start source:GSD defaults -->
## GSD Workflow Enforcement

Before using Edit, Write, or other file-changing tools, start work through a GSD command so planning artifacts and execution context stay in sync.

Use these entry points:
- `/gsd-quick` for small fixes, doc updates, and ad-hoc tasks
- `/gsd-debug` for investigation and bug fixing
- `/gsd-execute-phase` for planned phase work

Do not make direct repo edits outside a GSD workflow unless the user explicitly asks to bypass it.
<!-- GSD:workflow-end -->



<!-- GSD:profile-start -->
## Developer Profile

> Profile not yet configured. Run `/gsd-profile-user` to generate your developer profile.
> This section is managed by `generate-claude-profile` -- do not edit manually.
<!-- GSD:profile-end -->
