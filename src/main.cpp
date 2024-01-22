#include <SDL2/SDL.h>
#include <SDL2/SDL_rect.h>
#include <SDL2/SDL_render.h>
#include <SDL2/SDL_ttf.h>
#include <cstdio>
#include <ctime>
#include <iostream>
#include "draw.h"
#include "main.h"

const int SHIPS[] = {5, 4, 4, 3, 3, 2, 2, 2};

Game game;
int score1, score2, score_sum;

bool init() {
	if (init_draw()) return 1;

	for (int i = 0; i < 10; i++) {
		for (int j = 0; j < 10; j++) {
			game.board1[i][j] = EMPTY;
			game.board2[i][j] = EMPTY;
		}
	}

	for (int i = 0; i < sizeof(SHIPS) / sizeof(SHIPS[0]); i++) {
		score_sum += SHIPS[i];
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
			--game.board1[y + i][x + j];
		}
	}
	switch (k) {
		case SDLK_SPACE:
			if (place_ship(x, y, width, height, game.board1)) {
				break;
			}
			ship++;
			if (ship >= sizeof(SHIPS) / sizeof(SHIPS[0])) {
				draw_frame(&game);
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
			++game.board1[y + i][x + j];
		}
	}

	draw_frame(&game);
}

int handle_attack(int x, int y, CellState board[10][10], int &score) {
	switch (board[y][x]) {
		case EMPTY:
			board[y][x] = MISS;
		break;
		case FULL:
			board[y][x] = HIT;
			score++;
			return 2;
		break;
		default:
			return 1;
		break;
	}

	return 0;
}

void enemy_attack() {
	int x = rand() % 10;
	int y = rand() % 10;
	int res;
	do {
		res = handle_attack(x, y, game.board1, score2);
		draw_frame(&game);
		if (res == 2) {
			x = rand() % 10;
			y = rand() % 10;
		} else if (res == 1) {
			int next = rand() % 4;
			switch (next) {
				case 0:
					if (x > 0) x--;
				break;
				case 1:
					if (x < 10 - 1) x++;
				break;
				case 2:
					if (y > 0) y--;
				break;
				case 3:
					if (y < 10 - 1) y++;
				break;
				default:
				break;
			}

			SDL_Delay(100);
		}
	} while (res);
}

void handle_stage2_input(int& x, int& y, SDL_Keycode k) {
	--game.board2[y][x];
	switch (k) {
		case SDLK_SPACE:
			if (handle_attack(x, y, game.board2, score1)) {
				break;
			}
			enemy_attack();
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
	++game.board2[y][x];

	sprintf(game.text1, "%d/%d", score1, score_sum);
	sprintf(game.text2, "%d/%d", score2, score_sum);

	draw_frame(&game);
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
				width, height, game.board2
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
		++game.board1[cursor.y][cursor.x + i];
	}

	while (ship < sizeof(SHIPS) / sizeof(SHIPS[0])) {
		sprintf(
			game.text1, "Ship %d/%lu",
			ship + 1, sizeof(SHIPS) / sizeof(SHIPS[0])
		);
		draw_frame(&game);
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
	++game.board2[cursor.y][cursor.x];

	sprintf(game.text1, "%d/%d", score1, score_sum);
	sprintf(game.text2, "%d/%d", score2, score_sum);

	draw_frame(&game);

	while (score1 < score_sum && score2 < score_sum) {
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

	sprintf(game.text2, " ");
	if (score1 == score_sum) {
		sprintf(game.text1, "YOU WIN!");
	} else {
		sprintf(game.text1, "YOU LOSE!");
	}
	draw_frame(&game);

	while (true) {
		SDL_Event e;
		if (SDL_WaitEvent(&e)) {
			if (e.type == SDL_QUIT) {
				return 0;
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
