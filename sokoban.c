/*
 * SOKO-BAN  —  versão nostálgica (estilo CGA/EGA anos 80)
 * Homenagem ao Sokoban original de Spectrum HoloByte (1984).
 *
 * Compilar:  gcc sokoban.c -o sokoban $(sdl2-config --cflags --libs) -lSDL2_ttf -lm
 * Rodar:     ./sokoban
 *
 * Controles:
 *   Setas / WASD .... mover
 *   U / Backspace ... desfazer (undo)
 *   R ............... reiniciar fase
 *   N / P ........... próxima / anterior fase
 *   ENTER ........... avançar nas telas de título / vitória
 *   ESC / Q ......... sair
 */

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

/* ---------- Paleta: CGA "atualizada" (cores vivas, espírito 80s) ---------- */
typedef struct { Uint8 r, g, b; } Color;
static const Color C_BLACK   = {  12,  10,  24 };  /* fundo escuro azulado   */
static const Color C_CYAN    = {  40, 220, 220 };  /* piso / ciano CGA       */
static const Color C_MAGENTA = { 230,  60, 200 };  /* paredes / magenta CGA  */
static const Color C_WHITE   = { 240, 240, 255 };  /* texto / player         */
static const Color C_DKMAG   = { 120,  20, 110 };  /* sombra de parede       */
static const Color C_GOLD    = { 245, 200,  60 };  /* caixas                 */
static const Color C_DKGOLD  = { 170, 120,  20 };  /* sombra de caixa        */
static const Color C_GREEN   = {  80, 230, 120 };  /* caixa no alvo          */
static const Color C_DKGRN   = {  30, 140,  70 };

/* ---------- Tiles ---------- */
#define WALL   '#'
#define FLOOR  ' '
#define GOAL   '.'
#define BOX    '$'
#define BOXG   '*'   /* caixa sobre alvo */
#define PLAYER '@'
#define PLYRG  '+'   /* player sobre alvo */

#define MAXW 32
#define MAXH 24
#define TILE 28
#define HUD  40

/* ---------- Fases (clássicas Sokoban) ---------- */
static const char *levels[] = {
/* 1 */
"    #####\n"
"    #   #\n"
"    #$  #\n"
"  ###  $##\n"
"  #  $ $ #\n"
"### # ## #   ######\n"
"#   # ## #####  ..#\n"
"# $  $          ..#\n"
"##### ### #@##  ..#\n"
"    #     #########\n"
"    #######\n",
/* 2 */
"############\n"
"#..  #     ###\n"
"#..  # $  $  #\n"
"#..  #$####  #\n"
"#..    @ ##  #\n"
"#..  # #  $ ##\n"
"###### ##$ $ #\n"
"  # $  $ $ $ #\n"
"  #    #     #\n"
"  ############\n",
/* 3 */
"        ########\n"
"        #     @#\n"
"        # $#$ ##\n"
"        # $  $#\n"
"        ##$ $ #\n"
"######### $ # ###\n"
"#....  ## $  $  #\n"
"##...    $  $   #\n"
"#....  ##########\n"
"########\n",
/* 4 */
"           #####\n"
"           #   #\n"
"           #$  #\n"
"         ###  $##\n"
"         #  $ $ #\n"
"       ### # ## #   ###\n"
"       #   # ## ##### .#\n"
"       # $  $        .#\n"
"       ##### ###@##  .#\n"
"           #     #####\n"
"           #######\n",
};
#define NLEVELS (int)(sizeof(levels)/sizeof(levels[0]))

/* ---------- Estado do jogo ---------- */
typedef struct {
    char grid[MAXH][MAXW];
    int  w, h;
    int  px, py;
    int  moves, pushes;
    Uint32 start_ms, elapsed_paused;
} Board;

#define UNDO_MAX 4096
typedef struct { char grid[MAXH][MAXW]; int px, py, moves, pushes; } Snapshot;
static Snapshot undo_stack[UNDO_MAX];
static int undo_top = 0;

static int level_index = 0;
static Board B;

/* ---------- Helpers de desenho ---------- */
static void set_color(SDL_Renderer *r, Color c) {
    SDL_SetRenderDrawColor(r, c.r, c.g, c.b, 255);
}
static void fill(SDL_Renderer *r, int x, int y, int w, int h, Color c) {
    SDL_Rect rc = { x, y, w, h };
    set_color(r, c);
    SDL_RenderFillRect(r, &rc);
}

