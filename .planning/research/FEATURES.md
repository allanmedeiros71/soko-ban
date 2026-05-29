# Feature Landscape — Soko-ban 3D Retro Remake

**Domínio:** Puzzle retro remake (Sokoban, 1984 Spectrum HoloByte)
**Pesquisado:** 2026-05-29
**Contexto:** Brownfield C/SDL2 com gameplay funcional. Adicionando intro, menus estilo elevador, perfis locais, seleção de níveis, e visuais 3D.

---

## Table Stakes

Features que o jogador espera. Ausência = produto incompleto ou quebrado.

| Feature | Por que é esperada | Complexidade | Notas |
|---------|-------------------|--------------|-------|
| **Undo ilimitado** | Deadlocks são inevitáveis; sem undo o jogo é injogável | Baixa | **Já existe** na codebase atual (pilha de histórico) |
| **Reiniciar nível** | Fallback quando o jogador quer começar do zero | Baixa | **Já existe** |
| **Condição de vitória** | Detectar quando todos os caixotes estão nos alvos | Baixa | **Já existe** |
| **Navegação entre níveis** | Avançar e voltar entre fases ao terminar ou escolher | Baixa | **Já existe** parcialmente (troca de fase por tecla) |
| **Controles responsivos** | Setas/WASD com feedback imediato de 1 tile por input | Baixa | **Já existe** |
| **Menu principal** | Tela inicial antes do jogo (mínimo: Play, Exit) | Média | **A implementar** — formato elevador é o diferencial |
| **Seleção de nível** | Escolher qual fase jogar sem depender de sequência | Média | **A implementar** — como andares do elevador |
| **Tela de vitória** | Feedback ao completar uma fase | Baixa | **Já existe** (WIN scene) |
| **Animação de abertura** | Intro antes do menu — contextualiza o jogo | Média | **A implementar** — cinematográfica, inspirada no original |
| **Suporte a teclado completo** | Atalhos para undo, restart, menu, quit | Baixa | **Já existe** base; estender para novos estados |
| **Paredes visualmente legíveis** | Player distingue paredes de chão e caixotes sem hesitar | Média | **A implementar** — tijolos 3D fiéis ao original 1984 |
| **Caixotes visualmente distintos** | Diferença visual clara entre caixote normal e caixote-no-alvo | Média | **A implementar** — 3D com marcação de estado |
| **Player reconhecível** | Avatar que o jogador identifica como protagonista | Média | **A implementar** — personagem 3D |

---

## Differentiators

Features que diferenciam este projeto. Não obrigatórias, mas que entregam o valor central da *releitura* retro.

| Feature | Proposta de valor | Complexidade | Notas |
|---------|-------------------|--------------|-------|
| **Menu elevador animado** | Reproduz a metáfora icônica do original 1984: opções aparecem como andares de um elevador | Alta | Núcleo da identidade visual do projeto; Exit / Edit / Play como andares |
| **Intro cinematográfica** | Gancham o jogador antes do menu, estabelecem o tom retro | Média-Alta | Animada, inspirada — não pixel-perfect; pode usar sequência de cenas timed |
| **Perfis locais de usuário** | Cada jogador tem nome salvo, progresso independente | Média | Arquivo JSON local; sem backend; tela de cadastro/seleção antes do Play |
| **Seleção de nível como andares** | Níveis apresentados como andares de prédio no elevador — coerência visual com o menu | Alta | Estende a metáfora do elevador para toda a experiência de seleção |
| **Visuais 3D mantendo perspectiva top-down** | 3D nos sprites/tiles (paredes com tijolos em relevo, caixotes com profundidade) sem mudar a câmera aérea | Alta | Diferencial visual do remake vs. clones 2D genéricos |
| **Paleta CGA retro no contexto 3D** | Manter o espírito da paleta ciano/magenta nos objetos 3D | Média | Fidelidade estética sem pixel-perfect — "inspired" não "recreated" |
| **Contador de movimentos e pushes** | Exibir movimentos e empurrões no HUD — dado histórico do original | Baixa | Original Soko-Ban 1988 usava moves + pushes para scoring; adiciona desafio pessoal |
| **Melhor score por nível** | Registrar e exibir o melhor resultado por fase, por perfil | Média | Incentiva rejogo mesmo após completar |

---

## Anti-features

Features a **não** construir nesta versão.

| Anti-feature | Por que evitar | O que fazer em vez disso |
|--------------|---------------|--------------------------|
| **Recriação pixel-perfect** | Requer assets exatos do original (licença, fidelidade framerate, paleta exata); scope impossível de validar | Fidelidade *inspired*: mesmo espírito, assets próprios adaptados |
| **Backend de autenticação / contas online** | Dependência de rede, complexidade operacional, fora do escopo MVP | Perfis locais em arquivo JSON |
| **Multiplayer / ranking online** | Original tinha competição 1-4 jogadores, mas exige sessão compartilhada ou servidor | Fora do escopo v1; registrar localmente o melhor score por perfil |
| **Editor de fases completo (Edit mode)** | Alta complexidade (UI de grade editável, validação, persistência) para v1 | Placeholder no menu elevador; o item "Edit" pode ser um stub desabilitado ou minimal |
| **Detecção automática de deadlock** | Algoritmo não-trivial (NP-hard); distorce o espírito de puzzle desafiador | Undo generoso resolve o mesmo problema de forma mais simples |
| **Hint / solução automática** | Solver para Sokoban é computacionalmente caro e remove a essência do puzzle | Deixar o jogador resolver; undo suficiente como safety net |
| **Suporte mobile / touch** | Fora do target (macOS desktop/MacBook) | Controle por teclado; sem swipe/tap |
| **IA de level generation** | Gerar puzzles válidos é tema de pesquisa acadêmica; qualidade difícil de garantir | Usar os 4 níveis existentes + possibilidade de adicionar manualmente mais arquivos `.sok` |
| **Câmera rotacionável / primeira pessoa** | Quebra a coerência com o original (top-down); aumenta escopo 3D drasticamente | 3D nos tiles/sprites com câmera fixa top-down ou leve perspectiva isométrica |
| **Animações de movimento suaves (tweening)** | Desejável, mas adiciona estado de interpolação entre frames ao loop de jogo; complexidade não-trivial em C/SDL2 | Movimento snap-to-grid (1 tile por frame de input) — autêntico ao original |

