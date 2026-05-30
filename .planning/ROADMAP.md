# Roadmap: Soko-ban — Primeira versão 3D

**Created:** 2026-05-29
**Milestone:** v1 — Releitura 3D Retro
**Project Mode:** MVP Vertical — cada fase entrega uma fatia jogável end-to-end
**Granularity:** Coarse (4 phases)

---

## Phases

- [ ] **Phase 1: 3D Foundation** — OpenGL + tiles isométricos, gameplay 100% preservado
- [ ] **Phase 2: Elevator Menu** — sistema de cenas, menu principal Exit/Edit/Play
- [ ] **Phase 3: Profiles & Level Select** — usuários locais, andares do elevador para fases
- [ ] **Phase 4: Opening Intro** — animação de abertura cinematográfica e polish

---

## Phase Details

### Phase 1: 3D Foundation
**Goal**: O jogo atual roda com visual 3D isométrico — paredes de tijolo, caixotes e personagem em 3D — e toda mecânica de gameplay está 100% preservada
**Mode:** mvp
**Depends on**: Nothing (first phase)
**Requirements**: VIS-01, VIS-02, VIS-03, VIS-04, VIS-05, GAME-01, GAME-02, GAME-03, GAME-04, GAME-05, PLAT-01, PLAT-02, PLAT-03
**Success Criteria** (what must be TRUE):
  1. Jogador abre o jogo e vê o tabuleiro em perspectiva isométrica 3D — paredes com tijolos, caixotes com volume, personagem blocky legível
  2. Toda mecânica existente funciona sem regressão: mover em 4 direções, empurrar uma caixa por vez, undo, reiniciar fase, detectar vitória
  3. HUD exibe fase atual, contador de movimentos, empurrões e tempo durante a partida
  4. Jogo compila e executa via `make` em macOS (HiDPI/Retina) e Linux sem distorção de viewport OpenGL
  5. Paleta retro CGA e marcadores de alvo preservados na estética 3D
**Plans**: TBD

### Phase 2: Elevator Menu
**Goal**: Ao abrir o jogo aparece o menu estilo elevador com Play, Edit e Exit — o jogador navega, seleciona Play para entrar no fluxo de jogo e Exit para fechar; Edit exibe placeholder funcional
**Mode:** mvp
**Depends on**: Phase 1
**Requirements**: MENU-02, MENU-03, MENU-04, MENU-06
**Success Criteria** (what must be TRUE):
  1. Ao abrir o jogo, o menu principal estilo elevador é exibido com as opções Exit, Edit e Play navegáveis por teclado
  2. Selecionar Play inicia o fluxo de jogo (entra na cena de play); selecionar Exit encerra o processo corretamente
  3. Selecionar Edit exibe tela placeholder funcional sem crash (escopo completo v2)
  4. Navegação completa entre cenas (menu → jogo → vitória → menu) funciona sem crash ou vazamento de estado
**Plans**: TBD
**UI hint**: yes

### Phase 3: Profiles & Level Select
**Goal**: Após selecionar Play, o jogador cria ou escolhe um perfil local e vê a seleção de níveis como andares do elevador; progresso persiste entre sessões
**Mode:** mvp
**Depends on**: Phase 2
**Requirements**: USER-01, USER-02, USER-03, USER-04, MENU-05
**Success Criteria** (what must be TRUE):
  1. Jogador pode cadastrar um perfil local com nome e selecioná-lo em sessões futuras
  2. Tela de seleção de níveis é apresentada como andares do elevador — jogador navega e escolhe uma fase
  3. Ao completar uma fase, progresso do perfil (nível desbloqueado, melhor score) é salvo em arquivo local sem dependência de rede
  4. Ao reabrir o jogo e selecionar o mesmo perfil, o progresso anterior está disponível
**Plans**: TBD

### Phase 4: Opening Intro
**Goal**: Experiência completa e polida — intro cinematográfica roda antes do menu, transição suave, paleta CGA retro coerente em toda a interface
**Mode:** mvp
**Depends on**: Phase 3
**Requirements**: MENU-01
**Success Criteria** (what must be TRUE):
  1. Ao abrir o jogo, a animação de abertura cinematográfica exibe título e ambientação (≤5s), podendo ser pulada com qualquer tecla
  2. Intro transita automaticamente para o menu do elevador ao terminar, sem intervenção do jogador
  3. Toda a experiência visual — intro, menu, jogo — mantém estética retro coerente com paleta CGA
**Plans**: TBD
**UI hint**: yes

---

## Progress Table

| Phase | Plans Complete | Status | Completed |
|-------|----------------|--------|-----------|
| 1. 3D Foundation | 0/? | Not started | - |
| 2. Elevator Menu | 0/? | Not started | - |
| 3. Profiles & Level Select | 0/? | Not started | - |
| 4. Opening Intro | 0/? | Not started | - |

---

## Coverage

| Requirement | Phase | Category |
|-------------|-------|----------|
| GAME-01 | Phase 1 | Gameplay |
| GAME-02 | Phase 1 | Gameplay |
| GAME-03 | Phase 1 | Gameplay |
| GAME-04 | Phase 1 | Gameplay |
| GAME-05 | Phase 1 | Gameplay |
| VIS-01 | Phase 1 | Renderização 3D |
| VIS-02 | Phase 1 | Renderização 3D |
| VIS-03 | Phase 1 | Renderização 3D |
| VIS-04 | Phase 1 | Renderização 3D |
| VIS-05 | Phase 1 | Renderização 3D |
| PLAT-01 | Phase 1 | Plataforma |
| PLAT-02 | Phase 1 | Plataforma |
| PLAT-03 | Phase 1 | Plataforma |
| MENU-02 | Phase 2 | Menus |
| MENU-03 | Phase 2 | Menus |
| MENU-04 | Phase 2 | Menus |
| MENU-06 | Phase 2 | Menus |
| USER-01 | Phase 3 | Perfis |
| USER-02 | Phase 3 | Perfis |
| USER-03 | Phase 3 | Perfis |
| USER-04 | Phase 3 | Perfis |
| MENU-05 | Phase 3 | Menus |
| MENU-01 | Phase 4 | Menus |

**Coverage: 23/23 v1 requirements mapped ✓**

---
*Roadmap created: 2026-05-29*
*Last updated: 2026-05-29 after platform scope clarification (macOS + Linux)*
