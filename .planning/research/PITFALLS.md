# Domain Pitfalls

**Domain:** Retro game remake — brownfield C/SDL2 → 3D, macOS, single-file migration
**Researched:** 2026-05-29
**Sources:** SDL2 official docs, Khronos forums, GameDev.net, game remake post-mortems

---

## Armadilhas Críticas

Erros que causam reescrita, crashes não reproduzíveis, ou bloqueio total do desenvolvimento.

---

### Pitfall 1: macOS OpenGL preso em 4.1 (sem avisos)

**O que dá errado:** O projeto requer features de OpenGL moderno (UBOs, instanced draw,
geometry shaders de GL 4.3+), mas o macOS está fixado em OpenGL 4.1 desde 2011. Shaders
compilam em Linux, crasham silenciosamente em macOS. A mensagem de erro é genérica.

**Por que acontece:** Apple deprecou OpenGL em 10.14 (Mojave) e congelou a implementação
em 4.1 muitos anos antes disso. Não há planos públicos de remoção imediata, mas novas
features não chegam. Desenvolvedores que testam primeiro em Linux ou Windows recebem uma
falsa sensação de segurança.

**Consequências:** Shaders que usam `layout(binding=...)` ou UBOs com `std430`
funcionam perfeitamente em Linux e quebram sem mensagem clara no macOS. Horas de debug.

**Sinais de alerta:**
- Tela preta sem erro de GL no macOS, mas funciona em Linux
- `glGetIntegerv(GL_MAJOR_VERSION)` retorna 4, `GL_MINOR_VERSION` retorna 1 no macOS
- Uso de `#version 430 core` ou superior nos shaders
- `GL_ARB_compute_shader`, geometry shaders, `glDispatchCompute` — não existem em 4.1

**Prevenção:**
- Usar `#version 410 core` ou `#version 330 core` em todos os shaders; nunca acima de 410 no macOS
- Testar no macOS logo na primeira compilação com OpenGL, não depois
- Pedir contexto explicitamente: `SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3)` +
  `SDL_GL_CONTEXT_MINOR_VERSION, 3` + `SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE`
- Alternativa: usar somente a API do SDL2 Renderer (sem OpenGL manual) para o caso 2.5D — evita o problema completamente

**Fase que deve tratar:** Fase de renderização 3D (primeira que adiciona OpenGL)

---

### Pitfall 2: HiDPI/Retina — DrawableSize ≠ WindowSize

**O que dá errado:** Em telas Retina, `SDL_GetWindowSize` retorna dimensões lógicas (ex.:
896×712), mas `SDL_GL_GetDrawableSize` retorna o dobro (1792×1424). Usar as dimensões
erradas para `glViewport` ou para conversão de coordenadas de tela → tile faz o rendering
aparecer em apenas ¼ da tela, ou o mouse-click errar o tile por fator 2.

**Por que acontece:** SDL2 distingue "logical size" de "drawable size" desde o suporte
Retina no macOS. O código atual usa `MAXW * TILE` como largura fixa sem consultar SDL,
o que funcionava em 2D (SDL renderer escala automaticamente) mas não funciona com OpenGL
manual.

**Consequências:** Rendering comprimido no canto superior esquerdo, UI de elevador
parecendo minúscula, clique de mouse errado por fator 2×.

**Sinais de alerta:**
- `SDL_GetWindowSize` e `SDL_GL_GetDrawableSize` retornam valores diferentes
- Rendering correto em monitor 1× mas quebrado em Retina
- Coordenadas de clique deslocadas no macOS mas corretas em Linux

**Prevenção:**
```c
int draw_w, draw_h;
SDL_GL_GetDrawableSize(window, &draw_w, &draw_h);
glViewport(0, 0, draw_w, draw_h);

// Para conversão mouse → tile, usar logical size:
int win_w, win_h;
SDL_GetWindowSize(window, &win_w, &win_h);
float scale_x = (float)draw_w / win_w;  // ~2.0 em Retina
```
- Nunca usar constantes hardcoded de dimensão de janela para `glViewport`
- Testar em Retina Mac logo na primeira integração OpenGL

**Fase que deve tratar:** Fase de renderização 3D (setup inicial do contexto)

---

### Pitfall 3: NSOpenGLContext em thread não-principal → crash no macOS 10.15+

**O que dá errado:** Chamar `SDL_GL_MakeCurrent` em qualquer thread que não seja a thread
principal causa um crash com `EXC_BAD_INSTRUCTION` (illegal instruction) no macOS Catalina+.
O erro não ocorre no Linux nem em versões mais antigas do macOS.