---

## Notas de Complexidade

### O que parece simples mas é trápico

| Feature | Complexidade real | Armadilha |
|---------|------------------|-----------|
| Animação de abertura | Média-Alta | Timing frame-a-frame em C sem engine de animação; precisa de estado próprio de cena e scheduler de eventos temporais |
| Elevador animado | Alta | Não é só mudar texto — o elevador precisa *parecer* se mover (scroll vertical de opções, efeito de porta abrindo/fechando) |
| Seleção de nível como andares | Alta | Integrar com perfis de usuário (qual fase está desbloqueada?) e com o elevador (scroll para o andar N) |
| 3D nos tiles com SDL2 | Alta | SDL2 puro não tem 3D nativo; requer ou OpenGL (projeção perspectiva, shaders) ou sprites pré-renderizados isométricos; a decisão de stack impacta diretamente esta feature |
| Persistência de perfis | Média | Sem biblioteca JSON em C padrão; requer escolha de lib (cJSON, jansson) ou formato próprio simples |

### O que parece difícil mas é direto

| Feature | Complexidade real | Razão |
|---------|------------------|-------|
| Contador moves/pushes | Baixa | Incrementar dois contadores no `update()` e exibir no HUD existente |
| Tela de seleção de usuário | Baixa-Média | Lista estática de perfis do arquivo; entrada de teclado para nome; sem autenticação |
| Melhor score por perfil | Baixa | Persistir junto com o arquivo de perfil; comparar a cada vitória |

---

## Dependências entre Features

```
Animação de abertura
    └─→ Menu elevador (Enter)
            ├─→ Tela de cadastro/seleção de usuário (Play)
            │       └─→ Seleção de nível como andares (perfil carregado)
            │               └─→ Gameplay 3D (nível selecionado)
            │                       ├─→ Undo / restart (já existe)
            │                       ├─→ HUD com moves + pushes
            │                       └─→ Tela de vitória → melhor score salvo no perfil
            ├─→ Editor (Edit) [placeholder v1]
            └─→ Sair (Exit)

Visuais 3D
    └─→ Decisão de stack (SDL2+OpenGL ou sprites isométricos)
            ├─→ Paredes 3D (tijolos)
            ├─→ Caixotes 3D (normal + em-alvo)
            └─→ Personagem 3D

Persistência de perfis
    ├─→ Seleção de usuário (lê/escreve perfis)
    ├─→ Seleção de nível (lê progresso por perfil)
    └─→ Melhor score (escreve pós-vitória)
```

---

## MVP Recommendation

Priorizar nesta ordem:

1. **Visuais 3D** — Paredes, caixotes e personagem 3D (núcleo da diferenciação visual; desbloqueia tudo visual)
2. **Animação de abertura** — Intro cinematográfica (define o tom retro antes de qualquer menu)
3. **Menu elevador** — Exit / Edit / Play como andares (identidade central do projeto)
4. **Perfis locais** — Cadastro e seleção de usuário (habilita personalização e progresso)
5. **Seleção de nível como andares** — Completar a metáfora do elevador (coerência UX)
6. **HUD de moves + pushes** — Contador no gameplay (baixo esforço, alto valor retro)
7. **Melhor score por perfil** — Motivação de rejogo

**Diferir:**
- Edit mode completo → stub/placeholder suficiente para v1
- Tweening de movimento → snap-to-grid é mais fiel ao original
- Deadlock detection → undo generoso resolve o mesmo problema

---

## Fontes

- GameFAQs Walkthrough Soko-Ban (odino): https://gamefaqs.gamespot.com/pc/935300-soko-ban/faqs/61200 — MEDIUM confidence (secondary source; documenta UX original)
- Spectrum HoloByte Soko-Ban (1988) packaging description via EliSoftware: http://www.elisoftware.org/w/index.php?title=Soko-Ban — MEDIUM confidence (marketing copy do original)
- Wikipedia Sokoban: https://en.wikipedia.org/wiki/Sokoban — HIGH confidence (histórico e regras canônicas)
- Internet Archive Soko-Ban: https://archive.org/details/soko-ban_1984 — HIGH confidence (fonte primária do jogo)
- SOKO clone (DEV.to): https://dev.to/novikorisnik/soko-another-sokoban-clone-1mda — LOW confidence (impl. moderna, não-oficial)
- sudokatie/crates (GitHub): https://github.com/sudokatie/crates — LOW confidence (clone moderno; feature list útil)
- GameAnalytics — Immersive Game Intros: https://www.gameanalytics.com/blog/create-immersive-game-intros — LOW confidence (design advice geral)
- Game Developer — HD Remake the Right Way: https://www.gamedeveloper.com/design/doing-an-hd-remake-the-right-way — LOW confidence (orientação geral para remakes)
