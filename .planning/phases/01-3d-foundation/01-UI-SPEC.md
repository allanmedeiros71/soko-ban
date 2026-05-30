---
phase: 1
slug: 3d-foundation
status: approved
shadcn_initialized: false
preset: none
created: 2026-05-29
reviewed_at: 2026-05-29
---

# Phase 1 — UI Design Contract

> Contrato visual e de interação para a migração 3D isométrica. Gerado a partir de CONTEXT.md (D-01–D-16), REQUIREMENTS.md (VIS-*, GAME-*, PLAT-*) e paleta existente em `sokoban.c`.

---

## Design System

| Property | Value |
|----------|-------|
| Tool | none (native C game — não aplica shadcn) |
| Preset | not applicable |
| Render pipeline | SDL2 window + OpenGL 3.3 core + SDL2_ttf overlay |
| Component library | none — primitivas OpenGL + helpers `draw_*_3d` |
| Icon library | none — símbolos geométricos 3D e marcadores 2D no chão |
| Font | `RobotoMono-Bold.ttf` em `assets/fonts/`; fallback mono SO (macOS/Linux) |

**Nota:** HUD e overlays de cena usam SDL2_ttf sobre viewport OpenGL (D-13). Tabuleiro 3D ocupa área central superior; barra inferior reservada ao HUD (D-14).

---

## Spacing Scale

Valores declarados (múltiplos de 4):

| Token | Value | Usage |
|-------|-------|-------|
| xs | 4px | Margem interna de blocos 3D, gap entre tijolos |
| sm | 8px | Padding interno de overlay de vitória, respiro entre linhas de texto |
| md | 16px | Padding lateral mínimo do HUD |
| lg | 24px | Margem entre tabuleiro e bordas da viewport |
| xl | 32px | Espaçamento vertical entre blocos de texto na cena TITLE |
| 2xl | 48px | Respiro entre título e subtítulo na cena TITLE |
| 3xl | 64px | Offset vertical do título principal na cena TITLE |

**Layout fixo (Phase 1):**

| Elemento | Valor | Fonte |
|----------|-------|-------|
| HUD height | 40px | `HUD` em `sokoban.c` — preservar |
| HUD text inset | 10px | padding esquerdo + vertical do texto HUD |
| HUD top border | 2px | linha magenta separando tabuleiro e barra |
| Tile lógico | 28px (referência 2D) | base para escala isométrica; célula 3D ocupa mesma grade lógica |
| Win overlay box | 460×140px | centralizado sobre tabuleiro |
| Win overlay border | 2px | contorno dourado |

Exceptions: altura do HUD (40px) e tile lógico (28px) são legados intencionais — não arredondar para preservar proporção do layout existente.

---

## Typography

| Role | Size | Weight | Line Height | Usage |
|------|------|--------|-------------|-------|
| Body | 18px | Bold (700) | 1.5 | HUD stats, instruções, subtítulos de cena |
| Label | 18px | Bold (700) | 1.5 | Mesmo slot que Body — mono bold único |
| Heading | 40px | Bold (700) | 1.2 | Título "SOKO-BAN", "FASE COMPLETA!" |
| Display | 40px | Bold (700) | 1.2 | Mesmo slot que Heading — máx. 2 tamanhos distintos |

**Regras:**
- Apenas **2 tamanhos** (18px, 40px) e **1 peso** (Bold) — fonte mono retro CGA/DOS.
- Texto HUD: alinhado à esquerda, cor `C_CYAN`.
- CTAs e títulos de cena: centralizados horizontalmente (`draw_text_c`).
- Sem itálico, sem anti-aliasing custom além do TTF default.

---

## Color

Paleta base herdada de `sokoban.c` — valores RGB exatos:

