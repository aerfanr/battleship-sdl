#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <iostream>
#include "draw.h"

SDL_Window *window = nullptr;
SDL_Surface *surface = nullptr;

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

	return 0;
}

bool listen() {
	while (true) {
		SDL_Event e;
		if (SDL_WaitEvent(&e)) {
			if (e.type == SDL_QUIT) {
				break;
			}
		}
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

	CellState board[10][10];
	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			board[i][j] = EMPTY;
		}
	}
	board[3][4] = HIT;
	board[3][7] = FULL;
	board[3][5] = MISS;
	board[7][6] = EMPTY_S;

	if (draw_board(board, surface, false, 10, 10)) return 1;
	if (draw_board(board, surface, true, 300, 10)) return 1;

	SDL_UpdateWindowSurface(window);

	if (listen()) return 1;


	if (quit()) return 1;

	return 0;
}
