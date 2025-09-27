#include "apple.h"

#include <raylib.h>

#include <algorithm>

int appleKeysPressed = 0;

CApple::CApple(int x, int y) {
	this->x = x;
	this->y = y;
}

template void NewEntity<CApple, int, int>(int&&, int&&);

void CApple::CheckCollision() {}

void CApple::Draw() { DefaultDraw(x, y, APPLE_COL); }

void CApple::Move() {
	moveTimer += GetFrameTime();
	if (moveTimer > 0.1f) {
		if (appleKeysPressed & APPLE_UP) {
			y--;
		}
		if (appleKeysPressed & APPLE_DOWN) {
			y++;
		}
		if (appleKeysPressed & APPLE_LEFT) {
			x--;
		}
		if (appleKeysPressed & APPLE_RIGHT) {
			x++;
		}

		x = std::max(std::min(x, 14), 0);
		y = std::max(std::min(y, 14), 0);

		appleKeysPressed = 0;
		moveTimer -= 0.1f;
	}
}
