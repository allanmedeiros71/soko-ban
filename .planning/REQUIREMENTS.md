# Requirements: Soko-ban — Primeira versão 3D

**Defined:** 2026-05-29
**Core Value:** O jogador sente que está jogando o Sokoban clássico — empurrar caixas até os alvos com estética retro e fluxo de menus do original, agora com presença 3D e navegação por elevador.

## v1 Requirements

### Gameplay Core (existente — validar após migração 3D)

- [ ] **GAME-01**: Jogador move-se nas 4 direções e empurra uma caixa por vez
- [ ] **GAME-02**: Jogador pode desfazer movimentos (undo)
- [ ] **GAME-03**: Jogador pode reiniciar a fase atual
- [ ] **GAME-04**: Vitória detectada quando todas as caixas estão nos alvos
- [ ] **GAME-05**: HUD exibe fase, movimentos, empurrões e tempo

### Renderização 3D

- [ ] **VIS-01**: Tabuleiro renderizado em 3D com câmera isométrica top-down inspirada no original
- [ ] **VIS-02**: Paredes exibem tijolos 3D visualmente fiéis ao estilo do original
- [ ] **VIS-03**: Caixotes renderizados em 3D (estilo engradado/dourado)
- [ ] **VIS-04**: Personagem renderizado em 3D (blocky, legível em top-down)
- [ ] **VIS-05**: Alvos, piso e paleta CGA retro preservados na estética 3D

### Menus e Fluxo (Elevador)

- [ ] **MENU-01**: Animação de abertura (intro) antes do menu principal
- [ ] **MENU-02**: Menu principal estilo elevador com opções Exit, Edit e Play
- [ ] **MENU-03**: Exit encerra o jogo; Play avança para seleção de usuário/nível
- [ ] **MENU-04**: Edit exibe placeholder funcional (tela mínima, escopo completo v2)
- [ ] **MENU-05**: Seleção de níveis apresentada como andares do elevador
- [ ] **MENU-06**: Navegação entre cenas (intro → menu → user → levels → play → win) funcional

### Perfis Locais

- [ ] **USER-01**: Jogador pode cadastrar perfil local com nome
- [ ] **USER-02**: Jogador pode selecionar perfil existente ao iniciar Play
- [ ] **USER-03**: Progresso por perfil persiste localmente (níveis desbloqueados ou melhor score)
- [ ] **USER-04**: Dados salvos em arquivo local sem dependência de rede

### Plataforma

- [ ] **PLAT-01**: Compila e executa no macOS (MacBook) via Make
- [ ] **PLAT-02**: Suporte HiDPI/Retina sem distorção de viewport OpenGL

## v2 Requirements

### Editor de Fases

- **EDIT-01**: Editor completo de fases acessível via opção Edit do menu elevador
- **EDIT-02**: Salvar/carregar fases customizadas em formato `.sok`

### Gameplay Avançado

- **GAME-06**: Melhor score (movimentos/tempo) por fase e perfil exibido na seleção
- **GAME-07**: Animação de caminhada do personagem 3D

### Conteúdo

- **CONT-01**: Conjunto expandido de fases além das 4 iniciais

## Out of Scope

| Feature | Reason |
|---------|--------|
| Contas online / backend | Decisão v1: perfis locais apenas |
| Recriação pixel-perfect frame-a-frame | Abordagem *inspired*, adaptada ao macOS |
| Mobile / iOS | Foco macOS desktop |
| Multiplayer / ranking online | Não solicitado |
| Solver / hints automáticos | Anti-feature — reduz puzzle |
| Deadlock detection | Complexidade desproporcional para v1 |
| Câmera rotacionável livre | Quebra estética top-down clássica |

## Traceability

| Requirement | Phase | Phase Name | Status |
|-------------|-------|------------|--------|
| GAME-01 | Phase 1 | 3D Foundation | Pending |
| GAME-02 | Phase 1 | 3D Foundation | Pending |
| GAME-03 | Phase 1 | 3D Foundation | Pending |
| GAME-04 | Phase 1 | 3D Foundation | Pending |
| GAME-05 | Phase 1 | 3D Foundation | Pending |
| VIS-01 | Phase 1 | 3D Foundation | Pending |
| VIS-02 | Phase 1 | 3D Foundation | Pending |
| VIS-03 | Phase 1 | 3D Foundation | Pending |
| VIS-04 | Phase 1 | 3D Foundation | Pending |
| VIS-05 | Phase 1 | 3D Foundation | Pending |
| PLAT-01 | Phase 1 | 3D Foundation | Pending |
| PLAT-02 | Phase 1 | 3D Foundation | Pending |
| MENU-02 | Phase 2 | Elevator Menu | Pending |
| MENU-03 | Phase 2 | Elevator Menu | Pending |
| MENU-04 | Phase 2 | Elevator Menu | Pending |
| MENU-06 | Phase 2 | Elevator Menu | Pending |
| USER-01 | Phase 3 | Profiles & Level Select | Pending |
| USER-02 | Phase 3 | Profiles & Level Select | Pending |
| USER-03 | Phase 3 | Profiles & Level Select | Pending |
| USER-04 | Phase 3 | Profiles & Level Select | Pending |
| MENU-05 | Phase 3 | Profiles & Level Select | Pending |
| MENU-01 | Phase 4 | Opening Intro | Pending |

**Coverage:**
- v1 requirements: 22 total
- Mapped to phases: 22
- Unmapped: 0 ✓

---
*Requirements defined: 2026-05-29*
*Last updated: 2026-05-29 after roadmap creation*