/* ---------- Carregar fase ---------- */
static void load_level(int idx) {
    memset(&B, 0, sizeof(B));
    const char *s = levels[idx];
    int x = 0, y = 0, maxw = 0;
    for (const char *p = s; *p; p++) {
        if (*p == '\n') { if (x > maxw) maxw = x; x = 0; y++; continue; }
        B.grid[y][x] = *p;
        if (*p == PLAYER || *p == PLYRG) { B.px = x; B.py = y; }
        x++;
    }
    if (x > maxw) maxw = x;
    B.w = maxw; B.h = (x > 0) ? y + 1 : y;
    B.moves = B.pushes = 0;
    B.start_ms = SDL_GetTicks();
    undo_top = 0;
}

static bool is_won(void) {
    for (int y = 0; y < B.h; y++)
        for (int x = 0; x < B.w; x++)
            if (B.grid[y][x] == BOX) return false;   /* caixa fora de alvo */
    return true;
}

/* ---------- Undo ---------- */
static void push_undo(void) {
    if (undo_top >= UNDO_MAX) return;
    Snapshot *s = &undo_stack[undo_top++];
    memcpy(s->grid, B.grid, sizeof(B.grid));
    s->px = B.px; s->py = B.py; s->moves = B.moves; s->pushes = B.pushes;
}
static void do_undo(void) {
    if (undo_top <= 0) return;
    Snapshot *s = &undo_stack[--undo_top];
    memcpy(B.grid, s->grid, sizeof(B.grid));
    B.px = s->px; B.py = s->py; B.moves = s->moves; B.pushes = s->pushes;
}

/* ---------- Movimento ---------- */
static bool is_goal_cell(char c) { return c == GOAL || c == BOXG || c == PLYRG; }

static void move_player(int dx, int dy) {
    int nx = B.px + dx, ny = B.py + dy;
    if (nx < 0 || ny < 0 || nx >= B.w || ny >= B.h) return;
    char dest = B.grid[ny][nx];
    if (dest == WALL) return;

    if (dest == BOX || dest == BOXG) {
        int bx = nx + dx, by = ny + dy;
        if (bx < 0 || by < 0 || bx >= B.w || by >= B.h) return;
        char beyond = B.grid[by][bx];
        if (beyond == WALL || beyond == BOX || beyond == BOXG) return;
        push_undo();
        /* mover caixa */
        B.grid[by][bx] = is_goal_cell(beyond) ? BOXG : BOX;
        B.grid[ny][nx] = is_goal_cell(dest) ? GOAL : FLOOR;
        B.pushes++;
    } else {
        push_undo();
    }
    /* mover player */
    char from = B.grid[B.py][B.px];
    B.grid[B.py][B.px] = (from == PLYRG) ? GOAL : FLOOR;
    char now = B.grid[ny][nx];
    B.grid[ny][nx] = is_goal_cell(now) ? PLYRG : PLAYER;
    B.px = nx; B.py = ny;
    B.moves++;
}