**Por que acontece:** A partir de 10.15, `NSOpenGLContext setView:` e `update` são
obrigatórias na main thread. SDL2 despacha para a main thread internamente ao chamar
`SDL_GL_MakeCurrent`, mas se o loop de render estiver em uma thread secundária (ex.:
tentativa de separar render de game logic), o dispatch pode deadlockear ou crashar.

**Consequências:** Crash instantâneo ao iniciar rendering em qualquer arquitetura
multi-thread. Difícil de reproduzir porque só afeta macOS 10.15+.

**Sinais de alerta:**
- `EXC_BAD_INSTRUCTION` no stack trace ao chamar qualquer função GL
- Stack contendo `NSOpenGLContext`, `setView`, `Cocoa_GL_MakeCurrent`
- Funciona em Linux, crasha no Mac

**Prevenção:**
- Manter todo o código SDL e OpenGL na main thread (padrão de jogos C simples — adequado para este projeto)
- Nunca criar threads separadas para rendering ou input sem verificar se SDL está OK com isso no macOS
- Se necessariamente multi-thread: usar `dispatch_sync(dispatch_get_main_queue(), ^{...})` para toda chamada SDL/GL

**Fase que deve tratar:** Fase de renderização 3D (setup do contexto) e qualquer fase que introduza threads

---

### Pitfall 4: Big-bang refactoring — partir o arquivo e adicionar 3D ao mesmo tempo

**O que dá errado:** O desenvolvedor tenta simultaneamente: (a) extrair `game.c`/`render.c`
do monólito `sokoban.c` e (b) adicionar OpenGL e 3D. As duas mudanças se interferem — não
há ponto de rollback claro, testes quebram por múltiplas razões ao mesmo tempo, e o jogo
fica injogável por dias ou semanas.

**Por que acontece:** O impulso natural em brownfield é "limpar antes de adicionar". Mas
sem testes automatizados (inexistentes neste projeto), refatorar e adicionar features ao
mesmo tempo remove o único sinal de sanidade: o jogo rodando.

**Consequências:** Semanas de debugging sem feedback do jogo funcionando. Alta chance de
abandonar a branch e recomeçar, perdendo trabalho.

**Sinais de alerta:**
- PR com mais de 300 linhas de mudança que inclui tanto movimentação de código quanto nova feature
- O jogo não compila (ou compila mas crasha) por mais de 2 dias seguidos
- `git diff --stat` mostra mudanças em 5+ arquivos ao mesmo tempo para uma única tarefa

**Prevenção:**
- **Regra de ouro:** Uma mudança por vez, jogo sempre jogável ao final de cada commit
- Sequência correta:
  1. Extrair `Board`/`game.c` **sem alterar comportamento** → jogo funciona igual
  2. Adicionar camada de render abstrata (ainda 2D) → jogo funciona igual
  3. Plugar OpenGL na camada de render → jogo funciona, visual pode mudar
  4. Adicionar geometria 3D incrementalmente tile por tile
- Usar `#ifdef USE_3D_RENDER` como feature flag temporária durante a transição

**Fase que deve tratar:** Fase de refatoração / arquitetura (deve ser fase dedicada antes da 3D)

---

### Pitfall 5: Z-fighting e ordenação de profundidade em 3D tile-based isométrico

**O que dá errado:** Tiles adjacentes (chão + parede, caixa em cima do chão) ficam com
artefatos de z-fighting — pixels alternando entre as duas superfícies a cada frame,
criando tremulação visual. Piora em câmera oblíqua (estilo isométrico).

**Por que acontece:** Com polígonos coplanares ou quase coplanares, o depth buffer de 24 bits
não tem precisão suficiente para distingui-los quando a razão near/far é muito grande.
No estilo 3D "fake" (paredes extrudadas de grid 2D), múltiplos tiles compartilham exatamente
o mesmo plano Z.

**Consequências:** Paredes tremeluzentes, bordas de tiles piscando, visual inaceitável.
Muito difícil de consertar "depois" porque afeta toda a arquitetura de renderização.

**Sinais de alerta:**
- Borda de tile aparece e some a cada frame (especialmente em movimentos de câmera)
- `glPolygonOffset` sendo adicionado como "workaround"
- Tiles de chão visíveis através de paredes em certos ângulos

