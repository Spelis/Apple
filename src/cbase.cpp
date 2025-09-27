#include "cbase.h"

#include <memory>

#include "world.h"

std::vector<std::unique_ptr<CBaseEntity>> entities;

CBaseEntity::~CBaseEntity() = default;

void CBaseEntity::Move() {}
void CBaseEntity::CheckCollision() {}
void CBaseEntity::Draw() {}

void DefaultDraw(int x, int y, Color col) {
	DrawRectangle(x * WORLD_TILESIZE, y * WORLD_TILESIZE, WORLD_TILESIZE,
				  WORLD_TILESIZE, col);
}

template void NewEntity<CBaseEntity>();
