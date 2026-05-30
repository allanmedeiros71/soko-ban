# Phase 1: 3D Foundation - Context

**Gathered:** 2026-05-29
**Status:** Ready for planning

<domain>
## Phase Boundary

Substituir a camada de renderização 2D (SDL2 rects/lines) por visual 3D isométrico top-down — paredes de tijolos individuais, caixotes engradados, personagem blocky — mantendo 100% da mecânica de gameplay existente (movimento 4 direções, empurrar caixa, undo, reiniciar fase, vitória, HUD de stats). Phase 1 não inclui menus elevador, perfis, intro cinematográfica ou animação de caminhada do personagem (v2).

</domain>

<decisions>
## Implementation Decisions

### Estética dos tiles 3D
- **D-01:** Paredes com **tijolos individuais** — blocos 3D separados visíveis, fiel ao vídeo de referência (não superfície plana com textura)
- **D-02:** Caixotes com **engradado detalhado** — tábuas e listras douradas/marrom como no original
- **D-03:** Personagem **humanóide blocky** — cabeça + corpo + pernas legíveis em visão top-down isométrica
- **D-04:** **Alvos e piso preservam marcadores 2D** — pontos/círculos no chão plano; não relevos 3D elaborados nos alvos

### Feedback de movimento
- **D-05:** Movimento do personagem com **deslize suave** — interpolação curta (~100–150 ms) entre células
- **D-06:** Empurrar caixa com **deslize animado** — personagem e caixa movem juntos visualmente
- **D-07:** **Bloquear input** durante animação de movimento — evita estados visuais inválidos
- **D-08:** **Undo sempre instantâneo** — sem animação reversa; comportamento lógico imediato como no 2D atual

### Luz e cor
- **D-09:** **Shading sutil** — mesma paleta CGA base, faces mais escuras/claras para ler volume 3D (sem sombras projetadas complexas)
- **D-10:** Paredes em **tom tijolo avermelhado/marrom** — mais próximo do vídeo original; detalhes e resto do tabuleiro mantêm espírito CGA
- **D-11:** Alvos como **pontos brilhantes** — ciano/amarelo forte sobre piso escuro para máxima legibilidade
- **D-12:** Caixa no alvo indicada por **mudança de cor** — equivalente visual ao `BOXG` atual (verde/dourado quando posicionada)

### HUD e cenas
- **D-13:** HUD **híbrido** — stats renderizados via overlay SDL2_ttf sobre viewport OpenGL; barra de status com fundo semi-transparente
- **D-14:** HUD na **barra inferior** da janela — tabuleiro ocupa área central superior
- **D-15:** Estilo HUD **retro CGA** — fundo escuro, texto ciano/magenta, sensação terminal/DOS
- **D-16:** Cena **TITLE com tratamento 3D simples** — mesmo pipeline de render 3D com texto overlay; consistência visual imediata (menus completos ficam para Phase 2)

### Claude's Discretion
- Duração exata da interpolação de movimento dentro da faixa ~100–150 ms
- Detalhe geométrico dos tijolos individuais (contagem de blocos por tile de parede) desde que leiam como tijolos separados
- Ordem exata de extração de módulos (`game_logic` → OpenGL) conforme STATE.md — sequência técnica, não decisão de produto

</decisions>

<canonical_refs>
## Canonical References

**Downstream agents MUST read these before planning or implementing.**

