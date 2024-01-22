#ifndef DRAW_H
#define DRAW_H
#include <SDL2/SDL_rect.h>
#include "main.h"

bool init_draw();
void quit_draw();
// bool draw_board(CellState board[10][10], bool show, int x, int y);
// void draw_text(const char *text, SDL_Rect* target);
void draw_frame(Game *game);

#endif // !DRAW_H