/* ---------- Desenho dos tiles (estilo pixel-art CGA) ---------- */
static void draw_wall(SDL_Renderer *r, int px, int py) {
    /* parede de tijolos magenta */
    fill(r, px, py, TILE, TILE, C_DKMAG);
    int bh = TILE / 4;
    for (int row = 0; row < 4; row++) {
        int oy = py + row * bh;
        int offset = (row % 2) ? TILE / 2 : 0;
        for (int bx = -1; bx < 3; bx++) {
            int x = px + bx * (TILE / 2) + offset;
            fill(r, x + 1, oy + 1, TILE / 2 - 2, bh - 2, C_MAGENTA);
        }
    }
}
static void draw_floor(SDL_Renderer *r, int px, int py, bool goal) {
    fill(r, px, py, TILE, TILE, C_BLACK);
    if (goal) {
        /* diamante ciano (alvo) — como no original */
        int cx = px + TILE/2, cy = py + TILE/2, s = TILE/4;
        SDL_Point d[5] = {
            { cx, cy - s }, { cx + s, cy }, { cx, cy + s }, { cx - s, cy }, { cx, cy - s }
        };
        set_color(r, C_CYAN);
        SDL_RenderDrawLines(r, d, 5);
        /* preencher */
        for (int yy = -s; yy <= s; yy++) {
            int half = s - abs(yy);
            SDL_RenderDrawLine(r, cx - half, cy + yy, cx + half, cy + yy);
        }
    }
}
static void draw_box(SDL_Renderer *r, int px, int py, bool on_goal) {
    Color top = on_goal ? C_GREEN : C_GOLD;
    Color sh  = on_goal ? C_DKGRN : C_DKGOLD;
    int m = 3;
    fill(r, px+m, py+m, TILE-2*m, TILE-2*m, sh);
    fill(r, px+m, py+m, TILE-2*m-2, TILE-2*m-2, top);
    /* "X" de engradado */
    set_color(r, sh);
    SDL_RenderDrawLine(r, px+m, py+m, px+TILE-m-2, py+TILE-m-2);
    SDL_RenderDrawLine(r, px+TILE-m-2, py+m, px+m, py+TILE-m-2);
}
static void draw_player(SDL_Renderer *r, int px, int py) {
    int cx = px + TILE/2;
    /* cabeça */
    fill(r, cx-4, py+3, 8, 7, C_WHITE);
    /* corpo magenta (camisa) */
    fill(r, px+6, py+10, TILE-12, 9, C_MAGENTA);
    /* pernas */
    fill(r, px+7, py+19, 4, 6, C_CYAN);
    fill(r, px+TILE-11, py+19, 4, 6, C_CYAN);
}

/* ---------- Texto ---------- */
static void draw_text(SDL_Renderer *r, TTF_Font *f, const char *txt, int x, int y, Color c) {
    SDL_Color sc = { c.r, c.g, c.b, 255 };
    SDL_Surface *s = TTF_RenderText_Solid(f, txt, sc);
    if (!s) return;
    SDL_Texture *t = SDL_CreateTextureFromSurface(r, s);
    SDL_Rect dst = { x, y, s->w, s->h };
    SDL_RenderCopy(r, t, NULL, &dst);
    SDL_FreeSurface(s); SDL_DestroyTexture(t);
}
static void draw_text_c(SDL_Renderer *r, TTF_Font *f, const char *txt, int cx, int y, Color c) {
    int w, h; TTF_SizeText(f, txt, &w, &h);
    draw_text(r, f, txt, cx - w/2, y, c);
}

/* ---------- Programa principal ---------- */
typedef enum { SC_TITLE, SC_PLAY, SC_WIN } Scene;

