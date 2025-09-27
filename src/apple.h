#pragma once
#include <raylib.h>

#include "cbase.h"
#include "color.h"
class CApple : public CBaseEntity {
   public:
	int x, y;
	float moveTimer = 0.0f;
	CApple(int x, int y);
	void CheckCollision() override;
	virtual void Move() override;
	~CApple() override = default;
	virtual void Draw() override;
};

typedef enum {
	APPLE_UP = 0x0001,
	APPLE_DOWN = 0x0002,
	APPLE_LEFT = 0x0004,
	APPLE_RIGHT = 0x0008
} APPLE_MOVEKEYS;

extern int appleKeysPressed;
