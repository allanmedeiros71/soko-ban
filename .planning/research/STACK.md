# Stack Research — Soko-ban 3D Retro Remake

**Projeto:** Soko-ban (brownfield C/SDL2 → 3D)
**Pesquisa:** 2026-05-29
**Dimensão:** Stack
**Modo:** Brownfield evolution — preserve C base, add 3D layer

---

## Recommended Stack (with versions)

### Core — manter

| Tecnologia | Versão | Papel | Justificativa |
|-----------|--------|-------|---------------|
| C | C11 | Linguagem principal | Codebase existente; C11 adiciona `_Bool`, `_Alignas`, atomics; sem custo de migração |
| SDL2 | 2.30.x (latest) | Windowing, eventos, timer, 2D overlay | Já em uso; SDL_GL_CreateContext substitui SDL2_Renderer sem trocar a camada de input/audio |
| SDL2_ttf | 2.22.x | Texto do HUD e menus | Já em uso; gera texturas para uso no pipeline OpenGL |
| GNU Make | 4.x | Build system | Já em uso; Makefile simples basta para escopo atual |

### Core — adicionar

| Tecnologia | Versão | Papel | Justificativa |
|-----------|--------|-------|---------------|
| OpenGL | 3.3 Core Profile | Renderização 3D | macOS suporta até 4.1; 3.3 é suficiente para cubos low-poly com iluminação Phong; universalmente disponível em Linux/macOS; documentação excelente (learnopengl.com) |
| GLAD 2 | 2.0.8 | GL extension loader | Substitui GLEW; gera dois arquivos C (glad.c + glad.h) via glad.sh; sem dependência externa; integra no Makefile com um `gcc glad.c sokoban.c ...` |
| cglm | 0.9.6 | Matemática 3D (vetores, matrizes) | Header-only; SIMD (SSE/AVX/NEON para Apple Silicon); API familiar (`glm_mat4_mul`, `glm_perspective`, `glm_lookat`); última release fev/2025 |

### Persistência

| Tecnologia | Versão | Papel | Justificativa |
|-----------|--------|-------|---------------|
| cJSON | 1.7.18 | Perfis de usuário locais | Dois arquivos (cJSON.c + cJSON.h); ANSI C89; 13K stars; perfeito para `{name, lastLevel, highScore}` sem dependência externa |

### Dependências de desenvolvimento (macOS)

```bash
# Homebrew
brew install sdl2 sdl2_ttf

# GLAD: gerar via web (não instalar via Homebrew)
# https://glad.sh → API: OpenGL, Spec: Core, Version: 3.3
# Output: C/C++, header-only: false → baixa glad.c + glad.h

# cglm e cJSON: copiar headers/source diretamente no repositório
# (ambos são single-file friendly)
```

---

## Rationale — Por que cada escolha

### SDL2 (manter, não migrar para SDL3)

SDL3 introduziu `SDL_GPU` (Metal/Vulkan/DX12) e é o caminho futuro, mas **a migração de SDL2 → SDL3 quebra o API** (SDL_CreateRenderer, SDL_CreateTexture, SDL_BlitSurface mudaram). Para este projeto brownfield de escopo pequeno (~420 linhas), o custo de migrar SDL2 → SDL3 supera o benefício.

SDL2 cria contexto OpenGL com `SDL_GL_CreateContext(window)` — exatamente o que precisamos. A camada de input, timer e gerenciamento de janela fica inalterada.

**Decisão:** SDL2 permanece. OpenGL substitui apenas a renderização.

### OpenGL 3.3 Core Profile (não 4.x, não Vulkan, não Metal)

