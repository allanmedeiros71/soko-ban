# Phase 1 Research: 3D Foundation

**Researched:** 2026-05-29
**Phase:** 01-3d-foundation
**Confidence:** HIGH (brownfield codebase + project research artifacts)

---

## Summary

Phase 1 migra a renderização 2D SDL para OpenGL 3.3 isométrico preservando 100% da lógica de gameplay. A estratégia segue STATE.md: **extrair `game_logic` primeiro**, depois OpenGL — nunca ambos no mesmo commit. O jogo deve permanecer jogável após cada plano.

---

## Standard Stack

| Layer | Choice | Rationale |
|-------|--------|-----------|
| Language | C99/C11 | Brownfield — projeto inteiro em C |
| Window/Input | SDL2 | Já em uso; `SDL_GL_CreateContext` para OpenGL |
| 3D API | OpenGL 3.3 core | macOS cap 4.1 compatível com `#version 330 core` |
| Math | cglm (header-only) ou manual | Projeção isométrica ortográfica; cglm opcional se matrizes manuais bastarem |
| Loader | GLAD2 (single-file) | Evita dependência de headers GL deprecated no macOS |
| Text overlay | SDL2_ttf → texture GL | HUD híbrido (D-13); evitar SDL_Renderer + GL no mesmo frame |
| Shaders | GLSL 330 core | Mesma versão macOS + Linux |
| Build | Makefile | Estender Makefile existente com novos `.c` |

**Não usar:** SDL_Renderer + OpenGL direto no mesmo frame (PITFALLS anti-pattern 1).

---

## Architecture Patterns

### Ordem de implementação (mandatory)

1. Extrair `src/game_logic.c/.h` — zero `#include <SDL.h>`
2. `src/gl_context.c/.h` — contexto, shaders, VAO cubo unitário
3. `src/renderer_3d.c/.h` — câmera isométrica, `draw_*_3d`, Painter's Algorithm
4. `src/main.c` — loop eventos → game_logic → renderer_3d → HUD overlay
5. Manter `sokoban.c` como referência até build modular estável

### Câmera isométrica

- Projeção **ortográfica** (não perspectiva)
- Rotação Y ≈ 45°, inclinação X ≈ 30° (ajustável na implementação)
- View matrix fixa por frame em `r3d_begin_frame()`

### Depth sort (Painter's Algorithm)

- Ordenar tiles por `(col + row)` antes de desenhar
- Correto para grid isométrico 2.5D; evita z-fighting sem depth buffer complexo
- Decisão STATE.md — não substituir por depth buffer na v1

### Renderer lê grid, não modifica lógica

- `r3d_draw_board(Board *b, ...)` — read-only sobre `B.grid`
- Animações de movimento = estado **do renderer** (posições interpoladas), não mutação antecipada do grid
- Undo instantâneo: grid salta imediatamente; renderer sincroniza sem animação reversa (D-08)

### HUD híbrido (D-13)

- Viewport GL para tabuleiro (área acima do HUD 40px)
- SDL2_ttf renderiza texto para textura GL ou overlay após `r3d_end_frame`
- Barra inferior: fundo `C_DKMAG`, borda 2px `C_MAGENTA`, texto `C_CYAN` (UI-SPEC)

---

## Tile Visual Specs (from UI-SPEC + CONTEXT)

| Tile | 3D Treatment |
|------|-------------|
| Wall | Tijolos individuais 3D, C_BRICK/C_DKBRICK (D-01, D-10) |
| Floor | Plano escuro |
| Goal | Marcador 2D ciano no chão (D-04, D-11) |
| Box | Engradado madeira/dourado (D-02) |
| Box on goal | Verde/dourado C_GREEN (D-12) |
| Player | Blocky: cabeça branca, corpo magenta, pernas ciano (D-03) |

Shading: faces claras/escuras sem sombras projetadas (D-09).

---

## Animation Specs (D-05–D-08)

| Behavior | Spec |
|----------|------|
| Player move | Lerp 100–150 ms entre células |
| Box push | Player + box movem juntos |
| Input during anim | Bloqueado |
| Undo | Instantâneo — sem animação reversa |

---

## Platform Requirements

| Platform | Requirement |
|----------|-------------|
| macOS | `#version 330 core`, `SDL_GL_GetDrawableSize` para HiDPI/Retina (PLAT-02) |
| Linux | Link `-lGL`, mesma versão shader (PLAT-03) |
| Both | `make` compila e executa (PLAT-01) |

**SDL2 base path:** Considerar `SDL_GetBasePath()` + `chdir()` em main para assets/fonts (PITFALL 6) — aplicar em Phase 1 pois fontes embarcadas são críticas.

---

## File Structure (Phase 1 scope)

```
soko-ban/
├── Makefile                 ← atualizado para src/*.c
├── sokoban.c                ← referência (não deletar ainda)
├── assets/
│   ├── fonts/RobotoMono-Bold.ttf
│   └── shaders/
│       ├── tile.vert
│       └── tile.frag
└── src/
    ├── main.c
    ├── game_logic.c / .h
    ├── gl_context.c / .h
    ├── renderer_3d.c / .h
    └── hud.c / .h           ← overlay SDL2_ttf (opcional módulo)
```

Phase 1 **não** inclui: scene_manager completo, profile.c, scenes/menu.c (Phase 2+).

---

## Risks and Mitigations

| Risk | Mitigation |
|------|------------|
| Big-bang refactor | Um commit por plano; jogo jogável após cada wave |
| macOS GL deprecated | GLAD + 330 core; testar macOS no primeiro commit GL |
| SDL_Renderer + GL conflict | OpenGL para tabuleiro; TTF via textura GL |
| Scope creep (walk anim) | GAME-07 explicitamente v2 — não implementar |
| z-fighting | Painter's Algorithm, não depth buffer |

---

## Out of Scope (Phase 1)

- Menus elevador completos (Phase 2)
- Perfis locais (Phase 3)
- Intro cinematográfica (Phase 4)
- Animação de caminhada do personagem (GAME-07, v2)
- Câmera rotacionável

---

## Sources

- `.planning/research/ARCHITECTURE.md` — component boundaries, build order
- `.planning/research/PITFALLS.md` — anti-patterns, platform traps
- `.planning/phases/01-3d-foundation/01-CONTEXT.md` — D-01–D-16 locked decisions
- `.planning/phases/01-3d-foundation/01-UI-SPEC.md` — visual contract
- `sokoban.c` — current implementation reference
