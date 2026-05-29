# SOKO-BAN — edição nostálgica (estilo CGA 1984)

Uma releitura do clássico Sokoban da Spectrum HoloByte, com o visual
retrô dos PCs dos anos 80 (paleta CGA ciano/magenta) mas com cores
mais vivas e suaves para monitores atuais.

## Dependências

**macOS (Homebrew):**
```bash
brew install sdl2 sdl2_ttf
```

**Debian / Ubuntu / Mint:**
```bash
sudo apt install build-essential libsdl2-dev libsdl2-ttf-dev
```

**Fedora:**
```bash
sudo dnf install gcc make SDL2-devel SDL2_ttf-devel
```

**Arch / Manjaro:**
```bash
sudo pacman -S gcc make sdl2 sdl2_ttf
```

## Compilar e rodar
```bash
make        # compila
make run    # compila e executa
./sokoban   # ou rode direto
```

O jogo tenta carregar a fonte em `assets/fonts/RobotoMono-Bold.ttf` e,
se não encontrar, usa fontes do sistema (SF Mono no macOS, DejaVu no Linux).

## Controles
| Tecla | Ação |
|-------|------|
| Setas / W A S D | Mover o personagem |
| U / Backspace | Desfazer (undo) |
| R | Reiniciar a fase |
| N / P | Próxima / fase anterior |
| ENTER | Avançar nas telas |
| ESC / Q | Sair |

## Objetivo
Empurre todas as caixas (douradas) para cima dos alvos (diamantes ciano).
Quando uma caixa está no lugar certo, ela fica **verde**. Você só empurra
uma caixa por vez e não pode puxá-las — pense antes de mover!

## Personalizar fases
As fases ficam no array `levels[]` no topo de `sokoban.c`, em formato
texto padrão Sokoban:
```
#  parede      $  caixa        .  alvo
@  jogador     *  caixa no alvo  +  jogador no alvo
(espaço)  piso vazio
```
É só adicionar novas strings ao array para criar seus próprios desafios.
