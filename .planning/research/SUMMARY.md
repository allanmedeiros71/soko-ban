# Project Research Summary

**Project:** Soko-ban — Releitura 3D Retro
**Domain:** Brownfield retro game remake (C/SDL2 → 3D, macOS target)
**Researched:** 2026-05-29
**Confidence:** HIGH

## Executive Summary

Este é um projeto brownfield de releitura nostálgica: partir de um Sokoban funcional em C/SDL2 (~420 linhas, single-file) e evoluir para uma experiência 3D inspirada no original de 1984, com menus em formato de elevador animado, cadastro local de usuários e visuais de tijolos/caixotes em 3D isométrico. O ponto central da diferenciação não é gráficos avançados — é a fidelidade ao *espírito* do original (fluxo de elevador, paleta retro, progressão por andares) traduzida para C moderno sobre OpenGL 3.3.

A abordagem recomendada pelos pesquisadores é conservadora e incremental: manter SDL2 (não migrar para SDL3), adicionar OpenGL 3.3 Core Profile como camada de render exclusiva (substituindo o SDL2_Renderer 2D), e usar bibliotecas de arquivo único (GLAD 2, cglm 0.9.6, cJSON 1.7.18) que entram no repositório sem dependências externas. A lógica de tabuleiro existente (`Board`, `move_player`, `undo`) não deve ser tocada — o que muda é exclusivamente a camada de render e a adição de sistema de cenas (intro → menu → usuário → seleção → jogo).

O maior risco de projeto não é técnico: é o **scope creep visual**. A animação do elevador e a intro cinematográfica são atraentes e podem consumir todo o tempo do MVP sem entregar gameplay completo. O segundo risco é técnico: macOS exige cuidado explícito com contexto OpenGL na main thread, HiDPI (DrawableSize ≠ WindowSize), e limite de OpenGL 4.1. Ambos os riscos têm mitigação clara se abordados desde a primeira fase.

---

## Key Findings

### Recommended Stack

A stack recomendada evolui o projeto sem quebrar o que já funciona. SDL2 permanece responsável por janela, input, timer e audio; apenas o render muda. O OpenGL 3.3 Core Profile é suficiente para cubos low-poly com iluminação Phong — não há nenhuma razão para GL 4.x no macOS, onde o teto é 4.1. GLAD 2 (dois arquivos: `glad.c` + `glad.h`) substitui GLEW sem dependência de sistema. cglm é header-only e SIMD-aware (NEON para Apple Silicon). cJSON provê persistência de perfis sem SQLite ou parser custom.

**Core technologies:**

| Tecnologia | Papel | Justificativa |
|-----------|-------|---------------|
| **C11** | Linguagem principal | Codebase existente; sem custo de migração |
| **SDL2 2.30** | Janela, input, timer | Já em uso; `SDL_GL_CreateContext` integra OpenGL sem mudar nada mais |
| **OpenGL 3.3 Core** | Renderização 3D | Suficiente para cubos Phong; universal em macOS/Linux; documentação excelente |
| **GLAD 2** | GL extension loader | Dois arquivos no repo; sem dependências externas; moderno vs. GLEW |
| **cglm 0.9.6** | Matemática 3D | Header-only; SIMD (SSE/NEON); API familiar (`glm_perspective`, `glm_lookat`) |
| **cJSON 1.7.18** | Perfis de usuário | Dois arquivos, MIT, zero dependências; perfeito para `{name, lastLevel, bestMoves}` |
| **SDL2_ttf 2.22** | Texto HUD/menus | Já em uso; gera texturas para pipeline OpenGL |

**Não usar:** SDL3/SDL_GPU (migração de API cara), raylib (substitui SDL2 em vez de complementar), Vulkan (1000+ linhas de boilerplate sem ganho visual), GLEW (dep. de sistema), CMake (Makefile basta).

→ Detalhes em `.planning/research/STACK.md`

---

### Expected Features

A maioria das table stakes já existe na codebase. O projeto acrescenta visuais 3D e toda a camada de UX de elevador que está ausente.

