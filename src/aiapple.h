#pragma once
#include <raylib.h>

#include "apple.h"
#include "cbase.h"
#include "color.h"
class CAIApple : public CApple {
   public:
	int x, y;
	float moveTimer = 0.0f;
	CAIApple(int x, int y);
	void CheckCollision() override;
	void Move() override;
	~CAIApple() override = default;
	void Draw() override;
};
