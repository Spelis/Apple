#pragma once

#include <cstdint>
#include <functional>

#include "world.h"

#pragma pack(push, 1)
struct SaveStruct {
	char magic[4];	// "APL\0"
	uint32_t version;
	uint32_t score;
	int bind_up;
	int bind_down;
	int bind_left;
	int bind_right;
	int tilesize;
};
#pragma pack(pop)

inline unsigned int version = 0x03;

extern int frame;
extern bool dead;
extern WIN_TYPE whoWon;
extern float highscore;
extern float score;

extern void SaveSave();
extern void LoadSave();

struct Point {
	int x, y;

	bool operator==(const Point& other) const {
		return x == other.x && y == other.y;
	}

	bool operator<(const Point& other) const {
		return std::tie(x, y) < std::tie(other.x, other.y);
	}
};

struct PointHash {
	std::size_t operator()(const Point& p) const {
		return std::hash<int>()(p.x) ^ (std::hash<int>()(p.y) << 1);
	}
};
