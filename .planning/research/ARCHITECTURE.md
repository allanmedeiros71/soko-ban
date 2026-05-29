# Architecture Patterns — Sokoban 3D Brownfield Evolution

**Domain:** 2D → 3D game (single-file C/SDL2 brownfield)
**Researched:** 2026-05-29
**Confidence:** HIGH (based on direct codebase analysis + established C/SDL2/OpenGL patterns)

---

## Recommended Architecture

### Princípio central: separar lógica de renderização

O `sokoban.c` atual já tem uma divisão natural boa: a lógica de tabuleiro (`Board`, `move_player`, `load_level`, `is_won`, undo) é isolada e não chama SDL diretamente. O problema é que está no mesmo arquivo que a renderização 2D e o loop principal.

A evolução deve **extrair** essa lógica sem modificá-la, e **substituir** a camada de render — não o contrário.

### Diagrama geral

```
┌──────────────────────────────────────────────────────────┐
│                        main.c                            │
│   SDL init → OpenGL context → scene_manager loop         │
└────────────────────┬─────────────────────────────────────┘
                     │
         ┌───────────▼────────────┐
         │     scene_manager      │   scene atual + transições
         │  (SC_INTRO → MENU →    │
         │   USER → LEVEL → PLAY) │
         └───┬───────────┬────────┘
             │           │
    ┌────────▼──┐   ┌────▼──────────────┐
    │  scenes/  │   │   game_logic      │   Board, move, undo
    │  intro    │   │   (inalterado)    │   extraído de sokoban.c
    │  menu     │   └────────┬──────────┘
    │  user     │            │
    │  levels   │   ┌────────▼──────────┐
    │  play ────┼──►│   renderer_3d     │   OpenGL tile drawing
    └───────────┘   └────────┬──────────┘
                             │
                    ┌────────▼──────────┐
                    │   gl_context      │   shaders, VAO/VBO
                    └───────────────────┘

    ┌─────────────┐
    │   profile   │   ~/.soko-ban/save.dat  (independente)
    └─────────────┘
```

---

## Limites de componentes

| Componente | Arquivo(s) | Responsabilidade | Comunica com |
|------------|-----------|-----------------|--------------|
| **main** | `src/main.c` | SDL init, OpenGL context, loop principal | scene_manager |
| **game_logic** | `src/game_logic.c/.h` | Board, move_player, load_level, is_won, undo — **zero SDL** | scenes/play |
| **scene_manager** | `src/scene.c/.h` | Estado de cena atual, dispatch de eventos, transições | todas as scenes |
| **renderer_3d** | `src/renderer_3d.c/.h` | OpenGL: câmera isométrica, draw_wall_3d, draw_box_3d, draw_player_3d | scenes/play, gl_context |
| **gl_context** | `src/gl_context.c/.h` | Compilar shaders, criar VAO/VBO para geometria dos tiles | renderer_3d |
| **profile** | `src/profile.c/.h` | Ler/escrever perfis de usuário em arquivo, CRUD | scenes/user, scenes/play |
| **scenes/intro** | `src/scenes/intro.c` | Animação de abertura (título, fade, sequência) | scene_manager, renderer_3d |
| **scenes/menu** | `src/scenes/menu.c` | Elevator menu (Play/Edit/Exit), animação vertical | scene_manager |
| **scenes/user** | `src/scenes/user.c` | Cadastro/seleção de usuário, entrada de nome | scene_manager, profile |
| **scenes/levels** | `src/scenes/levels.c` | Seleção de fases como andares do elevador | scene_manager, profile |
| **scenes/play** | `src/scenes/play.c` | Loop de jogo: eventos → game_logic → renderer_3d | game_logic, renderer_3d, profile |

---

## Fluxo de dados

### Eventos → Estado → Render

