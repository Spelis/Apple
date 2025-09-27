#include "world.h"

#include <raylib.h>

#include <cmath>

#include "aiapple.h"
#include "apple.h"
#include "cbase.h"
#include "color.h"
#include "globals.h"
#include "snake.h"

void world::DrawBackground() {
	for (int col = 0; col < 20; col++) {
		for (int row = 0; row < 20; row++) {
			if ((col + row) % 2 == 1) {
				DrawRectangle(col * WORLD_TILESIZE, row * WORLD_TILESIZE,
							  WORLD_TILESIZE, WORLD_TILESIZE, BG2);
			}
		}
	}
}

void world::InitWorld(bool playable) {
	world::KillAll();

	if (playable) {
		NewEntity<CApple>(19, 19);
	} else {
		NewEntity<CAIApple>(19, 19);
		dead = true;
	}
	NewEntity<CSnake>();
}

void world::KillAll() {
	entities.clear();
	entities.shrink_to_fit();
}

void world::EndGame(WIN_TYPE winner) {
	whoWon = winner;
	dead = true;
	SaveSave();
}
