---
gsd_state_version: 1.0
milestone: v1.0
milestone_name: milestone
status: Not started
last_updated: "2026-05-30T00:12:21.704Z"
progress:
  total_phases: 4
  completed_phases: 0
  total_plans: 0
  completed_plans: 0
  percent: 0
---

# State: Soko-ban — Primeira versão 3D

**Last Updated:** 2026-05-29
**Session:** Initialization — research + requirements + roadmap

---

## Project Reference

**Core Value:** O jogador sente que está jogando o Sokoban clássico — empurrar caixas até os alvos com estética retro e fluxo de menus do original, agora com presença 3D e navegação por elevador.
**Milestone:** v1 — Releitura 3D Retro
**Mode:** MVP Vertical (cada fase entrega fatia jogável end-to-end)

---

## Current Position

**Phase:** 1 — 3D Foundation
**Plan:** TBD (not yet planned)
**Status:** Not started
**Progress:** ░░░░░░░░░░ 0% (0/4 phases complete)

---

## Performance Metrics

| Metric | Value |
|--------|-------|
| Phases complete | 0/4 |
| Requirements mapped | 23/23 |
| Plans written | 0 |
| Plans executed | 0 |

---

## Accumulated Context

### Key Decisions

| Decision | Rationale | Phase |
|----------|-----------|-------|
| Stack: C + SDL2 + OpenGL 3.3 | Brownfield — reutiliza base C/SDL2; sem custo de migração; GLAD 2 + cglm + cJSON como deps single-file | Pre-phase |
| Perfis locais (sem backend) | Simplicidade v1; offline-first; sem dependência de rede | Pre-phase |
| Fidelidade *inspired* (não pixel-perfect) | Adaptar a desktops modernos mantendo espírito 1984 | Pre-phase |
| Plataformas v1: macOS + Linux | Ambos os SOs são targets de primeira classe; verificar build em cada fase | Pre-phase |
| Modo MVP vertical | Entregar fatias jogáveis end-to-end por fase; nunca deixar o jogo não-jogável | Pre-phase |
| Granularidade coarse (4 fases) | Solo dev + Claude; fases grandes e coesas > muitas fases pequenas | Pre-phase |
| Painter's Algorithm para depth | Correto para isométrico 2.5D; evita z-fighting sem depth buffer complexo | Pre-phase |
| Intro na Phase 4 (não Phase 2) | Priorizar fluxo funcional completo antes de polish cinematográfico; evita scope creep | Pre-phase |
| Editor (Edit) como placeholder em v1 | Escopo completo v2; MVP não bloqueia em feature não-essencial | Pre-phase |

### Todos

- [ ] Verificar integração SDL2 + OpenGL no macOS Sequoia (arm64) e Linux — fazer no primeiro commit de Phase 1
- [ ] Extrair `game_logic.c` como primeiro commit de Phase 1, verificar jogo ainda roda antes de tocar OpenGL
- [ ] Definir estratégia Painter's Algorithm (depth sort por col+row) antes de iniciar renderização 3D
- [ ] Time-box animação do elevador em Phase 2: placeholder funcional primeiro; animação por cima como polish
- [ ] `.gitignore` com `sokoban` e `*.o` antes do primeiro commit

### Blockers

None

### Warnings

- **Dual platform:** Cada fase deve passar `make` em macOS e Linux antes de considerar done
- **macOS OpenGL cap 4.1:** Usar `#version 330 core`; testar no macOS no primeiro commit com GL, não depois
- **Linux OpenGL:** Linkar `-lGL`; shaders devem usar mesma versão 330 core que macOS
- **HiDPI/Retina:** Usar `SDL_GL_GetDrawableSize` para viewport; nunca hardcodar dimensões de janela
- **Scope creep:** Intro e animação do elevador podem consumir todo o tempo — critério de done não pode depender de animação perfeita
- **Big-bang refactoring:** Nunca extrair `game_logic` e adicionar 3D no mesmo commit; uma mudança por vez, jogo sempre jogável

---

## Session Continuity

**Last session:** 2026-05-30T00:12:21.699Z
**Work done:** Pesquisa de stack/features/arquitetura/pitfalls; definição de requirements (22 v1); criação de ROADMAP.md e STATE.md
**Next action:** `/gsd-plan-phase 1` — planejar Phase 1: 3D Foundation

---
*State initialized: 2026-05-29*