```
SDL_PollEvent
      │
      ▼
scene_manager.handle_event(event)
      │
      ├── SC_PLAY ──► scenes/play.handle_key()
      │                     │
      │               move_player() / do_undo() / load_level()
      │                     │
      │               Board.grid mutado in-place   ← inalterado do original
      │                     │
      │               is_won() ──► scene_transition(SC_WIN / SC_LEVEL_SELECT)
      │
      ├── SC_MENU ──► scenes/menu.handle_key()
      │                     │
      │               elevator_state.target_floor ± 1
      │
      └── SC_INTRO ──► (auto-progride por timer)

─── frame boundary ───────────────────────────────────

scene_manager.update(dt_ms)
      │
      └── current_scene.update(dt_ms)
              │  (animações de elevador, timer de intro, etc.)

scene_manager.render()
      │
      ├── SC_PLAY ──► renderer_3d.draw_board(Board*)
      │                     │
      │               para cada tile: draw_wall_3d / draw_box_3d / etc.
      │               câmera isométrica fixa
      │
      ├── SC_MENU ──► scenes/menu.render()
      │               (SDL2_ttf para texto, OpenGL para fundo 3D)
      │
      └── SC_INTRO ──► scenes/intro.render()

SDL_GL_SwapWindow (ou SDL_RenderPresent para cenas 2D puras)
```

### Transições de cena

```
SC_INTRO
  └─► SC_MENU_ELEVATOR  (intro completa)

SC_MENU_ELEVATOR
  ├─► SC_USER_SELECT   (Play selecionado)
  ├─► SC_EDIT          (Edit — placeholder v1)
  └─► quit             (Exit)

SC_USER_SELECT
  └─► SC_LEVEL_SELECT  (usuário criado/selecionado)

SC_LEVEL_SELECT
  └─► SC_PLAY          (andar/fase escolhido)

SC_PLAY
  ├─► SC_WIN           (is_won() == true)
  └─► SC_LEVEL_SELECT  (ESC / voltar)

SC_WIN
  └─► SC_LEVEL_SELECT  (próxima fase)
```

---

## Padrões a seguir

### Padrão 1: Interface de cena (vtable leve)

```c
// scene.h
typedef struct Scene {
    void (*handle_event)(SDL_Event *e);
    void (*update)(Uint32 dt_ms);
    void (*render)(void);
    void (*on_enter)(void);   // inicializar estado ao entrar
    void (*on_exit)(void);    // limpar ao sair
} Scene;

// scene_manager.c
static Scene *current = NULL;

void scene_push(Scene *next) {
    if (current && current->on_exit) current->on_exit();
    current = next;
    if (current->on_enter) current->on_enter();
}
```

**Quando usar:** toda cena implementa este contrato. Nenhuma cena conhece outra diretamente — todas as transições passam pelo scene_manager.

### Padrão 2: game_logic sem SDL (zero coupling)

```c
// game_logic.h — zero SDL, zero OpenGL
void load_level(int idx);
void move_player(int dx, int dy);
bool is_won(void);
void push_undo(void);
void do_undo(void);
extern Board B;           // estado global do tabuleiro
```

**Regra:** `game_logic.c` não inclui `<SDL.h>` em nenhuma circunstância. Só `<string.h>`, `<stdbool.h>`, `<stdlib.h>`. Isso garante que a lógica permanece testável e portável.

### Padrão 3: Renderer 3D isométrico (OpenGL mínimo)

```c
// renderer_3d.h
void r3d_init(void);          // compila shaders, cria VAOs
void r3d_begin_frame(void);   // glClear + set camera matrix
void r3d_draw_board(Board *b, int ox, int oy);  // itera grid, dispatch
void r3d_end_frame(void);     // flush
void r3d_shutdown(void);
```

**Câmera isométrica:**
- Projeção ortográfica (não perspectiva) — mantém estética retro
- Rotação Y = 45°, inclinação X = 30° (isométrico clássico)
- View matrix fixa por frame, calculada uma vez em `r3d_begin_frame`

