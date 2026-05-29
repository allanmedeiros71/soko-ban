# Concerns

## Technical Debt

- **Monolithic single file**: all 391 lines in `sokoban.c` — logic, rendering, data, and main() tightly coupled. Extracting or testing any piece requires splitting the file.
- **Hardcoded level data**: 4 levels as C string literals. No support for loading `.sok` files from disk; adding levels requires recompiling.
- **Global mutable state**: `B` (Board), `undo_stack`, `undo_top`, `level_index` are all file-scope globals. Makes unit testing and future multi-level state management awkward.
- **Dead field**: `Board.elapsed_paused` (line 107) is declared but never written after `memset` in `load_level`. The HUD uses a local `static` `frozen` instead (line 366).
- **Static local in render loop**: `static Uint32 frozen` inside `main()` (line 366) is a hidden state variable baked into the render path — a code smell.

## Known Issues

- **Win check in event handler**: `is_won()` is called after every keypress (line 318), inside the event poll loop, before rendering. If the game is very fast, a frame could be drawn in SC_WIN without the win overlay rendered on the same frame the win occurs.
- **Silent undo-stack overflow**: `push_undo` silently drops undo entries once `undo_top >= 4096` (line 155). After 4096 moves, undo silently stops working with no feedback to the player.
- **Off-by-one in board height**: `load_level` sets `B.h = (x > 0) ? y + 1 : y` (line 140). If the level string ends with `\n` (which all 4 do), `x` is 0 at loop end, so `B.h = y` (correct). But the condition inverts intuition — it works by accident.
- **Missing bounds check on grid writes**: `move_player` checks `nx`/`ny` and `bx`/`by` against `B.w`/`B.h` (lines 172, 178), but `B.grid` is allocated as `MAXH×MAXW` (24×32). If a level is wider than `MAXW` or taller than `MAXH`, the level parser writes out of bounds silently.
- **Unchecked SDL resource creation**: `SDL_CreateWindow` and `SDL_CreateRenderer` return values are not checked for NULL (lines 274–277). A failure here would crash with a null pointer dereference rather than a clean error message.
- **Font handle leak on early exit**: if `font` is loaded but `big` fails (line 289), `font` is leaked before returning (line 292).

## Risks

- **Linux-only hardcoded font paths** (lines 281–285): the font list only covers common Linux TTF locations. The game won't start on macOS, Windows, or minimal Linux installs that lack DejaVu/Liberation fonts, printing "Nenhuma fonte encontrada." and exiting.
- **No `-Wextra` or `-Werror` in Makefile**: the build uses `-Wall` but not stricter flags. Silent warnings may hide real bugs.
- **Compiled binary may be committed**: `sokoban` (the binary) appears in the working tree with no `.gitignore`. This bloats git history.
- **Fixed window size**: `win_w = MAXW * TILE = 896`, `win_h = MAXH * TILE + HUD = 712` (line 273). No resizing, HiDPI scaling, or fullscreen support.
- **No held-key auto-repeat**: movement only triggers on `SDL_KEYDOWN` events. SDL's key-repeat is OS-dependent; there is no explicit `SDL_SetTextInputRect` or repeat config, making fast movement feel sluggish on systems with slow repeat rates.

## Improvement Opportunities

- Extract `Board` logic (load, move, undo, win check) into `game.c`/`game.h` with no SDL dependency — enables unit testing.
- Load levels from `.sok` files at runtime instead of hardcoding strings.
- Add a proper pause screen (the `elapsed_paused` field suggests this was planned).
- Replace `SDL_Delay(16)` with a proper fixed-timestep loop using `SDL_GetTicks`.
- Add a `.gitignore` to exclude the compiled binary.
- Add held-key repeat via tracking `SDL_KEYDOWN`/`SDL_KEYUP` state and firing moves on a timer.
- Add audio (SDL_mixer) for move/push/win sounds — consistent with the CGA nostalgia theme.

## Missing Features

- **No pause**: `elapsed_paused` exists but is never used; there's no pause key or logic.
- **No post-level stats screen**: win screen shows no moves/pushes/time summary.
- **No level select**: only sequential N/P navigation; no menu to jump to a specific level.
- **Only 4 levels**: the original Sokoban had 50. Adding more requires code changes.
- **No high score / best time persistence**: stats are lost on exit.
- **No animated movement**: tiles snap instantly; no smooth sliding animation.
