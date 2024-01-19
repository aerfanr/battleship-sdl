#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <iostream>
#include "draw.h"

struct pos {
	int x;
	int y;
};

const int SHIPS[] = {5, 4, 3, 3, 2};

SDL_Window *window = nullptr;
SDL_Surface *surface = nullptr;

CellState board[10][10];

bool init() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
		std::cerr << "Failed to initialize SDL: "
			<< SDL_GetError() << std::endl;
		return 1;
	}

	window = SDL_CreateWindow(
		"Battleship",
		SDL_WINDOWPOS_UNDEFINED,
		SDL_WINDOWPOS_UNDEFINED,
		640,
		480,
		SDL_WINDOW_SHOWN
	);
	if (window == nullptr) {
		std::cerr << "Failed to create window: "
			<< SDL_GetError() << std::endl;
		return 1;
	}

	surface = SDL_GetWindowSurface(window);
	if (surface == nullptr) {
		std::cerr << "Failed to load image: "
			<< SDL_GetError() << std::endl;
		return 1;
	}

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			board[i][j] = EMPTY;
		}
	}

	return 0;
}

bool place_ship(int x, int y, int width, int height) {
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
			--board[y + i][x + j];
		}
	}
	switch (k) {
		case SDLK_SPACE:
			if (place_ship(x, y, width, height)) {
				break;
			}
			ship++;
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
			++board[y + i][x + j];
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
		++board[cursor.y][cursor.x + i];
	}

	while (ship < 5) {
		SDL_Event e;
		if (SDL_WaitEvent(&e)) {
			if (e.type == SDL_QUIT) {
				break;
			}

			if (e.type == SDL_KEYDOWN) {
				handle_stage1_input(
					cursor.x, cursor.y, e.key.keysym.sym,
					ship_vertical, ship
				);
			}
		}

		if (draw_board(board, surface, false, 10, 10)) return 1;
		if (draw_board(board, surface, true, 300, 10)) return 1;

		SDL_UpdateWindowSurface(window);
	}

	return 0;
}

bool quit() {
	SDL_DestroyWindow(window);
	SDL_Quit();

	return 0;
}

int main(int argc, char *argv[]) {
	if (init()) return 1;

	SDL_FillRect(surface, NULL, 
	      SDL_MapRGB(surface->format, 0xFF, 0xFF, 0xFF)
	);

	if (listen()) return 1;


	if (quit()) return 1;

	return 0;
}
