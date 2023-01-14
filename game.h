#pragma once
#include"draw.h"
#include"myvector.h"
#include"object.h"
#include"constansts.h"

class Game
{
	public:
		Game();
		~Game();

		void Init(const char* title, int xpos, int ypos, bool fullscreen);
		void Events();
		void Time();
		void Render();
		void CleanSurfaces();
		bool Running();
		void Pause();
		void InstructionWindow();
		void RoadLines();
		void Colisions();
		void PlayerLostLife();
		void ColorsInit();
		void GameIsPausedWindow();
		void DrawTrees();
		void StartNewGame();
		void GameEnd();
		void Save();
		void Load(char*);
		void LoadSaveNames();
		void LoadRender();

		

	private:
		bool lost_life, game_pause, offroad, is_running, game_end, load;
		int rc, t1, t2, frames, road_line_offset, lifes, current_save_number;
		int black, green, red, blue, grey, white, brown;
		float ftest;
		double delta, worldTime, fpsTimer, fps, total_distance, player_speed;
		SDL_Event event;
		Myvector<char*> save_names;
		Myvector<SDL_Surface*> static_surfaces;
		Object player_car;
		Object trees[NUMBER_OF_TREES];
		SDL_Texture* scrtex;
		SDL_Window* window;
		SDL_Renderer* renderer;

};

