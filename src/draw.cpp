#include "draw.h"
#include <SDL2/SDL_pixels.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_surface.h>

bool draw_board(CellState board[10][10], SDL_Surface *screen, bool show, int x, int y) {
	const int PADDING = 2;
	const int CELL_SIZE = 24;
	const int SEA_COLOR[2][3] = {{0x20, 0x20, 0xaa}, {0x40, 0x40, 0xaa}};

	SDL_Rect back = {
		x, y, 
		CELL_SIZE * 10 + PADDING * 11,
		CELL_SIZE * 10 + PADDING * 11
	};

	SDL_FillRect(screen, &back, 
	      SDL_MapRGB(screen->format, 0x20, 0x20, 0xaa)
	);

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			if (board[i][j] % 2) {
				SDL_Rect outline = {
					x + i * (CELL_SIZE + PADDING),
					y + j * (CELL_SIZE + PADDING),
					CELL_SIZE + 2 * PADDING,
					CELL_SIZE + 2 * PADDING
				};

				SDL_FillRect(
					screen, &outline,
					SDL_MapRGB(screen->format, 0xFF, 0x00, 0x30)
				);
			}

			SDL_Rect cell = {
				x + i * (CELL_SIZE + PADDING) + PADDING,
				y + j * (CELL_SIZE + PADDING) + PADDING,
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
			
			SDL_FillRect(
				screen, &cell, 
				SDL_MapRGB(screen->format, r, g, b)
			);
		}
	}

	return 0;
}

