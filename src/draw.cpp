#include "draw.h"
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <iostream>

CellState& operator++(CellState& state) {
	return state = static_cast<CellState>(state + 1);
}

CellState& operator--(CellState& state) {
	return state = static_cast<CellState>(state - 1);
}

SDL_Window *window = nullptr;
SDL_Renderer *renderer = nullptr;
TTF_Font *font = nullptr;

bool init_draw() {
	if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS) < 0) {
		std::cerr << "Failed to initialize SDL: "
			<< SDL_GetError() << std::endl;
		return 1;
	}

	if (SDL_CreateWindowAndRenderer(
		640,
		480,
		SDL_WINDOW_SHOWN,
		&window,
		&renderer
	)) {
		std::cerr << "Failed to create window and renderer: "
			<< SDL_GetError() << std::endl;
		return 1;
	}

	SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

	if (TTF_Init()) {
		std::cerr << "Failed to initialize TTF: "
			<< TTF_GetError() << std::endl;
		return 1;
	}
	font = TTF_OpenFont("assets/FreeSans.ttf", 24);
	if (font == nullptr) {
		std::cerr << "Failed to load font: "
			<< TTF_GetError() << std::endl;
		return 1;
	}

	return 0;
}

void quit_draw() {
	SDL_DestroyWindow(window);
	SDL_Quit();
	TTF_Quit();
}

void draw_text(const char *text, SDL_Rect* rect) {
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderFillRect(renderer, rect);

	if (text == nullptr || text[0] == '\0') {
		return;
	}

	SDL_Surface *surface = TTF_RenderText_Solid(font, text, SDL_Color{0x00, 0x00, 0x00});
	rect->w = surface->w;
	rect->h = surface->h;

	SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
	SDL_FreeSurface(surface);
	SDL_RenderCopy(renderer, texture, NULL, rect);
	SDL_DestroyTexture(texture);
}

bool draw_board(CellState board[10][10], bool show, int x, int y) {
	const int PADDING = 2;
	const int CELL_SIZE = 24;
	const int SEA_COLOR[2][3] = {{0x20, 0x20, 0xaa}, {0x40, 0x40, 0xaa}};

	SDL_Rect back = {
		x, y, 
		CELL_SIZE * 10 + PADDING * 11,
		CELL_SIZE * 10 + PADDING * 11
	};

	SDL_SetRenderDrawColor(renderer, 0x20, 0x20, 0xaa, 0x80);
	SDL_RenderFillRect(renderer, &back);

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			if (board[i][j] % 2) {
				SDL_Rect outline = {
					x + j * (CELL_SIZE + PADDING),
					y + i * (CELL_SIZE + PADDING),
					CELL_SIZE + 2 * PADDING,
					CELL_SIZE + 2 * PADDING
				};

				SDL_SetRenderDrawColor(renderer, 0xFF, 0x00, 0x30, 0x80);
				SDL_RenderFillRect(renderer, &outline);
			}

			SDL_Rect cell = {
				x + j * (CELL_SIZE + PADDING) + PADDING,
				y + i * (CELL_SIZE + PADDING) + PADDING,
				CELL_SIZE,
				CELL_SIZE
			};

			unsigned char r;
			unsigned char g;
			unsigned char b;

			switch (board[i][j] / 2 * 2) {
				case EMPTY:
					r = SEA_COLOR[(i + j) % 2][0];
					g = SEA_COLOR[(i + j) % 2][1];
					b = SEA_COLOR[(i + j) % 2][2];
					break;
				case FULL:
					if (show) {
						r = 0x00; g = 0x00; b = 0x00;
					} else {
						r = SEA_COLOR[(i + j) % 2][0];
						g = SEA_COLOR[(i + j) % 2][1];
						b = SEA_COLOR[(i + j) % 2][2];
					}
					break;
				case HIT:
					r = 0x00; g = 0xFF; b = 0x00;
					break;
				case MISS:
					r = 0xFF; g = 0x00; b = 0x00;
					break;
				default:
					r = 0xFF; g = 0xFF; b = 0xFF;
					break;
			}
			
			SDL_SetRenderDrawColor(renderer, r, g, b, 0xFF);
			SDL_RenderFillRect(renderer, &cell);
		}
	}

	return 0;
}

void draw_frame(Game *game) {
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(renderer);

	draw_board(game->board1, true, 300, 10);
	draw_board(game->board2, false, 10, 10);

	draw_text(game->text1, &game->text1_rect);
	draw_text(game->text2, &game->text2_rect);

	SDL_RenderPresent(renderer);
}
