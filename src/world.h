#pragma once
inline int WORLD_TILESIZE = 16;

enum WIN_TYPE { WIN_NONE, WIN_APPLE, WIN_SNAKE };

namespace world {
void DrawBackground();
void KillAll();
void InitWorld(bool playable);
void EndGame(WIN_TYPE winner);
}  // namespace world