### Requisitos e escopo
- `.planning/ROADMAP.md` — Phase 1 goal, success criteria, requirement mapping (VIS-*, GAME-*, PLAT-*)
- `.planning/REQUIREMENTS.md` — Definições formais VIS-01–05, GAME-01–05, PLAT-01–03
- `.planning/PROJECT.md` — Core value, constraints (C + SDL2 + OpenGL), fidelidade *inspired*
- `.planning/STATE.md` — Decisões pré-fase (Painter's Algorithm, extração game_logic, HiDPI, dual platform)

### Referência visual e produto
- `descricao.md` — Brief do autor: tijolos, caixotes, personagem 3D; link ao vídeo YouTube
- https://www.youtube.com/watch?v=Rjw_q9WH1Ao — Referência visual principal (Spectrum HoloByte 1984 inspired)

### Arquitetura e pitfalls
- `.planning/research/ARCHITECTURE.md` — Separação game_logic / renderer_3d / gl_context; fluxo eventos→estado→render
- `.planning/research/PITFALLS.md` — Armadilhas conhecidas (big-bang refactor, macOS GL 4.1 cap, HiDPI, scope creep)

### Código existente
- `sokoban.c` — Implementação 2D atual: Board, move_player, draw_*, Scene enum, paleta C_*
- `Makefile` — Build targets; deve continuar funcionando com deps OpenGL adicionadas

</canonical_refs>

<code_context>
## Existing Code Insights

### Reusable Assets
- `Board` struct + `grid[MAXH][MAXW]` — lógica de tabuleiro intacta; renderer 3D lê grid, não modifica regras
- `move_player`, `load_level`, `is_won`, `push_undo`/`do_undo` — extrair para `game_logic` sem alteração de comportamento
- Paleta `C_*` (`C_CYAN`, `C_GOLD`, `C_DKMAG`, etc.) — base para materiais 3D com shading sutil
- `levels[]` + `NLEVELS` — dados de fase permanecem; loader inalterado
- `draw_text` / SDL2_ttf — reutilizar para overlay HUD híbrido (D-13)
- `Scene` enum (`SC_TITLE`, `SC_PLAY`, `SC_WIN`) — TITLE ganha render 3D simples; PLAY é foco principal

### Established Patterns
- Single-file `sokoban.c` (~420 linhas) — Phase 1 inicia modularização: extrair `game_logic` primeiro, depois OpenGL
- Global state `B`, `level_index`, `undo_stack` — preservar durante migração
- Painter's Algorithm (depth sort por col+row) — decidido em STATE.md para ordenação isométrica
- K&R braces, 4 spaces, `draw_*` naming — novos renderers seguem `draw_wall_3d`, `draw_box_3d`, etc.
- Erro SDL → stderr + exit — manter para init OpenGL/SDL

### Integration Points
- `main()` loop event-poll → update → render — substituir branch de render 2D por pipeline OpenGL + overlay HUD
- Funções `draw_wall`, `draw_floor`, `draw_box`, `draw_player` — candidatas a substituição total por equivalentes 3D
- `SDL_CreateWindow` → migrar para contexto OpenGL (`SDL_GL_CreateContext`); viewport via `SDL_GL_GetDrawableSize` (HiDPI)
- Makefile — adicionar flags OpenGL (`-lGL` Linux), GLAD/cglm conforme ARCHITECTURE.md

</code_context>

<specifics>
## Specific Ideas

- Vídeo YouTube (link em `descricao.md`) é a referência visual primária para tijolos, caixotes e espírito retro
- Tijolos devem ser blocos 3D **individuais e visíveis**, não atalho com textura plana
- Caixotes devem evocar **engradado de madeira dourada** do original, não cubo LEGO genérico
- Movimento animado é desejável para personagem e caixa, mas undo deve permanecer **snappy/instantâneo**
- HUD retro CGA na barra inferior; tabuleiro 3D domina a área visual principal

</specifics>

<deferred>
## Deferred Ideas

None — discussion stayed within phase scope

Itens explicitamente fora desta fase (já no roadmap, não discutidos como escopo):
- Animação de caminhada do personagem (GAME-07, v2)
- Menus elevador completos (Phase 2)
- Intro cinematográfica (Phase 4)
- Câmera rotacionável (Out of Scope em REQUIREMENTS.md)

</deferred>

---

*Phase: 1-3D Foundation*
*Context gathered: 2026-05-29*
