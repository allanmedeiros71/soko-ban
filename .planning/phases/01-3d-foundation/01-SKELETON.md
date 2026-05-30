# Walking Skeleton — Soko-ban 3D

**Phase:** 1
**Generated:** 2026-05-29

## Capability Proven End-to-End

O jogador abre o jogo, vê o tabuleiro Sokoban em perspectiva isométrica 3D com tijolos, caixotes e personagem blocky, move-se nas 4 direções, empurra caixas, desfaz movimentos e completa fases — com HUD retro CGA na barra inferior.

## Architectural Decisions

| Decision | Choice | Rationale |
|----------|--------|-----------|
| Language | C99/C11 | Brownfield — zero migração de linguagem |
| Window/Input | SDL2 | Base existente; event loop preservado |
| 3D API | OpenGL 3.3 core + GLAD2 | macOS/Linux compatível; shaders GLSL 330 |
| Game logic | `game_logic.c/.h` sem SDL | Testável, portável; renderer read-only |
| Render | `renderer_3d.c/.h` isométrico | Painter's Algorithm; geometria procedural |
| Text/HUD | SDL2_ttf → overlay GL | Contrato UI-SPEC D-13; evita SDL_Renderer+GL mix |
| Build | Makefile estendido | Alinhado ao projeto; `make` + `make run` |
| Directory layout | `src/` modular | game_logic → gl_context → renderer_3d → main |

## Stack Touched in Phase 1

- [x] Project scaffold — Makefile modular, `src/` layout
- [x] Game logic — Board, move_player, undo extraídos sem SDL
- [x] OpenGL context — shaders, VAO, viewport HiDPI
- [x] 3D renderer — isométrico, todos os tile types
- [x] UI — HUD híbrido + overlay SC_WIN + SC_TITLE simples
- [x] Local run — `make && ./sokoban` macOS + Linux

## Out of Scope (Deferred to Later Slices)

- Scene manager com elevator menu (Phase 2)
- Perfis locais e persistência (Phase 3)
- Intro cinematográfica (Phase 4)
- Animação de caminhada do personagem (GAME-07, v2)
- Editor de fases (EDIT-01, v2)
- Câmera rotacionável

## Subsequent Slice Plan

- Phase 2: Elevator menu (Play/Edit/Exit) com scene_manager
- Phase 3: Perfis locais + seleção de níveis como andares
- Phase 4: Intro cinematográfica + polish visual global
