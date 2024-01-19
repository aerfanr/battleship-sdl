#ifndef DRAW_H
#define DRAW_H
#include <SDL2/SDL.h>

enum CellState {
	EMPTY = 0, EMPTY_S,
	FULL, FULL_S,
	HIT, HIT_S,
	MISS, MISS_S
};
CellState& operator++(CellState& state);
CellState& operator--(CellState& state);

bool draw_board(CellState board[10][10], SDL_Surface *screen, bool show, int x, int y);

#endif // !DRAW_H