**Prevenção:**
- Escolher estratégia de renderização **antes** de começar: depth buffer OR painter's algorithm
  - **Opção A (recomendada para este projeto):** Painter's Algorithm — renderizar tiles de trás
    para frente por `(col + row)`, sem depth buffer. Correto para ortogonal/isométrico 2.5D.
  - **Opção B:** Depth buffer 3D real com câmera perspective, near=0.1, far=100.0, sem polígonos coplanares
- Nunca misturar as duas abordagens no mesmo frame
- Definir near/far clip planes o mais apertados possível (`near=0.1f`, `far=50.0f` para grid pequeno)
- Evitar `glPolygonOffset` como solução — é implementation-defined e inconsistente entre GPUs

**Fase que deve tratar:** Fase de renderização 3D — decisão arquitetural no início da fase

---

## Armadilhas Moderadas

---

### Pitfall 6: SDL2 working directory no macOS ≠ SDL 1.2

**O que dá errado:** No SDL 1.2, o working directory era setado automaticamente para o
diretório do executável. No SDL2, o working directory é herdado do shell (ou `/` se
lançado pelo Finder). Fonts, arquivos `.sok`, e perfis de usuário não são encontrados.

**Por que acontece:** Mudança de comportamento entre SDL 1.2 e 2.0 não documentada
na migração. O código atual já tem hardcoded paths de Linux (`/usr/share/fonts/...`),
o que vai quebrar no macOS de qualquer forma.

**Sinais de alerta:**
- `"Nenhuma fonte encontrada."` ao rodar de fora do diretório do projeto
- Arquivo de perfil de usuário criado em `/` (raiz do sistema) ao rodar pelo Finder
- `fopen("levels/level1.sok", "r")` retornando NULL no macOS

**Prevenção:**
```c
// No início do main(), antes de qualquer fopen:
char *base = SDL_GetBasePath();  // ex: "/Users/allan/git/soko-ban/"
chdir(base);
SDL_free(base);
```
- Todos os `fopen` com paths relativos funcionam normalmente depois disso
- Tratar na fase de setup/perfis de usuário, onde primeiro arquivo em disco é criado

**Fase que deve tratar:** Fase de perfis de usuário (primeiro acesso a arquivo em disco)

---

### Pitfall 7: Armadilha da nostalgia — fidelidade pixel-perfect vs. "inspired"

**O que dá errado:** O desenvolvedor vê o vídeo do Sokoban 1984 e começa a reproduzir
cada pixel, cada frame de animação, cada cor exata. Semanas são gastas tentando replicar
animações do elevador frame-a-frame. O gameplay (o coração do projeto) fica para depois.

**Por que acontece:** Referência visual concreta (vídeo no YouTube) ativa o instinto de
"fazer igual". O PROJECT.md já define fidelidade "inspired", mas na prática a tentação
de replicar é forte quando há referência visual disponível.

**Consequências:** A animação de abertura fica perfeita, mas o menu de seleção de usuário
nunca é feito. Feature creep vertical (uma feature muito profunda) em vez de horizontal
(todas as features no MVP).

**Sinais de alerta:**
- Mais de 3 dias gastos na animação de abertura antes de qualquer outra feature
- Comparações frequentes com screenshots do original ("esse tijolo está 2px errado")
- O arquivo `descricao.md` sendo tratado como spec técnica em vez de inspiração visual

**Prevenção:**
- Definir explicitamente o que "inspired" significa no PLAN.md de cada fase:
  ex.: "Elevador: mesma ideia de menu animado, sem replicar timing frame-a-frame"
- Time-box a fase de animação de abertura (máximo 3 dias de implementação)
- Sequência: gameplay end-to-end primeiro → visual fiel segundo
- Criar checklist de "done" para visual: "parece 3D com tijolos" é suficiente para v1

**Fase que deve tratar:** Fase de animação de abertura e fase de UI de elevador

---

### Pitfall 8: Estado global vazando para a camada de render 3D

**O que dá errado:** As variáveis globais existentes (`B` Board, `undo_stack`, `level_index`)
começam a ser acessadas diretamente dentro das novas funções de render OpenGL, criando
acoplamento que impede testar o render isolado ou trocar o backend.

**Por que acontece:** O padrão atual do código é global state + função monolítica. Ao
adicionar código novo no mesmo arquivo (ou sem estrutura clara), o padrão se repete.

**Consequências:** Impossível testar renderização sem um estado de jogo completo.
Impossível trocar renderer sem tocar em lógica de jogo. Bugs sutis quando o render
muda o estado do jogo acidentalmente.