int main(void) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) { fprintf(stderr, "SDL: %s\n", SDL_GetError()); return 1; }
    if (TTF_Init() != 0) { fprintf(stderr, "TTF: %s\n", TTF_GetError()); return 1; }

    int win_w = MAXW * TILE, win_h = MAXH * TILE + HUD;
    SDL_Window *win = SDL_CreateWindow("SOKO-BAN",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, win_w, win_h, 0);
    SDL_Renderer *ren = SDL_CreateRenderer(win, -1,
        SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);

    /* fonte: tenta algumas comuns no Linux */
    const char *fonts[] = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSansMono-Bold.ttf",
        "/usr/share/fonts/truetype/liberation/LiberationMono-Bold.ttf",
        "/usr/share/fonts/TTF/DejaVuSansMono-Bold.ttf",
        NULL
    };
    TTF_Font *font = NULL, *big = NULL;
    for (int i = 0; fonts[i]; i++) {
        font = TTF_OpenFont(fonts[i], 18);
        big  = TTF_OpenFont(fonts[i], 40);
        if (font && big) break;
    }
    if (!font) { fprintf(stderr, "Nenhuma fonte encontrada.\n"); return 1; }

    Scene scene = SC_TITLE;
    load_level(level_index);

    bool running = true;
    while (running) {
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
            if (e.type == SDL_KEYDOWN) {
                SDL_Keycode k = e.key.keysym.sym;
                if (k == SDLK_ESCAPE || k == SDLK_q) running = false;

                if (scene == SC_TITLE) {
                    if (k == SDLK_RETURN || k == SDLK_SPACE) scene = SC_PLAY;
                }
                else if (scene == SC_PLAY) {
                    if (k == SDLK_UP || k == SDLK_w)    move_player(0,-1);
                    if (k == SDLK_DOWN || k == SDLK_s)  move_player(0, 1);
                    if (k == SDLK_LEFT || k == SDLK_a)  move_player(-1,0);
                    if (k == SDLK_RIGHT|| k == SDLK_d)  move_player( 1,0);
                    if (k == SDLK_u || k == SDLK_BACKSPACE) do_undo();
                    if (k == SDLK_r) load_level(level_index);
                    if (k == SDLK_n) { level_index = (level_index+1)%NLEVELS; load_level(level_index); }
                    if (k == SDLK_p) { level_index = (level_index-1+NLEVELS)%NLEVELS; load_level(level_index); }
                    if (is_won()) scene = SC_WIN;
                }
                else if (scene == SC_WIN) {
                    if (k == SDLK_RETURN || k == SDLK_SPACE || k == SDLK_n) {
                        level_index = (level_index+1)%NLEVELS;
                        load_level(level_index);
                        scene = SC_PLAY;
                    }
                }
            }
        }

        /* ---- render ---- */
        set_color(ren, C_BLACK);
        SDL_RenderClear(ren);

        if (scene == SC_TITLE) {
            draw_text_c(ren, big,  "SOKO-BAN", win_w/2, 120, C_MAGENTA);
            draw_text_c(ren, font, "edicao nostalgica - estilo CGA 1984", win_w/2, 185, C_CYAN);
            draw_text_c(ren, font, "Setas/WASD: mover    U: desfazer    R: reiniciar", win_w/2, 320, C_WHITE);
            draw_text_c(ren, font, "N/P: trocar fase     ESC: sair", win_w/2, 350, C_WHITE);
            draw_text_c(ren, font, "[ ENTER para jogar ]", win_w/2, 430, C_GOLD);
        }
        else {
            /* tabuleiro centralizado */
            int ox = (win_w - B.w * TILE) / 2;
            int oy = (win_h - HUD - B.h * TILE) / 2;
            for (int y = 0; y < B.h; y++) {
                for (int x = 0; x < B.w; x++) {
                    char c = B.grid[y][x];
                    int px = ox + x*TILE, py = oy + y*TILE;
                    switch (c) {
                        case WALL:   draw_wall(ren, px, py); break;
                        case GOAL:   draw_floor(ren, px, py, true); break;
                        case BOX:    draw_floor(ren, px, py, false); draw_box(ren, px, py, false); break;
                        case BOXG:   draw_floor(ren, px, py, true);  draw_box(ren, px, py, true);  break;
                        case PLAYER: draw_floor(ren, px, py, false); draw_player(ren, px, py); break;
                        case PLYRG:  draw_floor(ren, px, py, true);  draw_player(ren, px, py); break;
                        case FLOOR:  draw_floor(ren, px, py, false); break;
                        default: break;
                    }
                }
            }
            /* HUD inferior (estilo barra do original) */
            fill(ren, 0, win_h - HUD, win_w, HUD, C_DKMAG);
            fill(ren, 0, win_h - HUD, win_w, 2, C_MAGENTA);
            char hud[160];
            Uint32 secs = (scene == SC_PLAY) ? (SDL_GetTicks() - B.start_ms)/1000 : 0;
            static Uint32 frozen = 0;
            if (scene == SC_PLAY) frozen = secs;
            snprintf(hud, sizeof hud, "FASE %d/%d   movimentos: %04d   empurroes: %04d   tempo: %u:%02u:%02u",
                     level_index+1, NLEVELS, B.moves, B.pushes,
                     frozen/3600, (frozen/60)%60, frozen%60);
            draw_text(ren, font, hud, 10, win_h - HUD + 10, C_CYAN);

            if (scene == SC_WIN) {
                fill(ren, win_w/2 - 230, win_h/2 - 70, 460, 140, C_BLACK);
                set_color(ren, C_GOLD);
                SDL_Rect bord = { win_w/2 - 230, win_h/2 - 70, 460, 140 };
                SDL_RenderDrawRect(ren, &bord);
                draw_text_c(ren, big,  "FASE COMPLETA!", win_w/2, win_h/2 - 50, C_GREEN);
                draw_text_c(ren, font, "[ ENTER para a proxima fase ]", win_w/2, win_h/2 + 20, C_WHITE);
            }
        }

        SDL_RenderPresent(ren);
        SDL_Delay(16);
    }

    TTF_CloseFont(font); if (big) TTF_CloseFont(big);
    SDL_DestroyRenderer(ren); SDL_DestroyWindow(win);
    TTF_Quit(); SDL_Quit();
    return 0;
}