- **macOS limita OpenGL a 4.1** (deprecated desde macOS 10.14/Mojave, porém **funcional** no macOS Sequoia 15.x em 2026 — Apple implementa GL sobre Metal internamente). Confirmar: `OpenGL.framework` está presente em `/System/Library/Frameworks/` em todas as versões recentes.
- **OpenGL 3.3** oferece tudo necessário: VBO, VAO, shaders GLSL, texturas, profundidade — suficiente para cubos Phong-shaded e personagem low-poly.
- **Vulkan** seria 1000+ linhas de boilerplate para o mesmo resultado visual. MoltenVK adiciona dependência e complexidade sem ganho prático para um Sokoban retro.
- **Metal** é macOS-only; quebra suporte a Linux.
- **SDL_GPU (SDL3)** é a resposta certa em longo prazo, mas exige migração SDL2→SDL3 + SPIR-V shaders + SDL_shadercross. Escopo demais para v1.

**Decisão:** OpenGL 3.3 Core Profile. Supressão de warnings de deprecação no build macOS: `-Wno-deprecated-declarations`.

### GLAD 2 (não GLEW)

- GLEW requer instalação como lib externa (`libglew-dev`).
- GLAD gera dois arquivos que se tornam parte do repositório. Alinha com a filosofia atual (single-file, sem deps externas).
- GLAD 2 suporta OpenGL, Vulkan, EGL; forward-compatible.
- **Integração no Makefile:** `gcc glad/glad.c sokoban.c -o sokoban ...`

### cglm 0.9.6 (não GLM C++, não hand-rolled)

- GLM é C++ apenas; mudaria a linguagem do projeto.
- Hand-rolled matrix math funciona mas é código a manter; cglm resolve `mat4`, `vec3`, perspective, lookat, rotação — tudo necessário para câmera isométrica 3D do Sokoban.
- Header-only, zero linking, SIMD-aware (NEON para Apple Silicon M1/M2/M3/M4).

### cJSON 1.7.18 (não SQLite, não INI custom)

- SQLite é overkill para 3-10 perfis locais com nome + nível.
- Parser INI custom é código a manter.
- cJSON: dois arquivos, API clara (`cJSON_GetObjectItem`, `cJSON_Print`), MIT license. Formato legível por humanos (útil em dev).

---

## What NOT to Use — e por quê

| Opção | Por que não |
|-------|------------|
| **SDL3 + SDL_GPU** | Correto em longo prazo, mas migração SDL2→SDL3 quebra API; SDL_GPU exige SPIR-V shaders + SDL_shadercross; custo > benefício para v1 |
| **raylib** | Substitui SDL2 — não augmenta. Reescreveria a camada de windowing/input. Excelente para projetos novos, errado para brownfield SDL2 |
| **Vulkan** | 1000+ linhas de boilerplate para o mesmo resultado visual. Sem ganho prático para cubos low-poly |
| **Metal direto** | macOS-only; quebra Linux. Só justificado se precisasse de features exclusivas Metal |
| **OpenGL 4.x** | Desnecessário (não há features GL4 usadas aqui) e macOS cap é 4.1 de qualquer forma |
| **GLEW** | Depende de lib externa instalada no sistema; GLAD é superior e auto-contido |
| **GLM (C++)** | C++ only. Mudaria linguagem do projeto |
| **SDL2_renderer 2D com isometric tricks** | Não é 3D real; limitado; não entrega a estética de tijolo/caixote 3D do original |
| **Unity / Godot / Unreal** | Reescrita completa. Elimina o charme do projeto C nativo |
| **CMake** | Makefile simples já funciona; CMake adiciona complexidade sem benefício para projeto single-file evoluindo para 3-5 arquivos |

---

## Confidence Levels

