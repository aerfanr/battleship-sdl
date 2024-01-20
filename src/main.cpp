#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <ctime>
#include <iostream>
#include "draw.h"

struct pos {
	int x;
	int y;
};

const int SHIPS[] = {5, 4, 3, 3, 2};


CellState board1[10][10], board2[10][10];

void draw_boards() {
	draw_board(board1, true, 300, 10);
	draw_board(board2, false, 10, 10);
}

bool init() {
	if (init_draw()) return 1;

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			board1[i][j] = EMPTY;
			board2[i][j] = EMPTY;
		}
	}

	srand(std::time(nullptr));

	return 0;
}

bool place_ship(int x, int y, int width, int height, CellState board[10][10]) {
	if (x + width > 10 || y + height > 10) {
		return 1;
	}

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			if (board[y + i][x + j] != EMPTY) {
				return 1;
			}
		}
	}

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			board[y + i][x + j] = FULL;
		}
	}

	return 0;
}

void handle_stage1_input(int& x, int& y, SDL_Keycode k, bool& vertical, int& ship) {
	int width = SHIPS[ship];
	int height = 1;
	if (vertical) std::swap(width, height);

	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			--board1[y + i][x + j];
		}
	}
	switch (k) {
		case SDLK_SPACE:
			if (place_ship(x, y, width, height, board1)) {
				break;
			}
			ship++;
			if (ship >= sizeof(SHIPS) / sizeof(SHIPS[0])) {
				draw_boards();
				return;
			}
			width = SHIPS[ship];
			height = 1;
			if (vertical) std::swap(width, height);
		break;
		case SDLK_x:
			vertical = !vertical;
			std::swap(height, width);

			if (x + width > 10) x = 10 - width;
			if (y + height > 10) y = 10 - height;
		break;
		case SDLK_LEFT:
			if (x > 0) x--;
		break;
		case SDLK_RIGHT:
			if (x < 10 - width) x++;
		break;
		case SDLK_UP:
			if (y > 0) y--;
		break;
		case SDLK_DOWN:
			if (y < 10 - height) y++;
		break;
		default:
		break;
	}
	for (int i = 0; i < height; i++) {
		for (int j = 0; j < width; j++) {
			++board1[y + i][x + j];
		}
	}

	draw_boards();
}

bool handle_attack(int x, int y, CellState board[10][10]) {
	switch (board[y][x]) {
		case EMPTY:
			board[y][x] = MISS;
		break;
		case FULL:
			board[y][x] = HIT;
			return 1;
		break;
		default:
			return 1;
		break;
	}

	return 0;
}

void handle_stage2_input(int& x, int& y, SDL_Keycode k) {
	--board2[y][x];
	switch (k) {
		case SDLK_SPACE:
			if (handle_attack(x, y, board2)) break;
			while (handle_attack(rand() % 10, rand() % 10, board1)) {
				draw_boards();
				SDL_Delay(1000);
			}
		break;
		case SDLK_LEFT:
			if (x > 0) x--;
		break;
		case SDLK_RIGHT:
			if (x < 10 - 1) x++;
		break;
		case SDLK_UP:
			if (y > 0) y--;
		break;
		case SDLK_DOWN:
			if (y < 10 - 1) y++;
		break;
		default:
		break;
	}
	++board2[y][x];

	draw_boards();
}

void set_enemy_board() {
	for (int i = 0; i < sizeof(SHIPS) / sizeof(SHIPS[0]); i++) {
		int width = SHIPS[i];
		int height = 1;

		while (true) {
			if (rand() % 2) {
				std::swap(width, height);
			}

			if (!place_ship(
				rand() % 10,
				rand() % 10,
				width, height, board2
			)) {
				break;
			}
		}
	}
}

bool listen() {
	int ship = 0;
	int ship_width = SHIPS[ship];
	int ship_height = 1;
	bool ship_vertical = false;

	pos cursor = {0, 0};
	for (int i = 0; i < ship_width; i++) {
		++board1[cursor.y][cursor.x + i];
	}

	draw_boards();
	while (ship < sizeof(SHIPS) / sizeof(SHIPS[0])) {
		SDL_Event e;
		if (SDL_WaitEvent(&e)) {
			if (e.type == SDL_QUIT) {
				return 0;
			}

			if (e.type == SDL_KEYDOWN) {
				handle_stage1_input(
					cursor.x, cursor.y, e.key.keysym.sym,
					ship_vertical, ship
				);
			}
		}
	}

	set_enemy_board();
	++board2[cursor.y][cursor.x];
	draw_boards();

	while (true) {
		SDL_Event e;
		if (SDL_WaitEvent(&e)) {
			if (e.type == SDL_QUIT) {
				return 0;
			}

			if (e.type == SDL_KEYDOWN) {
				handle_stage2_input(
					cursor.x, cursor.y, e.key.keysym.sym
				);
			}
		}
	}


	return 0;
}

void quit() {
	quit_draw();
}

int main(int argc, char *argv[]) {
	if (init()) return 1;

	if (listen()) return 1;

	quit();
	return 0;
}
