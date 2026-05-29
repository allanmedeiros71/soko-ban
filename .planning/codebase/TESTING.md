# Testing

## Test Infrastructure

None. The project has no automated tests — no test framework, no test runner, no test targets in the Makefile. The only verification is manual (`make run` and playing the game).

## Test Coverage

None. All logic is untested:

- `move_player` (sokoban.c:170) — collision, box pushing, boundary clamping
- `is_won` (sokoban.c:146) — win condition
- `load_level` (sokoban.c:129) — level string parsing, player position detection
- `push_undo` / `do_undo` (sokoban.c:154–165) — undo stack save/restore
- `is_goal_cell` (sokoban.c:168) — tile classification

## How to Run Tests

No test command exists. Manual verification only:

```
make run
```

## Recommended Approach

Extract pure game logic (no SDL dependency) into a `game.c`/`game.h` pair, then write a minimal test runner using a single-header C framework:

- [greatest](https://github.com/silentbicycle/greatest) — lightweight, header-only
- [Unity](https://github.com/ThrowTheSwitch/Unity) — more full-featured

Add a `test` target to the Makefile that compiles and runs the test suite without SDL.
