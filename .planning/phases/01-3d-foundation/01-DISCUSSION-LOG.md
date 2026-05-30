# Phase 1: 3D Foundation - Discussion Log

> **Audit trail only.** Do not use as input to planning, research, or execution agents.
> Decisions are captured in CONTEXT.md — this log preserves the alternatives considered.

**Date:** 2026-05-29
**Phase:** 1-3D Foundation
**Areas discussed:** Estética dos tiles 3D, Feedback de movimento, Luz e cor, HUD na transição 3D

---

## Estética dos tiles 3D

| Option | Description | Selected |
|--------|-------------|----------|
| Tijolos individuais | Cada parede mostra blocos 3D separados como no vídeo | ✓ |
| Superfície com padrão de tijolo | Geometria simples + detalhe visual de tijolos | |
| Blocky estilizado | Formas 3D simples, espírito retro sem replicar cada tijolo | |

**User's choice:** Tijolos individuais

| Option | Description | Selected |
|--------|-------------|----------|
| Engradado detalhado | Tábuas, listras douradas/marrom como no original | ✓ |
| Caixote blocky | Cubo 3D dourado com linhas simples | |
| Cubo com faces coloridas | Volume mínimo, cores CGA dominam | |

**User's choice:** Engradado detalhado

| Option | Description | Selected |
|--------|-------------|----------|
| Humanóide blocky | Cabeça + corpo + pernas visíveis de cima | ✓ |
| Figura simples | Bloco principal + detalhe mínimo | |
| Cubo/personagem icônico | Forma geométrica pura | |

**User's choice:** Humanóide blocky

| Option | Description | Selected |
|--------|-------------|----------|
| Preservar marcadores 2D | Alvos como círculos/pontos no chão, piso plano | ✓ |
| Alvos 3D sutis | Disco/relevo leve no chão | |
| Alvos com brilho | Marcador luminoso no chão 3D | |

**User's choice:** Preservar marcadores 2D

---

## Feedback de movimento

| Option | Description | Selected |
|--------|-------------|----------|
| Snap instantâneo | Personagem salta de célula em célula (como 2D) | |
| Deslize suave | Interpolação curta (~100-150ms) entre células | ✓ |
| Você decide | Snap se complicar, slide se couber | |

**User's choice:** Deslize suave

| Option | Description | Selected |
|--------|-------------|----------|
| Caixa salta instantaneamente | Lógica e visual no mesmo frame | |
| Caixa desliza | Personagem e caixa movem juntos com animação | ✓ |
| Híbrido | Personagem snap, caixa desliza | |

**User's choice:** Caixa desliza

| Option | Description | Selected |
|--------|-------------|----------|
| Bloquear input durante animação | Evita movimentos inválidos visuais | ✓ |
| Enfileirar próximo movimento | Input responsivo, executa ao terminar | |
| Ignorar input durante animação | Simples, sem fila | |

**User's choice:** Bloquear input durante animação

| Option | Description | Selected |
|--------|-------------|----------|
| Undo instantâneo | Volta ao estado anterior sem animação | ✓ |
| Undo com animação reversa | Desfaz visualmente o último movimento | |
| Undo sempre instantâneo | Animação só para movimentos para frente | |

**User's choice:** Undo instantâneo

---

## Luz e cor

| Option | Description | Selected |
|--------|-------------|----------|
| Flat CGA puro | Cores sólidas da paleta, sem gradientes | |
| Shading sutil | Mesmas cores CGA, faces mais escuras/claras | ✓ |
| Luz ambiente leve | Tom uniforme com leve variação por face | |

**User's choice:** Shading sutil

| Option | Description | Selected |
|--------|-------------|----------|
| Tijolos magenta/ciano | Mesmas cores do 2D (C_DKMAG, C_CYAN) | |
| Tijolos avermelhados/marrom | Mais próximo do vídeo original | ✓ |
| Misto | Paredes realistas, personagem/caixas CGA puro | |

**User's choice:** Tijolos avermelhados/marrom

| Option | Description | Selected |
|--------|-------------|----------|
| Pontos brilhantes | Alvos em ciano/amarelo forte sobre piso escuro | ✓ |
| Piso tingido | Célula alvo com cor de fundo diferente | |
| Duplo marcador | Tint no piso + ponto central | |

**User's choice:** Pontos brilhantes

| Option | Description | Selected |
|--------|-------------|----------|
| Mudança de cor | Caixa fica verde/dourada quando no alvo | ✓ |
| Brilho no alvo | Caixa normal + alvo brilha por baixo | |
| Ambos | Cor da caixa muda E alvo brilha | |

**User's choice:** Mudança de cor

---

## HUD na transição 3D

| Option | Description | Selected |
|--------|-------------|----------|
| Overlay SDL2_ttf | HUD 2D sobre viewport OpenGL | |
| Texto em OpenGL | HUD integrado na pipeline 3D | |
| Híbrido | Stats em overlay 2D + barra semi-transparente | ✓ |

**User's choice:** Híbrido

| Option | Description | Selected |
|--------|-------------|----------|
| Barra superior | HUD fixo no topo (como layout 2D) | |
| Barra inferior | Stats na parte de baixo | ✓ |
| Canto compacto | Stats mínimos no canto | |

**User's choice:** Barra inferior

| Option | Description | Selected |
|--------|-------------|----------|
| Retro CGA | Fundo escuro, texto ciano/magenta | ✓ |
| Minimalista transparente | Texto direto sobre o jogo | |
| Borda pixel-art | Barra com borda estilo 8-bit | |

**User's choice:** Retro CGA

| Option | Description | Selected |
|--------|-------------|----------|
| Manter TITLE 2D | Phase 1 foca PLAY 3D | |
| TITLE simples em 3D | Mesmo render 3D com texto overlay | ✓ |
| Ir direto ao PLAY | Pular TITLE temporariamente | |

**User's choice:** TITLE simples em 3D

---

## Claude's Discretion

Nenhuma área delegada explicitamente ao Claude durante a discussão.

## Deferred Ideas

Nenhuma — discussão permaneceu dentro do escopo da Phase 1.