| Token | RGB | Hex |
|-------|-----|-----|
| C_BLACK | 12, 10, 24 | `#0C0A18` |
| C_CYAN | 40, 220, 220 | `#28DCDC` |
| C_MAGENTA | 230, 60, 200 | `#E63CC8` |
| C_WHITE | 240, 240, 255 | `#F0F0FF` |
| C_DKMAG | 120, 20, 110 | `#78146E` |
| C_GOLD | 245, 200, 60 | `#F5C83C` |
| C_DKGOLD | 170, 120, 20 | `#AA7814` |
| C_GREEN | 80, 230, 120 | `#50E678` |
| C_DKGRN | 30, 140, 70 | `#1E8C46` |
| C_BRICK | 180, 80, 50 | `#B45032` (novo — paredes, D-10) |
| C_DKBRICK | 120, 45, 30 | `#782D1E` (novo — sombreamento tijolo) |

**Distribuição 60/30/10:**

| Role | % | Token(s) | Usage |
|------|---|----------|-------|
| Dominant (60%) | 60% | C_BLACK, piso escuro | Fundo da janela, chão do tabuleiro, viewport 3D |
| Secondary (30%) | 30% | C_DKMAG, C_DKBRICK, C_DKGOLD, C_DKGRN | Barra HUD, sombreamento de volumes 3D, caixotes/paredes |
| Accent (10%) | 10% | C_CYAN, C_MAGENTA, C_GOLD, C_GREEN | Destaques pontuais — ver lista abaixo |
| Destructive | — | C_MAGENTA | Não há ações destrutivas na UI Phase 1; reservado para confirmações futuras |

**Accent reservado exclusivamente para:**
- Marcadores de alvo no chão (ciano brilhante, D-11)
- CTAs primários: `[ ENTER para jogar ]`, `[ ENTER para a proxima fase ]` (C_GOLD)
- Título principal "SOKO-BAN" (C_MAGENTA)
- Linha separadora superior do HUD (C_MAGENTA, 2px)
- Texto de stats no HUD (C_CYAN)
- Caixa posicionada no alvo — topo verde `C_GREEN` / sombra `C_DKGRN` (D-12, equivalente BOXG)
- Mensagem de vitória "FASE COMPLETA!" (C_GREEN)

**Não usar accent em:** paredes, piso comum, corpo do personagem, caixas fora do alvo, bordas decorativas genéricas.

---

## Copywriting Contract

| Element | Copy | Cor | Cena |
|---------|------|-----|------|
| Título principal | `SOKO-BAN` | C_MAGENTA | SC_TITLE |
| Subtítulo | `edicao nostalgica - estilo CGA 1984` | C_CYAN | SC_TITLE |
| Instruções linha 1 | `Setas/WASD: mover    U: desfazer    R: reiniciar` | C_WHITE | SC_TITLE |
| Instruções linha 2 | `N/P: trocar fase     ESC: sair` | C_WHITE | SC_TITLE |
| Primary CTA | `[ ENTER para jogar ]` | C_GOLD | SC_TITLE |
| HUD stats | `FASE %d/%d   movimentos: %04d   empurroes: %04d   tempo: %u:%02u:%02u` | C_CYAN | SC_PLAY, SC_WIN |
| Win heading | `FASE COMPLETA!` | C_GREEN | SC_WIN |
| Win CTA | `[ ENTER para a proxima fase ]` | C_WHITE | SC_WIN |
| Empty state | N/A — tabuleiro sempre tem conteúdo em Phase 1 | — | — |
| Error state (fonte) | stderr: `Nenhuma fonte encontrada.` + exit 1 | — | init |
| Error state (SDL/GL) | stderr: mensagem SDL/GL + exit 1 | — | init |
| Destructive confirmation | N/A — Phase 1 não tem ações destrutivas na UI | — | — |

**Regras de copy:**
- Manter português sem acentos nos strings de runtime (compatibilidade ASCII existente).
- CTAs entre colchetes `[ ]` — padrão retro DOS.
- HUD usa zero-padding em movimentos/empurrões (`%04d`).

---

## Registry Safety

| Registry | Blocks Used | Safety Gate |
|----------|-------------|-------------|
| shadcn official | none | not applicable |
| third-party | none | not applicable |

Phase 1 não usa component libraries web. Dimensão 6: PASS por ausência de registries.

---

## 3D Visual Contract

> Seção específica do jogo — complementa o template web. Fonte: CONTEXT.md D-01–D-04, D-09–D-12.

