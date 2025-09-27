#include "aiapple.h"

#include <raylib.h>

#include <algorithm>
#include <queue>

#include "apple.h"
#include "color.h"
#include "globals.h"
#include "snake.h"

CAIApple::CAIApple(int x, int y) : CApple(x, y) {
	this->x = x;
	this->y = y;
}

CSnake* getFirstSnake(
	const std::vector<std::unique_ptr<CBaseEntity>>& entities) {
	for (auto& e : entities) {
		if (auto snake = dynamic_cast<CSnake*>(e.get())) {
			return snake;
		}
	}
	return nullptr;	 // no snake found
}

Point findEscapeTile(CAIApple* apple, CSnake* snake, int maxDepth = 5) {
	struct Node {
		Point pos;
		int depth;
	};
	std::queue<Node> q;
	std::unordered_map<Point, Point, PointHash> cameFrom;

	q.push({{apple->x, apple->y}, 0});
	cameFrom[{apple->x, apple->y}] = {-1, -1};

	Point best = {apple->x, apple->y};
	int maxDist = 0;

	while (!q.empty()) {
		Node cur = q.front();
		q.pop();

		int distToSnake = abs(cur.pos.x - snake->x) + abs(cur.pos.y - snake->y);
		if (distToSnake > maxDist) {
			maxDist = distToSnake;
			best = cur.pos;
		}

		if (cur.depth >= maxDepth) continue;

		for (auto [dx, dy] : std::vector<std::pair<int, int>>{
				 {-1, 0}, {1, 0}, {0, -1}, {0, 1}}) {
			Point next{cur.pos.x + dx, cur.pos.y + dy};

			if (next.x < 0 || next.y < 0 || next.x >= 20 || next.y >= 20)
				continue;

			// avoid snake head & tails
			if (next.x == snake->x && next.y == snake->y) continue;
			bool collidesTail = false;
			for (auto& t : snake->tails)
				if (next == t) collidesTail = true;
			if (collidesTail) continue;

			if (cameFrom.count(next)) continue;	 // already visited
			cameFrom[next] = cur.pos;
			q.push({next, cur.depth + 1});
		}
	}

	// backtrack 1 step from apple to move
	Point moveTo = best;
	while (!(cameFrom[moveTo] == Point{-1, -1}) &&
		   !(cameFrom[moveTo] == Point{apple->x, apple->y})) {
		moveTo = cameFrom[moveTo];
	}
	return moveTo;
}

template void NewEntity<CAIApple, int, int>(int&&, int&&);

void CAIApple::CheckCollision() {}

void CAIApple::Draw() { DefaultDraw(x, y, APPLE_COL); }

void CAIApple::Move() {
	moveTimer += GetFrameTime();
	if (moveTimer > 0.1f) {
		CSnake* snake = getFirstSnake(entities);

		if (!snake) return;

		Point best = findEscapeTile(this, snake);

		x = best.x;
		y = best.y;
		x = std::max(std::min(x, 19), 0);
		y = std::max(std::min(y, 19), 0);
		CApple::x = x;
		CApple::y = y;

		moveTimer -= 0.1f;
	}
}