**Geometria procedural — sem arquivos externos:**
```c
// Wall: cubo unitário com 4 fileiras de tijolos (face frontal = subdivide em rects)
// Box:  cubo unitário com borda escura + diagonal cruzada (fiel ao original)
// Floor: quad plano (y=0), goal = diamond outline inscrito
// Player: 3 cubos compostos (cabeça 0.4×0.4, corpo 0.3×0.5, pernas)
```

### Padrão 4: Elevator state machine (animação suave)

```c
// menu state
typedef struct {
    int   current_floor;    // 0=Exit, 1=Edit, 2=Play
    int   target_floor;
    float anim_y;           // posição Y atual (interpolada)
    float anim_speed;       // pixels/ms
    bool  moving;
} ElevatorState;

// update loop
void menu_update(Uint32 dt_ms) {
    float target_y = target_floor * FLOOR_HEIGHT;
    anim_y = lerp(anim_y, target_y, dt_ms * anim_speed);
    moving = fabsf(anim_y - target_y) > 0.5f;
}
```

**Efeito:** os labels dos andares sobem/descem suavemente. Pressionar cima/baixo altera `target_floor`; a animação interpola.

### Padrão 5: Profile storage (arquivo de texto simples)

```c
// Formato: ~/.soko-ban/users.dat
// NOME_USUARIO:nivel_atual:moves_best\n
// Ex: alice:3:147\n

typedef struct {
    char  name[32];
    int   current_level;
    int   best_moves[NLEVELS];
} UserProfile;
```

**Sem dependência JSON** — formato linha-por-linha simples, lido com `fgets`/`sscanf`. Fácil de debugar, zero bibliotecas externas.

---

## Anti-padrões a evitar

### Anti-Padrão 1: Misturar SDL_Renderer + OpenGL no mesmo frame

**O que dá errado:** SDL_Renderer usa o pipeline de render próprio (pode ser D3D, Metal, OpenGL). Misturar `SDL_RenderFillRect` com chamadas OpenGL diretas no mesmo frame causa artefatos visuais ou crash.

**Em vez disso:** Escolher um dos dois por frame:
- **Cenas 3D** (play, intro com efeitos): usar OpenGL direto, `SDL_GL_SwapWindow`.
- **Cenas 2D puras** (user input, texto): usar `SDL_Renderer` + `SDL_GL_SwapWindow` (SDL consegue coexistir se o renderer for criado com `SDL_RENDERER_SOFTWARE` e blitado como textura para o contexto GL). Solução mais simples: **usar OpenGL para tudo**, renderizar texto via SDL_ttf → SDL_Surface → glTexImage2D.

### Anti-Padrão 2: Alterar game_logic para acomodar 3D

**O que dá errado:** Adicionar `float world_x, world_y, world_z` ao `Board` struct, ou fazer `move_player` chamar o renderer para animar o movimento.

**Em vez disso:** O renderer 3D lê `Board.grid` a cada frame e calcula a posição world-space a partir de `(grid_x * TILE_SIZE, 0, grid_y * TILE_SIZE)`. Animações de movimento são estado **do renderer**, não da lógica.

### Anti-Padrão 3: Scene global state via variáveis soltas

**O que dá errado:** O código atual já tem `static int level_index` e `static Board B` soltos em `sokoban.c`. Escalar isso para 6 cenas com dezenas de variáveis vira caos.

**Em vez disso:** Cada cena tem seu próprio struct de estado, instanciado estaticamente:

```c
// menu.c
static ElevatorState g_menu = { .current_floor = 2, .target_floor = 2 };
// Acessível apenas dentro de menu.c — encapsulamento via translation unit.
```

---

## Estrutura de arquivos recomendada

