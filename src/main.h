#ifndef MAIN_H
#define MAIN_H
#include <SDL2/SDL_rect.h>

struct pos {
	int x;
	int y;
};

struct ShipPos {
	int x;
	int y;
	bool vertical;
};

enum CellState {
	EMPTY = 0, EMPTY_S,
	FULL, FULL_S,
	HIT, HIT_S,
	MISS, MISS_S
};
CellState& operator++(CellState& state);
CellState& operator--(CellState& state);

const int SHIPS[] = {5, 4, 4, 3, 3, 2, 2, 2};
const int SHIP_COUNT = sizeof(SHIPS) / sizeof(SHIPS[0]);

struct Game {
	CellState board1[10][10];
	CellState board2[10][10];
	ShipPos ships1[SHIP_COUNT];
	ShipPos ships2[SHIP_COUNT];
	SDL_Rect text1_rect = {10, 300, 0, 0};
	SDL_Rect text2_rect = {300, 300, 0, 0};
	char text1[64];
	char text2[64];
};

#endif // !MAIN_H