**Must have (table stakes):**
- Undo ilimitado, reiniciar nível, condição de vitória — **já existem**
- Controles responsivos (setas/WASD) — **já existem**
- Menu principal + animação de abertura — **a implementar**
- Seleção de nível — **a implementar** (formato elevador)
- Paredes/caixotes/personagem visualmente 3D — **a implementar**

**Should have (differentiators):**
- Menu elevador animado (Play/Edit/Exit como andares) — identidade visual do projeto
- Intro cinematográfica inspirada no original 1984
- Perfis locais de usuário (arquivo JSON, sem backend)
- Seleção de nível como andares do elevador (coerência visual total)
- HUD com contador de movimentos e empurrões
- Melhor score por perfil por fase

**Defer (v2+):**
- Editor de fases completo (stub/placeholder em v1)
- Tweening de movimento (snap-to-grid é mais fiel ao original)
- Detecção automática de deadlock (undo generoso resolve o mesmo problema)
- Multiplayer / ranking online
- Suporte mobile/touch

→ Detalhes em `.planning/research/FEATURES.md`

---

### Architecture Approach

A arquitetura proposta separa claramente três preocupações: **lógica de jogo** (zero SDL/OpenGL, extraída do monólito para `game_logic.c`), **gerenciamento de cenas** (vtable leve em `scene.c` com `handle_event/update/render/on_enter/on_exit`) e **renderer 3D** (`renderer_3d.c` recebe um `Board*` somente leitura, calcula posições world-space a partir do grid, sem tocar em estado de jogo). Cada cena encapsula seu estado em struct estático local — sem variáveis globais compartilhadas entre cenas.

**Major components:**

