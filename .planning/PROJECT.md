# Soko-ban — Primeira versão (releitura 3D)

## What This Is

Releitura nostálgica do Sokoban original (Spectrum HoloByte, 1984), partindo de
uma implementação C/SDL2 funcional e evoluindo para uma experiência **inspirada** no
original: menus em formato de elevador, animação de abertura, cadastro local de
usuários, seleção de níveis como andares, e visual **3D** para paredes (tijolos),
caixotes e personagem. Referência visual: [vídeo YouTube](https://www.youtube.com/watch?v=Rjw_q9WH1Ao).

## Core Value

O jogador sente que está jogando o Sokoban clássico — empurrar caixas até os alvos
com a estética retro e fluxo de menus do original — agora com presença 3D e
navegação por elevador.

## Requirements

### Validated

- ✓ Gameplay Sokoban funcional (movimento, empurrar caixa, desfazer, reiniciar) — existing
- ✓ 4 fases hardcoded em formato `.sok` — existing
- ✓ Renderização 2D estilo CGA (paleta ciano/magenta, HUD) — existing
- ✓ Controles por teclado (setas/WASD, undo, troca de fase) — existing
- ✓ Build multiplataforma Linux + macOS via Make — existing
- ✓ Fonte embarcada + fallback sistema — existing

### Active

- [ ] Animação de abertura do jogo (intro cinematográfica inspirada no original)
- [ ] Menu principal estilo elevador com opções Exit, Edit, Play
- [ ] Tela de cadastro/seleção de usuário local (perfis salvos em arquivo)
- [ ] Tela de seleção de níveis como andares do elevador
- [ ] Paredes 3D com tijolos visualmente fiéis ao original
- [ ] Caixotes 3D visualmente fiéis ao original
- [ ] Personagem 3D
- [ ] Migração de renderização 2D → 3D mantendo lógica de tabuleiro existente

### Out of Scope

- Contas online / backend de autenticação — v1 usa perfis locais apenas
- Recriação pixel-perfect frame-a-frame do original — abordagem *inspired*, adaptada ao macOS moderno
- Suporte mobile/iOS — foco macOS desktop (base já compilável no MacBook)
- Multiplayer / ranking online — não solicitado
- Editor de fases completo (Edit) — escopo a detalhar na Fase de menus; MVP pode ser placeholder

## Context

**Brownfield:** codebase existente em `sokoban.c` (~420 linhas), single-file C com SDL2/SDL2_ttf.
Mapa de codebase em `.planning/codebase/`. Loop event-poll → update → render com cenas
TITLE / PLAY / WIN. Tabuleiro codificado em grid ASCII.

**Referência:** `descricao.md` descreve features visuais e de UX do Sokoban original 1984.
Fidelidade *inspired* — mesma mecânica e espírito retro, UX adaptada.

**Plataforma alvo:** macOS (MacBook) como ambiente principal de desenvolvimento e execução;
Linux permanece suportado via Make.

## Constraints

- **Tech**: Manter C como linguagem principal; evoluir stack para 3D via SDL2 + OpenGL (decisão recomendada para brownfield)
- **Compatibilidade**: macOS desktop como target primário
- **Arquitetura**: Preservar lógica de tabuleiro/movement/undo existente; substituir camada de render
- **Perfis**: Armazenamento local (JSON ou similar), sem dependência de rede
- **Escopo**: Granularidade coarse (3–5 fases), modo MVP vertical

## Key Decisions

| Decision | Rationale | Outcome |
|----------|-----------|---------|
| Perfis locais (sem backend) | Simplicidade v1, offline-first | — Pending |
| Fidelidade *inspired* (não pixel-perfect) | Adaptar ao macOS moderno mantendo espírito 1984 | — Pending |
| Stack 3D: C + SDL2 + OpenGL | Brownfield: reutiliza base C/SDL2/macOS; raylib exigiria migração maior; Godot reescreveria tudo | — Pending |
| Modo MVP vertical | Entregar fatias jogáveis end-to-end por fase | — Pending |
| Modo YOLO | Auto-aprovar artefatos de planejamento | — Pending |

## Evolution

This document evolves at phase transitions and milestone boundaries.

**After each phase transition** (via `/gsd-transition`):
1. Requirements invalidated? → Move to Out of Scope with reason
2. Requirements validated? → Move to Validated with phase reference
3. New requirements emerged? → Add to Active
4. Decisions to log? → Add to Key Decisions
5. "What This Is" still accurate? → Update if drifted

**After each milestone** (via `/gsd-complete-milestone`):
1. Full review of all sections
2. Core Value check — still the right priority?
3. Audit Out of Scope — reasons still valid?
4. Update Context with current state

---
*Last updated: 2026-05-29 after initialization*