**Sinais de alerta:**
- Funções `render_*` que leem diretamente `B.grid` sem receber parâmetro
- `level_index` sendo usado dentro de código de shader/VBO
- Dificuldade de escrever um `main_test.c` que renderiza uma cena de teste sem carregar uma fase real

**Prevenção:**
- Interface de dados mínima para o renderer: `render_frame(const RenderView *view)` onde
  `RenderView` é uma snapshot copiada do estado de jogo — o renderer não toca em `B` diretamente
- Separar fisicamente em arquivos: `game.c` (lógica pura, zero SDL), `render.c` (OpenGL/SDL,
  zero lógica de jogo)
- Regra: se `render.c` incluir `game.h`, OK; se `game.c` incluir `render.h`, é red flag

**Fase que deve tratar:** Fase de refatoração arquitetural (antes de adicionar 3D)

---

### Pitfall 9: Animação do elevador como projeto paralelo infinito

**O que dá errado:** A UI de elevador (menu principal + seleção de nível com "andares")
é visualmente atraente mas tecnicamente é um mini-sistema de animação completo:
easing, estados de transição, texto animado, sons. Pode consumir 60-80% do tempo total
sem entregar gameplay.

**Por que acontece:** O conceito de "elevador" é concreto e visual — fácil de começar,
difícil de terminar porque sempre há "mais um detalhe" de polish.

**Consequências:** MVP nunca fica jogável. Animação linda, sem níveis jogar.

**Sinais de alerta:**
- Branch de "elevator UI" com mais de 2 semanas de commits sem nenhum commit de gameplay
- Código de easing mais complexo que o código de movimento do player
- Sistema de "floor indicator" antes de ter seleção de nível funcional

**Prevenção:**
- Implementar placeholder funcional primeiro: menu de texto simples, funciona, seleciona
- Adicionar animação do elevador **por cima** do menu funcional (substituição visual, não arquitetural)
- Feature flag: `#define ELEVATOR_ANIM 0` — desliga animação, mantém funcionalidade
- Critério de done para MVP do menu: "usuário consegue selecionar Play e entrar no jogo"

**Fase que deve tratar:** Fase de menus / UI de elevador

---

### Pitfall 10: Loop de tempo com SDL_Delay fixo causa jitter em animações

**O que dá errado:** O loop atual usa `SDL_Delay(16)` (≈60fps). Em animações (abertura,
elevador), isso causa jitter porque `SDL_Delay` não é preciso (pode dormir 18, 20ms) e
acumula erro temporal. Animações ficam com velocidade irregular.

**Por que acontece:** `SDL_Delay(16)` é simples e funciona para input polling, mas para
animação baseada em tempo real é impreciso. O código atual usa `static Uint32 frozen` como
timer — padrão que não escala.

**Consequências:** Animação de abertura com velocidade inconsistente entre máquinas.
Elevator animation jerky em máquinas lentas.

**Sinais de alerta:**
- Animação visualmente mais rápida em Mac com tela 120Hz
- Delta time calculado como `16` constante em vez de `SDL_GetTicks() - last_frame`
- `static` local timers em múltiplas funções de render/animação

**Prevenção:**
```c
// Loop correto com delta time real:
Uint32 last = SDL_GetTicks();
while (running) {
    Uint32 now = SDL_GetTicks();
    float dt = (now - last) / 1000.0f;  // segundos
    last = now;
    update(dt);
    render();
    // cap a 60fps sem SDL_Delay fixo:
    Uint32 elapsed = SDL_GetTicks() - now;
    if (elapsed < 16) SDL_Delay(16 - elapsed);
}
```
- Passar `dt` explicitamente para todas as funções de animação
- Nunca usar `static` local para timers de animação — usar campo no struct de estado da cena

**Fase que deve tratar:** Fase de animação de abertura (primeira que precisar de tempo real)

---

## Armadilhas Menores

---

### Pitfall 11: Paths de fonte hardcoded para Linux

**O que dá errado:** O código atual busca fontes em `/usr/share/fonts/truetype/dejavu/`
e similares — caminhos exclusivos de Linux. No macOS, a busca falha e o jogo encerra
com "Nenhuma fonte encontrada." (já documentado em CONCERNS.md).

**Prevenção:** Usar `SDL_GetBasePath()` para buscar `RobotoMono-Bold.ttf` (já bundled
no projeto) antes de qualquer fallback do sistema. No macOS, fontes de sistema estão em
`/System/Library/Fonts/` e `/Library/Fonts/`, não nos paths Linux.