| Componente | Arquivo | Responsabilidade |
|-----------|---------|-----------------|
| **main** | `src/main.c` | SDL init, OpenGL context, loop principal |
| **game_logic** | `src/game_logic.c/.h` | Board, move, undo, is_won — **zero SDL** |
| **scene_manager** | `src/scene.c/.h` | Estado atual, dispatch de eventos, transições |
| **renderer_3d** | `src/renderer_3d.c/.h` | Câmera isométrica, draw_wall/box/player 3D |
| **gl_context** | `src/gl_context.c/.h` | Shaders, VAO/VBO, unidade de compilação GL |
| **profile** | `src/profile.c/.h` | Leitura/escrita de perfis locais em arquivo |
| **scenes/** | 5 arquivos | intro, menu, user, levels, play |

**Fluxo de cenas:** `SC_INTRO → SC_MENU_ELEVATOR → SC_USER_SELECT → SC_LEVEL_SELECT → SC_PLAY → SC_WIN → SC_LEVEL_SELECT`

→ Detalhes em `.planning/research/ARCHITECTURE.md`

---

### Critical Pitfalls

1. **macOS OpenGL 4.1 cap** — Shaders com `#version 430+` ou UBOs `std430` compilam no Linux e crasham silenciosamente no macOS. Prevenção: usar `#version 330 core` (ou no máximo `#version 410 core`) e pedir contexto explicitamente com `SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3)`. Testar no macOS na primeira compilação com GL, não depois.

2. **HiDPI/Retina — DrawableSize ≠ WindowSize** — `SDL_GetWindowSize` retorna dimensões lógicas; `SDL_GL_GetDrawableSize` retorna 2× em Retina. Usar `glViewport` com `GetDrawableSize`; converter mouse-click com fator de escala explícito. Nunca hardcodar dimensões de janela para operações de render.

3. **Big-bang refactoring** — Tentar extrair `game.c` e adicionar 3D ao mesmo tempo remove o único sinal de sanidade (jogo rodando). Regra de ouro: **uma mudança por vez, jogo sempre jogável ao final de cada commit**. Sequência: split → interface limpa → 3D incremental.

4. **Scope creep da animação do elevador** — O menu de elevador é visualmente concreto e pode consumir 60-80% do tempo sem entregar gameplay. Estratégia: implementar placeholder funcional (menu de texto simples) primeiro; adicionar animação por cima como polish separado. Critério de done: "usuário consegue selecionar Play e entrar no jogo."

5. **Z-fighting em tiles isométricos** — Polígonos coplanares (chão + parede, caixa sobre chão) causam tremulação visual com depth buffer padrão. Decisão arquitetural antes de começar a fase 3D: usar **Painter's Algorithm** (renderizar de trás para frente por `col + row`) em vez de depth buffer — correto para ortogonal/isométrico 2.5D.

→ Detalhes em `.planning/research/PITFALLS.md`

---

## Implications for Roadmap

### Fase 1: Fundação 3D — OpenGL + Renderer

**Rationale:** Nenhuma outra fase pode começar sem a camada de render 3D. A lógica de jogo existente já é o MVP de gameplay; o bloqueio é o visual. Esta fase também estabelece a estrutura de arquivos (src/, scenes/, shaders/) que tudo mais usa.

**Delivers:** O jogo atual (`sokoban.c`) continua jogável, mas tiles aparecem em 3D isométrico. Paredes com tijolos, caixotes com profundidade, personagem blocky. Gameplay 100% preservado.

**Features implementadas (de FEATURES.md):**
- Paredes 3D com tijolos fiéis ao original
- Caixotes 3D (normal + em-alvo)
- Personagem 3D (cubo composto)

**Componentes de arquitetura (de ARCHITECTURE.md):**
- `gl_context.c`: SDL_GL_CreateContext, shader compile, VAO unit cube
- `renderer_3d.c`: câmera isométrica, draw_wall_3d, draw_box_3d, draw_player_3d
- Makefile atualizado: `gcc src/main.c src/game_logic.c glad/glad.c ...`

**Pitfalls a evitar:**
- Pitfall 1: `#version 330 core`; testar no macOS primeiro
- Pitfall 2: `SDL_GL_GetDrawableSize` para viewport; `SDL_WINDOW_ALLOW_HIGHDPI`
- Pitfall 3: todo código SDL/GL na main thread
- Pitfall 4: **não** extrair `game_logic` e adicionar 3D na mesma fase; fazer primeiro a extração, depois o 3D
- Pitfall 5: definir estratégia de profundidade (Painter's Algorithm) antes de começar render

**Research flag:** Needs `/gsd-plan-phase --research-phase` — primeiro contato com OpenGL no projeto; macOS specifics precisam de verificação early.

---

### Fase 2: Sistema de Cenas + Menu Elevador

**Rationale:** Com o renderer 3D funcionando, a segunda prioridade é a identidade visual do projeto. O menu elevador é o núcleo da diferenciação; sem ele, o jogo não tem entrada. O scene_manager é o scaffolding que todas as fases seguintes dependem.

**Delivers:** Ao abrir o jogo, aparece o menu de elevador com Play/Edit/Exit. Navegação por setas. Selecionar Play entra no jogo. Selecionar Exit fecha. Edit é stub/placeholder.

**Features implementadas (de FEATURES.md):**
- Menu principal estilo elevador (diferenciador central)
- Intro cinematográfica (opcional nesta fase; pode ser fase separada se complexidade exigir)
- Placeholder para Edit mode

**Componentes de arquitetura:**
- `scene.c`: Scene interface (vtable), scene_push/pop
- `scenes/menu.c`: ElevatorState, animação lerp, render texto SDL_ttf → GL texture
- `scenes/intro.c`: timer-driven, fade title, transição automática → menu (≤5s, skip com tecla)

**Pitfalls a evitar:**
- Pitfall 7: nostalgia trap — time-box intro a 3 dias; elevator a 5 dias
- Pitfall 9: placeholder funcional antes da animação; critério de done sem animação perfeita
- Pitfall 10: usar delta time real (`SDL_GetTicks()` diff), não `SDL_Delay(16)` fixo
- Pitfall 1: Anti-padrão SDL_Renderer + OpenGL no mesmo frame — usar OpenGL para tudo (texto via SDL_ttf → glTexImage2D)

**Research flag:** Standard patterns (vtable de cenas é padrão bem documentado em C game dev). Animação de elevador pode precisar de pesquisa de easing curves, mas é optional polish.

---

### Fase 3: Perfis de Usuário + Seleção de Nível

**Rationale:** Com jogo jogável e menu funcional, o próximo passo é personalização e progressão. Os perfis são pré-requisito para a seleção de nível (saber quais fases estão desbloqueadas por usuário). Esta fase completa a metáfora do elevador estendendo-a para escolha de fase.

**Delivers:** Após selecionar Play, o jogador cria ou seleciona um perfil local. Em seguida, vê a tela de seleção de níveis como andares do elevador. Ao completar uma fase, o progresso é salvo. Reabrir o jogo preserva o estado.

**Features implementadas (de FEATURES.md):**
- Perfis locais de usuário (nome, nível atual, melhor score)
- Tela de cadastro/seleção de usuário
- Seleção de nível como andares do elevador
- Melhor score por perfil por fase (persiste em arquivo)
- HUD com contador de movimentos e empurrões

**Componentes de arquitetura:**
- `profile.c`: ler/escrever `~/.soko-ban/users.dat` (formato linha-por-linha ou JSON via cJSON)
- `scenes/user.c`: input de nome, listagem de perfis existentes
- `scenes/levels.c`: andares = fases, highlight de progresso por perfil

**Pitfalls a evitar:**
- Pitfall 6: `SDL_GetBasePath()` + `chdir()` no início de `main()` antes de qualquer `fopen`
- Pitfall 11: fonte bundled `RobotoMono-Bold.ttf` via `SDL_GetBasePath()`, não path Linux hardcoded
- Pitfall 12: undo stack silencioso após 4096 movimentos — corrigir aqui (ring buffer ou aviso visual)
- Pitfall 14: `.gitignore` com `sokoban` e `*.o` antes do primeiro commit desta fase

**Research flag:** Standard patterns (leitura de arquivo de texto em C, validação de input de nome). cJSON é bem documentado se escolhido; alternativa: formato linha-por-linha com `fgets`/`sscanf` (zero deps).

---

### Fase 4: Polish + Animação de Abertura Completa

**Rationale:** Com todas as funcionalidades do MVP em pé (jogo 3D, menu, perfis, seleção), esta fase eleva a qualidade estética: intro cinematográfica completa, polish da animação do elevador, paleta CGA retro no contexto 3D, e refinamentos de UX.

**Delivers:** Experiência completa e polida. Intro roda ≤5s (skip com tecla), transita suavemente para menu. Elevador com animação fluida (lerp com easing). Paleta retro coerente. Binário final compilável em macOS e Linux.

**Features implementadas (de FEATURES.md):**
- Animação de abertura cinematográfica finalizada
- Paleta CGA retro no contexto 3D (visual coerente)
- Refinamento da animação do elevador (easing, timing)
- Bug fixes e estabilidade

**Pitfalls a evitar:**
- Pitfall 7: definir "done" antes de começar — lista fechada de polishes, não lista aberta
- Pitfall 9: não adicionar features novas, apenas polish do que já existe
- Pitfall 10: verificar loop de tempo final com delta time real em todos os sistemas de animação

**Research flag:** Standard patterns (easing functions são biblioteca de 10 linhas; GLSL fragment shader para efeito CRT/scanline é bem documentado).

---

### Phase Ordering Rationale

- **3D antes de menu:** Sem renderer 3D, o menu não pode usar visual 3D como fundo. A fundação técnica (OpenGL context, VAO/VBO, câmera isométrica) é base de todas as fases.
- **Menu antes de perfis:** O scene_manager criado na Fase 2 é pré-requisito para `scenes/user.c` e `scenes/levels.c` da Fase 3.
- **Perfis antes de polish:** Não faz sentido polir animações em cima de um fluxo de UX incompleto. MVP funcional primeiro, estética depois.
- **Extração de game_logic dentro da Fase 1:** A separação da lógica de jogo é condição para adicionar 3D sem big-bang refactoring (Pitfall 4). Deve ser o primeiro commit da Fase 1, verificado com o jogo ainda rodando antes de tocar em OpenGL.

### Research Flags

**Precisa de pesquisa durante planejamento:**
- **Fase 1** — Integração SDL2 + OpenGL no macOS Sequoia (Retina, Core Profile, framework linking); verificar cglm + GLAD 2 com build macOS arm64 atual
- **Fase 2** — Easing curves para animação do elevador (optional; pesquisa rápida de 30 min)

**Padrões bem estabelecidos (pode pular pesquisa):**
- **Fase 3** — Leitura/escrita de arquivo de texto em C, validação de input; cJSON amplamente documentado
- **Fase 4** — GLSL fragment shaders para post-processing retro; easing functions (stdlib de 10 linhas)

---

## Confidence Assessment

| Área | Confiança | Notas |
|------|-----------|-------|
| Stack | **HIGH** | SDL2 + OpenGL 3.3 no macOS confirmado via SDL docs e SO; GLAD 2 e cglm verificados via repositórios oficiais |
| Features | **HIGH** | Baseado em análise direta do código existente + fontes primárias (Internet Archive Sokoban 1984) |
| Architecture | **HIGH** | Baseado em análise direta de `sokoban.c` + padrões estabelecidos de C game dev |
| Pitfalls | **HIGH** | Fontes primárias: SDL GitHub issues, Khronos forums, Stack Overflow com exemplos reproducíveis |

**Overall confidence: HIGH**

### Gaps to Address

- **Risco OpenGL deprecation macOS:** Apple deprecated OpenGL em 2018 mas nunca removeu (8 anos). Risco real mas baixo no horizonte v1. Mitigação: OpenGL sobre Metal funciona hoje; plano de contingência seria migrar para SDL_GPU (SDL3) em v2.
- **Tamanho dos modelos 3D procedurais:** Geometria de cubo composto para o personagem ainda não validada visualmente — pode requerer iteração na Fase 1 antes de "aceitar" o visual.
- **Performance em grid grande:** Codebase atual tem 4 fases com grade pequena. Se fases maiores forem adicionadas, renderizar tile por tile pode precisar de instanced drawing (GL 3.3 suporta via `glDrawArraysInstanced`).

---

## Sources

### Primary (HIGH confidence)
- Análise direta de `sokoban.c` (~420 linhas) — arquitetura atual, pontos de extensão
- [SDL2 docs — SDL_GL_CreateContext](https://wiki.libsdl.org/SDL2/SDL_GL_CreateContext) — integração SDL2+OpenGL
- [SDL2 GitHub Issue #3507](https://github.com/libsdl-org/SDL/issues/3507) — NSOpenGLContext main thread crash
- [Internet Archive Soko-Ban 1984](https://archive.org/details/soko-ban_1984) — fonte primária do jogo original
- [Wikipedia Sokoban](https://en.wikipedia.org/wiki/Sokoban) — histórico e regras canônicas

### Secondary (MEDIUM confidence)
- [Apple developer docs — OpenGL Mac](https://developer.apple.com/library/archive/documentation/GraphicsImaging/Conceptual/OpenGL-MacProgGuide/) — deprecation status
- [SDL2 Discourse — Mojave HiDPI DrawableSize](https://discourse.libsdl.org/t/macos-10-14-mojave-issues/25060)
- [Khronos Forums — Z-fighting near/far planes](https://community.khronos.org/t/terrain-z-fighting-issues/61820)
- [mazebert.com — Isometric depth sorting](https://mazebert.com/forum/news/isometric-depth-sorting--id775/)
- GameFAQs Walkthrough Soko-Ban (odino) — UX original documentada

### Tertiary (LOW confidence)
- [GameAnalytics — Immersive Game Intros](https://www.gameanalytics.com/blog/create-immersive-game-intros) — design advice geral
- [Game Developer — HD Remake the Right Way](https://www.gamedeveloper.com/design/doing-an-hd-remake-the-right-way) — orientação geral para remakes
- Clone moderno sudokatie/crates — feature list útil como referência

---
*Research completed: 2026-05-29*
*Ready for roadmap: yes*