| Área | Nível | Razão |
|------|-------|-------|
| SDL2 + OpenGL no macOS | **HIGH** | Confirmado via SDL GitHub issues e SO: OpenGL.framework presente no macOS Sequoia; `SDL_GL_CreateContext` funcional |
| OpenGL 3.3 suficiência | **HIGH** | Low-poly 3D (cubos, personagem blocky) não exige nenhuma feature GL4 |
| GLAD 2 vs GLEW | **HIGH** | Documentação oficial GLAD; confirmado como recomendação moderna pela comunidade Khronos |
| cglm 0.9.6 | **HIGH** | Release verificada (fev/2025, GitHub recp/cglm); header-only, SIMD; Apple Silicon via NEON |
| cJSON | **HIGH** | 13K stars, amplamente usado em C game dev; single-file; MIT |
| OpenGL deprecation risco | **MEDIUM** | Apple ainda não removeu após 8 anos de deprecação; GL rodando sobre Metal internamente. Risco real mas baixo no horizonte v1 |

---

## macOS-Specific Notes

### OpenGL deprecation (prático, não teórico)

Apple deprecated OpenGL em macOS 10.14 (Mojave, 2018). Em 2026 — **oito anos depois** — OpenGL.framework ainda está presente e funcional em macOS Sequoia. A implementação Apple roda GL sobre Metal internamente. Risco de remoção é real mas improvável no horizonte v1.

**Mitigação no build:**

```makefile
# Suprimir warnings de deprecação no macOS
CFLAGS_MACOS = -Wno-deprecated-declarations

# Link OpenGL no macOS (não libGL.dylib — use framework)
LIBS_MACOS = -framework OpenGL -framework CoreFoundation
```

### Apple Silicon (M1/M2/M3/M4)

- OpenGL 3.3 funciona em Apple Silicon via Metal translation layer.
- cglm 0.9.6 inclui otimizações NEON para ARM — performance nativa em M-series.
- SDL2 universal binary disponível via Homebrew (arm64 nativo).
- GLAD gerado em `glad.sh` é puro C, compila em qualquer arquitetura.

### Build command macOS (exemplo final)

```bash
gcc sokoban.c glad/glad.c cJSON.c \
    -o sokoban \
    $(sdl2-config --cflags --libs) \
    -lSDL2_ttf \
    -framework OpenGL \
    -Iinclude \
    -Wno-deprecated-declarations \
    -O2 -Wall -std=c11
```

### Font paths (atualizar para macOS)

Codebase atual usa paths Linux (`/usr/share/fonts/...`). macOS usa:

```c
"/System/Library/Fonts/Supplemental/Arial.ttf"
"/Library/Fonts/Arial.ttf"
// ou via bundled font (já existe BUNDLED_FONT no código)
```

A fonte bundled `RobotoMono-Bold.ttf` já presente no projeto resolve isso.

---

## Summary para o Roadmap

**Stack recomendada (brownfield, sem quebrar nada):**

```
C11 + SDL2 2.30 + OpenGL 3.3 Core + GLAD 2 + cglm 0.9.6 + cJSON 1.7.18
```

- **Camada de jogo** (lógica, input, UI texto): SDL2 permanece intacto.
- **Camada de render**: SDL2_Renderer é trocada por contexto OpenGL. Todo código de `SDL_RenderRect` / `SDL_RenderFillRect` existente é substituído por draw calls OpenGL via VBO/VAO.
- **Matemática 3D**: cglm injeta `mat4`, `vec3`, `glm_perspective`, `glm_lookat` sem nenhuma dependência de link adicional (header-only).
- **Persistência**: cJSON para arquivo `profiles.json` local (sem rede, sem SQLite).
- **Shaders**: GLSL 330 core; dois shaders (vertex + fragment) embedded como string literals ou arquivos `.glsl` no repo.

**Fases implicadas pelo stack:**
1. Setup OpenGL context + GLAD + pipeline básico (substitui renderer 2D)
2. Cubos 3D para paredes/caixotes com cglm MVP + iluminação Phong flat
3. Personagem 3D (modelo blocky, pode ser cubo composto)
4. Menus elevador + animação abertura (SDL2_ttf → texturas OpenGL + animação via timer SDL)
5. Perfis de usuário (cJSON, arquivo local)

---

*Pesquisa: 2026-05-29 | Confiança geral: HIGH*
