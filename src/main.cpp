#include <cstdint>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <ios>

#include "apple.h"
#include "color.h"

#define NO_FONT_AWESOME

#include <raylib.h>

#include "cbase.h"
#include "globals.h"
#include "imgui/imgui.h"
#include "imgui/rlImGui.h"
#include "world.h"

#define KEYBIND(name, label, defaultKey) \
	struct {                             \
		int key = defaultKey;            \
		bool rebinding = false;          \
		const char* title = label;       \
	} name

#define KEYBIND_UI(bind)                                                  \
	do {                                                                  \
		ImGui::Text("%s: %s", bind.title, GetKeyName(bind.key));          \
		ImGui::SameLine();                                                \
		if (!bind.rebinding) {                                            \
			if (ImGui::Button(("Rebind##" #bind))) bind.rebinding = true; \
		} else {                                                          \
			if (ImGui::Button("...")) bind.rebinding = false;             \
			for (int k = 0; k < 512; ++k) {                               \
				if (IsKeyPressed(k)) {                                    \
					bind.key = k;                                         \
					bind.rebinding = false;                               \
				}                                                         \
			}                                                             \
		}                                                                 \
	} while (0)

bool running = true;
bool paused = false;
bool justStartedGame = true;
bool dead = false;
WIN_TYPE whoWon = WIN_NONE;
float highscore = 0.0f;
float score = 0.0f;
int frame = 0;

KEYBIND(bind_up, "Up", KEY_W);
KEYBIND(bind_down, "Down", KEY_S);
KEYBIND(bind_left, "Left", KEY_A);
KEYBIND(bind_right, "Right", KEY_D);

bool settings_ui = false;

bool isPaused() { return (paused); }

std::filesystem::path getSaveFileLocation() {
#ifdef _WIN32
	const char* base = std::getenv("APPDATA");
	return std::filesystem::path(base ? base : ".") / "apple.bin";
#elif __APPLE__
	const char* home = std::getenv("HOME");
	return std::filesystem::path(home ? home : ".") /
		   "Library/Application Support/apple.bin";
#else
	const char* home = std::getenv("HOME");
	return std::filesystem::path(home ? home : ".") / ".config/apple.bin";
#endif
}

void SaveSave() {
	SaveStruct s{"APL",			 version,		(unsigned int)(highscore * 100),
				 bind_up.key,	 bind_down.key, bind_left.key,
				 bind_right.key, WORLD_TILESIZE};
	std::ofstream out(getSaveFileLocation(), std::ios::binary);
	out.write(reinterpret_cast<const char*>(&s), sizeof(s));
	out.close();
}

void LoadSave() {
	SaveStruct s;
	std::ifstream in(getSaveFileLocation(), std::ios::binary);
	in.read(reinterpret_cast<char*>(&s), sizeof(s));
	in.close();
	if (s.version != version || strcmp(s.magic, "APL\0")) return;
	highscore = ((float)s.score) / 100;
	bind_up.key = s.bind_up;
	bind_down.key = s.bind_down;
	bind_left.key = s.bind_left;
	bind_right.key = s.bind_right;
	WORLD_TILESIZE = s.tilesize;
}

int main() {
	SetConfigFlags(FLAG_WINDOW_RESIZABLE);
	InitWindow(800, 600, "Apple");
	SetExitKey(0);
	SetTargetFPS(60);
	rlImGuiSetup(true);
	LoadSave();

	RenderTexture gameTexture =
		LoadRenderTexture(WORLD_TILESIZE * 20, WORLD_TILESIZE * 20);

	world::InitWorld(false);

	ImGuiStyle& style = ImGui::GetStyle();

	style.FrameRounding = 3.0f;
	style.WindowRounding = 3.0f;
	style.GrabRounding = 0.0f;

	while (!WindowShouldClose() && running) {
		BeginDrawing();
		ClearBackground(BG);
		BeginTextureMode(gameTexture);
		ClearBackground(BG);
		world::DrawBackground();
		for (auto& ent : entities) {
			if (!isPaused()) {
				ent->Move();
				ent->CheckCollision();
			}
			ent->Draw();
		}
		EndTextureMode();

		// the reason this doesn't use isPaused is cause you know... it's
		// already paused
		if (IsKeyPressed(KEY_ESCAPE) && !(justStartedGame || dead)) {
			paused = !paused;
		}

		if (!(dead || justStartedGame || paused)) {
			if (IsKeyDown(bind_up.key)) {
				appleKeysPressed |= APPLE_UP;
			}
			if (IsKeyDown(bind_down.key)) {
				appleKeysPressed |= APPLE_DOWN;
			}
			if (IsKeyDown(bind_left.key)) {
				appleKeysPressed |= APPLE_LEFT;
			}
			if (IsKeyDown(bind_right.key)) {
				appleKeysPressed |= APPLE_RIGHT;
			}
		}

		rlImGuiBegin();
		if ((paused || justStartedGame)) {
			ImGui::Begin("Menu", &running,
						 ImGuiWindowFlags_AlwaysAutoResize |
							 ImGuiWindowFlags_NoCollapse);

			if ((!justStartedGame && !dead) || paused) {
				if (ImGui::Button("Resume Game")) {
					paused = false;
				}
			}
			if (ImGui::Button("New Game")) {
				justStartedGame = false;
				dead = false;
				paused = false;
				whoWon = WIN_NONE;
				score = 0.0f;
				world::InitWorld(true);
			}
			if (ImGui::Button("Settings")) {
				settings_ui = true;
			}
			ImGui::Text("Highscore: %.2fs", highscore);

			ImGui::End();
		}

		if (settings_ui) {
			ImGui::Begin("Settings", &settings_ui, 0);
			KEYBIND_UI(bind_up);
			KEYBIND_UI(bind_down);
			KEYBIND_UI(bind_left);
			KEYBIND_UI(bind_right);
			if (ImGui::SliderInt("Tile Size", &WORLD_TILESIZE, 1, 64)) {
				UnloadRenderTexture(gameTexture);
				gameTexture =
					LoadRenderTexture(WORLD_TILESIZE * 20, WORLD_TILESIZE * 20);
			}

			ImGui::End();
		}

		if (whoWon != WIN_NONE && dead == true) {
			ImGui::Begin("Game Over", NULL,
						 ImGuiWindowFlags_AlwaysAutoResize |
							 ImGuiWindowFlags_NoCollapse);

			if (whoWon == WIN_SNAKE) {
				ImGui::Text("You have lost.");
			} else if (whoWon == WIN_APPLE) {
				ImGui::Text("You have won!");
			}

			if (ImGui::Button("Ok")) {
				dead = false;
				whoWon = WIN_NONE;
				justStartedGame = true;
				world::KillAll();
				world::InitWorld(false);
			}

			ImGui::End();
		}

		ImGui::Begin(
			"Apple", NULL,
			ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoCollapse);

		ImGui::Image(
			(void*)(intptr_t)gameTexture.texture.id,
			ImVec2(gameTexture.texture.width, gameTexture.texture.height),
			ImVec2(0, 1), ImVec2(1, 0));

		ImGui::Text("Score: %.2fs", score);

		ImGui::End();

		rlImGuiEnd();
		EndDrawing();
		frame++;
	}

	SaveSave();
	world::KillAll();
	ImGui::DestroyContext();
	CloseWindow();
}