```
soko-ban/
├── sokoban.c               ← manter intocado (referência + build original)
├── Makefile                ← atualizar target "new" para novo build
├── assets/
│   ├── fonts/RobotoMono-Bold.ttf
│   └── shaders/
│       ├── tile.vert       ← vertex shader (MVP transform)
│       └── tile.frag       ← fragment shader (cor por face + luz direcional)
└── src/
    ├── main.c
    ├── game_logic.c / .h   ← extraído de sokoban.c, zero mudanças
    ├── gl_context.c / .h   ← SDL_GL_CreateContext, shader compile, VAO/VBO
    ├── renderer_3d.c / .h  ← isometric camera, draw_*_3d
    ├── scene.c / .h        ← Scene interface + scene_push/pop
    ├── profile.c / .h      ← leitura/escrita de perfis locais
    └── scenes/
        ├── intro.c         ← animação de abertura
        ├── menu.c          ← elevator menu (Play/Edit/Exit)
        ├── user.c          ← cadastro/seleção de usuário
        ├── levels.c        ← seleção de fases como andares
        └── play.c          ← gameplay: game_logic + renderer_3d
```

---

## Implicações de ordem de build (fases MVP)

### Fase 1 — Fundação OpenGL + renderer_3d (sem quebrar gameplay)

**Deliverable:** O jogo atual funciona, mas os tiles são renderizados em 3D isométrico.

Dependências primeiro:
1. `gl_context`: SDL_GL_CreateContext, shader compile/link, VAO unit cube
2. `renderer_3d.init()` → câmera isométrica + draw_wall_3d, draw_box_3d, draw_player_3d
3. Conectar renderer_3d no loop atual (substituir `draw_wall` → `draw_wall_3d`)

**Critério de aceite:** `make && ./sokoban` continua jogável, tiles aparecem em 3D.

**Risco:** Compatibilidade OpenGL em macOS (`#version 410 core`; requer `SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE)`).

---

### Fase 2 — Sistema de cenas + Elevator Menu

**Deliverable:** Ao abrir o jogo, aparece menu de elevador (Play/Edit/Exit).

Dependências:
1. `scene.c`: Scene interface, scene_push
2. `scenes/menu.c`: ElevatorState, animação lerp, render com texto
3. Transição `SC_MENU → SC_PLAY`

**Critério de aceite:** Navegar com setas no menu, selecionar Play inicia o jogo.

---

### Fase 3 — User profiles + Level Select

**Deliverable:** Após Play, selecionar/criar usuário; escolher fase como andar do elevador.

Dependências:
1. `profile.c`: ler/escrever `~/.soko-ban/users.dat`
2. `scenes/user.c`: input de nome, listagem de perfis
3. `scenes/levels.c`: andares = fases, highlight do progresso do usuário
4. Salvar `current_level` ao completar fase

**Critério de aceite:** Criar "alice", jogar fase 1, fechar, reabrir → alice continua no nível 1.

---

### Fase 4 — Animação de abertura

**Deliverable:** Ao iniciar, sequência cinematográfica antes do menu.

Dependências:
1. `scenes/intro.c`: timer-driven, sequência de frames OpenGL (fade title, flash)
2. Transição automática → `SC_MENU_ELEVATOR`

**Critério de aceite:** Intro roda ≤ 5s, skip com qualquer tecla, transita para menu.

---

## Considerações de escalabilidade

| Preocupação | Agora (4 fases) | Com mais fases |
|-------------|-----------------|----------------|
| Geometria 3D | Procedural em C, zero arquivos | Manter procedural; opcional: .obj loader simples |
| Level data | Strings hardcoded | Carregar `.sok` de `assets/levels/` via `fopen` |
| Profiles | Arquivo plano por linha | Manter formato; ~100 users = ~3 KB |
| Shaders | 2 arquivos (vert/frag) | Adicionar shader de pós-processamento (CRT scanline) se desejado |

---

## Fontes

- Análise direta de `sokoban.c` (391 linhas) — HIGH confidence
- SDL2 + OpenGL integration: [SDL2 docs](https://wiki.libsdl.org/SDL2/SDL_GL_CreateContext) — HIGH confidence
- macOS OpenGL 4.1 deprecation: [Apple developer docs](https://developer.apple.com/library/archive/documentation/GraphicsImaging/Conceptual/OpenGL-MacProgGuide/) — MEDIUM confidence (confirmado via codebase + conhecimento de plataforma)
- Isometric projection math: padrão da indústria para jogos grid-based — HIGH confidence