**Fase que deve tratar:** Fase de refatoração inicial / setup macOS

---

### Pitfall 12: Undo stack silencioso após 4096 movimentos

**O que dá errado:** `push_undo` descarta silenciosamente entradas após 4096 movimentos.
O jogador perde a funcionalidade de undo sem feedback (já documentado em CONCERNS.md).

**Prevenção:** Implementar ring buffer circular OR mostrar aviso visual quando o stack
está cheio. Trivial de corrigir na fase de refatoração.

**Fase que deve tratar:** Fase de refatoração do `Board` / extração de `game.c`

---

### Pitfall 13: Janela de tamanho fixo sem suporte a HiDPI/resize

**O que dá errado:** `win_w = MAXW * TILE = 896px` hardcoded. Em Retina Mac, a janela
parece pequena (448 pontos CSS equivalentes). Sem `SDL_WINDOW_ALLOW_HIGHDPI`, o renderer
não usa a resolução real do display.

**Prevenção:** Adicionar `SDL_WINDOW_ALLOW_HIGHDPI` nos flags de criação da janela.
Usar `SDL_GL_GetDrawableSize` (ver Pitfall 2) para todas as operações de renderização.

**Fase que deve tratar:** Fase de renderização 3D (setup da janela)

---

### Pitfall 14: Binário compilado sem .gitignore

**O que dá errado:** O binário `sokoban` está no working tree sem `.gitignore`. Commits
acidentais do binário incham o histórico git com arquivos de 100-500KB que não têm diff
legível.

**Prevenção:** Adicionar `.gitignore` com `sokoban` (e futuros `*.o`, `*.a`) antes da
próxima fase de desenvolvimento.

**Fase que deve tratar:** Qualquer fase de setup inicial — deve ser o primeiro commit

---

## Aviso por Fase

| Fase | Armadilha Provável | Mitigação |
|------|--------------------|-----------|
| Setup / Refatoração | Big-bang (Pitfall 4), globals vazando (Pitfall 8) | Sequenciar: split → interface limpa → 3D |
| Renderização 3D | OpenGL 4.1 macOS (Pitfall 1), HiDPI (Pitfall 2), Z-fighting (Pitfall 5), thread crash (Pitfall 3) | Testar no macOS na primeira compilação; escolher depth strategy antes de começar |
| Animação de Abertura | Nostalgia trap (Pitfall 7), SDL_Delay jitter (Pitfall 10) | Time-box 3 dias; usar delta time real |
| UI de Elevador | Feature creep infinito (Pitfall 9), nostalgia trap (Pitfall 7) | Placeholder funcional antes de animação |
| Perfis de Usuário | Working directory macOS (Pitfall 6) | SDL_GetBasePath + chdir na init |
| Seleção de Níveis | Z-fighting se tiles do elevador usarem 3D (Pitfall 5) | Manter UI como 2D/SDL_Rect, só o board é 3D |

---

## Fontes

- [SDL2 macOS README — Working Directory](https://wiki.libsdl.org/SDL2/README-macos) — MEDIUM confidence
- [SDL2 GitHub Issue #3507 — NSOpenGLContext main thread crash](https://github.com/libsdl-org/SDL/issues/3507) — HIGH confidence (official repo)
- [SDL2 Discourse — Mojave HiDPI DrawableSize bug](https://discourse.libsdl.org/t/macos-10-14-mojave-issues/25060) — MEDIUM confidence
- [Stack Overflow — macOS OpenGL 4.1 freeze](https://stackoverflow.com/questions/68022212/will-sdl-still-officially-support-osx-when-osx-no-longer-supports-opengl) — HIGH confidence
- [Khronos Forums — Z-fighting near/far planes](https://community.khronos.org/t/terrain-z-fighting-issues/61820) — HIGH confidence
- [mazebert.com — Isometric depth sorting topological](https://mazebert.com/forum/news/isometric-depth-sorting--id775/) — MEDIUM confidence
- [The Force Engine blog — Global state refactor in retro game engine](https://theforceengine.github.io/blog.html) — MEDIUM confidence
- [Fortress of Doors — Doing an HD Remake the Right Way](https://www.fortressofdoors.com/doing-an-hd-remake-the-right-way/) — MEDIUM confidence
- [Wayline — Feature/Scope Creep em indie games](https://www.wayline.io/blog/toxic-triangle-feature-scope-gold-plating-indie-games) — MEDIUM confidence
- CONCERNS.md — análise direta do codebase existente — HIGH confidence