### Câmera e layout

| Property | Value |
|----------|-------|
| Projeção | Isométrica top-down fixa (VIS-01) |
| Rotação | Nenhuma — câmera estática (Out of Scope) |
| Depth sort | Painter's Algorithm por col+row (STATE.md) |
| Viewport | Área acima do HUD; centralizada; `SDL_GL_GetDrawableSize` para HiDPI |
| Focal point | Tabuleiro 3D isométrico — ocupa ~70% da área visual acima do HUD |

### Tiles 3D

| Tile | Visual | Decisão |
|------|--------|---------|
| Parede (`#`) | Tijolos 3D individuais visíveis, tom avermelhado/marrom (C_BRICK/C_DKBRICK) | D-01, D-10 |
| Piso (` `) | Plano escuro com grid sutil; sem relevo | D-04 |
| Alvo (`.`) | Marcador 2D no chão — círculo/ponto ciano brilhante | D-04, D-11 |
| Caixa (`$`) | Engradado 3D — tábuas + listras douradas/marrom | D-02 |
| Caixa no alvo (`*`) | Mesmo engradado; topo verde/dourado (C_GREEN/C_GOLD) | D-12 |
| Jogador (`@`/`+`) | Humanóide blocky — cabeça branca, corpo magenta, pernas ciano | D-03 |

### Shading (D-09)

- Faces superiores: tom base da paleta.
- Faces laterais/frontais: tom escuro correspondente (`C_DK*`).
- Sem sombras projetadas, sem iluminação dinâmica.
- Objetivo: ler volume 3D mantendo espírito CGA.

### Cena TITLE (D-16)

- Mesmo pipeline OpenGL 3D (tabuleiro de demo ou fundo 3D simples).
- Texto overlay via SDL2_ttf sobre GL — mesmas posições e copy da implementação 2D atual.
- Consistência visual imediata; menus elevador completos ficam para Phase 2.

---

## Interaction Contract

> Fonte: CONTEXT.md D-05–D-08, GAME-*.

| Interação | Comportamento | Duração |
|-----------|---------------|---------|
| Movimento jogador | Deslize suave entre células | 100–150 ms (D-05) |
| Empurrar caixa | Jogador + caixa deslizam juntos | 100–150 ms (D-06) |
| Input durante animação | Bloqueado — ignorar teclas até fim | D-07 |
| Undo (U / Backspace) | Instantâneo — sem animação reversa | D-08 |
| Reiniciar (R) | Instantâneo — recarrega fase | GAME-03 |
| Vitória | Overlay SC_WIN sobre tabuleiro congelado | — |
| Próxima fase (ENTER em SC_WIN) | Avança level_index, recarrega | — |

**Teclas preservadas da v2D:** Setas/WASD, U, Backspace, R, N, P, ENTER, SPACE (title), ESC, Q.

---

## Scene Hierarchy

```
┌─────────────────────────────────────────┐
│  Viewport 3D (60% visual weight)        │
│  ┌─────────────────────────────────┐    │
│  │  Tabuleiro isométrico centrado  │    │  ← Focal point
│  │  (paredes > caixas > jogador)   │    │
│  └─────────────────────────────────┘    │
│  [Overlay SC_WIN: caixa centralizada]   │
├─────────────────────────────────────────┤
│  HUD bar 40px — stats C_CYAN (30%)      │  ← Secondary band
└─────────────────────────────────────────┘
```

**Prioridade visual (olho do jogador):**
1. Personagem blocky (movimento chama atenção)
2. Caixas e alvos brilhantes (objetivo do puzzle)
3. Paredes de tijolo (contexto/moldura)
4. HUD stats (referência periférica)

---

## Checker Sign-Off

- [x] Dimension 1 Copywriting: PASS
- [x] Dimension 2 Visuals: PASS
- [x] Dimension 3 Color: PASS
- [x] Dimension 4 Typography: PASS
- [x] Dimension 5 Spacing: PASS
- [x] Dimension 6 Registry Safety: PASS

**Approval:** approved 2026-05-29
