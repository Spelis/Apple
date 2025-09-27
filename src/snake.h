#pragma once
#include <raylib.h>

#include <deque>
#include <vector>

#include "cbase.h"
#include "color.h"
#include "globals.h"

class CSnake : public CBaseEntity {
   public:
	int x, y;
	float growTimer;
	float moveTimer;
	CSnake() = default;
	void CheckCollision() override;
	void Move() override;
	void Draw() override;
	~CSnake() override = default;
	Color entityColor = SNAKE_COL;
	std::vector<Point> tails;
	std::deque<Point> path;
};
