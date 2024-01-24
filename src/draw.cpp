#include "draw.h"
#include "main.h"
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL_video.h>
#include <SDL2/SDL_image.h>
#include <cstddef>
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

SDL_Texture *background = nullptr;
SDL_Texture *ship_textures[SHIP_COUNT];

void loadImages() {
	background = SDL_CreateTextureFromSurface(renderer, IMG_Load("assets/bg.png"));

	for (int i = 0; i < SHIP_COUNT; i++) {
		ship_textures[i] = SDL_CreateTextureFromSurface(
			renderer, 
			IMG_Load(("assets/ship" + std::to_string(i) + ".png").c_str())
		);
	}
}

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

	if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
		std::cerr << "Failed to initialize SDL_image: "
			<< IMG_GetError() << std::endl;
		return 1;
	}

	loadImages();

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

bool draw_board(CellState board[10][10], ShipPos ships[SHIP_COUNT], bool show, int x, int y) {
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
						r = 0x40; g = 0x40; b = 0x40;
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
			
			SDL_SetRenderDrawColor(renderer, r, g, b, 0x40);
			SDL_RenderFillRect(renderer, &cell);
		}
	}

	for (int i = 0; i < SHIP_COUNT; i++) {
		if (ships[i].x == -1) continue;

		bool distroyed = true;
		for (int j = 0; j < SHIPS[i]; j++) {
			CellState cell = (ships[i].vertical) ?
				board[ships[i].y + j][ships[i].x] :
				board[ships[i].y][ships[i].x + j];
			if (cell / 2 * 2 != HIT) {
				distroyed = false;
				break;
			}
		}

		if (!show && !distroyed) continue;

		int width = SHIPS[i];
		int height = 1;

		SDL_Rect ship = {
			x + ships[i].x * (CELL_SIZE + PADDING) + PADDING,
			y + ships[i].y * (CELL_SIZE + PADDING) + PADDING,
			width * CELL_SIZE,
			height * CELL_SIZE
		};
		SDL_Point center = {
			CELL_SIZE / 2,
			CELL_SIZE / 2
		};


		SDL_RenderCopyEx(
			renderer, ship_textures[i], NULL, &ship,
			ships[i].vertical ? 90 : 0,
			&center, (i % 2) ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE
		);
	}

	return 0;
}

void draw_frame(Game *game) {
	SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
	SDL_RenderClear(renderer);
	SDL_RenderCopy(renderer, background, NULL, NULL);

	draw_board(game->board1, game->ships1, true, 300, 10);
	draw_board(game->board2, game->ships2, false, 10, 10);

	draw_text(game->text1, &game->text1_rect);
	draw_text(game->text2, &game->text2_rect);

	SDL_RenderPresent(renderer);
}
