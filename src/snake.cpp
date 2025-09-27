#include "snake.h"

#include <raylib.h>

#include <climits>
#include <cmath>
#include <cstdlib>
#include <queue>
#include <unordered_map>
#include <unordered_set>

#include "aiapple.h"
#include "apple.h"
#include "cbase.h"
#include "globals.h"
#include "world.h"

template void NewEntity<CSnake>();

bool isOccupied(const Point& p,
				const std::vector<std::unique_ptr<CBaseEntity>>& entities) {
	for (auto& e : entities) {
		if (e->x == p.x && e->y == p.y) return true;
	}
	return false;
}

inline int heuristic(const Point& a, const Point& b) {
	int dx = b.x - a.x;
	int dy = b.y - a.y;
	float dist = dx * dx + dy * dy;	 // squared Euclidean, avoids sqrt
	return floor(dist);
}

std::deque<Point> reconstructPath(
	const std::unordered_map<Point, Point, PointHash>& cameFrom,
	const Point& current) {
	std::deque<Point> totalPath;
	Point curr = current;
	while (cameFrom.count(curr)) {
		totalPath.push_front(curr);
		curr = cameFrom.at(curr);
	}
	return totalPath;
}

CApple* findClosestApple(
	int snakeX, int snakeY,
	const std::vector<std::unique_ptr<CBaseEntity>>& entities) {
	CApple* closest = nullptr;
	int minDist = INT_MAX;

	for (auto& e : entities) {
		if (auto apple = dynamic_cast<CApple*>(e.get())) {
			int dist =
				abs(apple->x - snakeX) + abs(apple->y - snakeY);  // Manhattan
			if (dist < minDist) {
				minDist = dist;
				closest = apple;
			}
		}
	}

	return closest;
}

// Compute A* path
std::deque<Point> computeAStar(
	int startX, int startY, int goalX, int goalY,
	const std::vector<std::unique_ptr<CBaseEntity>>& entities, CSnake* snake) {
	Point start{startX, startY};
	Point goal{goalX, goalY};

	// min-heap that compares only the fScore (int)
	using PQItem = std::pair<int, Point>;
	auto cmp = [](const PQItem& a, const PQItem& b) {
		return a.first > b.first;
	};
	std::priority_queue<PQItem, std::vector<PQItem>, decltype(cmp)> openSet(
		cmp);

	openSet.push({0, start});

	std::unordered_map<Point, Point, PointHash> cameFrom;
	std::unordered_map<Point, int, PointHash> gScore;
	std::unordered_set<Point, PointHash> closedSet;

	gScore[start] = 0;

	while (!openSet.empty()) {
		Point current = openSet.top().second;
		openSet.pop();

		// skip if we've already processed this node
		if (closedSet.find(current) != closedSet.end()) continue;
		closedSet.insert(current);

		if (current == goal) return reconstructPath(cameFrom, current);

		for (auto [dx, dy] : std::vector<std::pair<int, int>>{
				 {-1, 0}, {1, 0}, {0, -1}, {0, 1}}) {
			Point neighbor{current.x + dx, current.y + dy};

			// bounds check (use your actual grid size)
			if (neighbor.x < 0 || neighbor.y < 0 || neighbor.x >= 20 ||
				neighbor.y >= 20)
				continue;

			// avoid tail segments
			bool collides = false;
			for (auto& t : snake->tails) {
				if (neighbor.x == t.x && neighbor.y == t.y) {
					collides = true;
					break;
				}
			}
			if (collides) continue;

			// allow stepping onto the goal even if it's marked occupied
			if (neighbor == goal) {
				// ok — treat as walkable
			} else if (isOccupied(neighbor, entities)) {
				continue;
			}

			int tentativeG = gScore[current] + 1;
			if (!gScore.count(neighbor) || tentativeG < gScore[neighbor]) {
				cameFrom[neighbor] = current;
				gScore[neighbor] = tentativeG;
				int fScore = tentativeG + heuristic(neighbor, goal);
				openSet.push({fScore, neighbor});
			}
		}
	}

	return {};	// no path
}

void CSnake::CheckCollision() {
	Point snake = {x, y};

	Point apple = {-2, -2};
	CApple* closestApple = findClosestApple(x, y, entities);
	if (dynamic_cast<CAIApple*>(closestApple)) {
		if (closestApple) {
			apple = {closestApple->x, closestApple->y};
			if (apple == snake) {
				closestApple->x = 19;
				closestApple->y = 19;
				x = 0;
				y = 0;
				tails.clear();
			}
		}
	} else {
		if (closestApple) {
			apple = {closestApple->x, closestApple->y};
			if (apple == snake) {
				world::EndGame(WIN_SNAKE);
			}
		}

		for (Point tail : tails) {
			if (snake == tail) {
				world::EndGame(WIN_APPLE);
			}
			if (apple == tail) {
				world::EndGame(WIN_SNAKE);
			}
		}
	}

	if (score > highscore) {
		highscore = score;
	}
}

void CSnake::Move() {
	growTimer += GetFrameTime();
	moveTimer += GetFrameTime();
	if (!dead) {
		score += GetFrameTime();
	}

	if (growTimer > 5.0f) {
		growTimer -= 5.0f;
		tails.push_back({-2, -2});
	}

	if (moveTimer > 0.15f) {
		moveTimer -= 0.15f;

		if (tails.size() > 0) {
			Point front = tails.front();
			front.x = x;
			front.y = y;

			tails.erase(tails.begin());
			tails.push_back(front);
		}

		CApple* target = findClosestApple(x, y, entities);

		if (!target) return;

		if (path.empty() || path.back().x != target->x ||
			path.back().y != target->y) {
			path = computeAStar(x, y, target->x, target->y, entities, this);
		}

		if (!path.empty()) {
			Point next = path.front();
			x = next.x;
			y = next.y;
			path.pop_front();
		}
	}
}

void CSnake::Draw() {
	DefaultDraw(x, y, SNAKE_COL);
	for (Point point : tails) {
		DefaultDraw(point.x, point.y, SNAKE_COL);
	}
	// for (Point point : path) {
	// 	DefaultDraw(point.x, point.y, {255, 0, 0, 64});
	// }
}
