#pragma once

#include <raylib.h>

#include <memory>
#include <vector>

class CBaseEntity {
   public:
	CBaseEntity() = default;
	virtual ~CBaseEntity();
	virtual void CheckCollision();
	virtual void Move();
	virtual void Draw();
	int x, y;
};

extern std::vector<std::unique_ptr<CBaseEntity>> entities;

void DefaultDraw(int x, int y, Color col);

template <typename T, typename... Args>
void NewEntity(Args&&... args) {
	entities.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
}
