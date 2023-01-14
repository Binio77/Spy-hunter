#include"game.h"
#include<string.h>
#include<math.h>
#include<stdio.h>
#include<time.h>

Game::Game() : is_running(true), frames(0), fpsTimer(0), fps(0),
worldTime(0), total_distance(0), player_speed(0), road_line_offset(0),
ftest(0),lifes(5), lost_life(false), game_pause(true), offroad(false), 
game_end(false), load(false) {};

Game::~Game()
{
	CleanSurfaces();
	SDL_DestroyTexture(scrtex);
	SDL_DestroyWindow(window);
	SDL_DestroyRenderer(renderer);
	SDL_Quit();
	for (int i = 0; i < save_names.Size(); i++)
		delete[] save_names.Get(i);
}

void Game::Init(const char* title, int xpos, int ypos, bool fullscreen)
{

	if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
	{
		printf("SDL_Init error: %s\n", SDL_GetError());
		is_running = false;
	}

	 //tryb pe³noekranowy / fullscreen mode
	//rc = SDL_CreateWindowAndRenderer(0, 0, SDL_WINDOW_FULLSCREEN_DESKTOP,
									// &window, &renderer);

	rc = SDL_CreateWindowAndRenderer(SCREEN_WIDTH, SCREEN_HEIGHT, 0,
		&window, &renderer);
	if (rc != 0)
	{
		SDL_Quit();
		printf("SDL_CreateWindowAndRenderer error: %s\n", SDL_GetError());
		is_running = false;
	}

	SDL_SetWindowTitle(window, "Spy Hunter");

	SDL_Surface *screen, *charset = nullptr;

	screen = SDL_CreateRGBSurface(0, SCREEN_WIDTH, SCREEN_HEIGHT, 32,
		0x00FF0000, 0x0000FF00, 0x000000FF, 0xFF000000);

	static_surfaces.Emplace_back(screen);

	scrtex = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
		SDL_TEXTUREACCESS_STREAMING,
		SCREEN_WIDTH, SCREEN_HEIGHT);

	SDL_ShowCursor(SDL_DISABLE);

	charset = SDL_LoadBMP("./cs8x8.bmp");
	if (charset == NULL)
	{
		printf("SDL_LoadBMP(cs8x8.bmp) error: %s\n", SDL_GetError());
		is_running = false;
	}
	else
	{
		static_surfaces.Emplace_back(charset);
		SDL_SetColorKey(static_surfaces.Get(1), true, 0x000000);
	}

	
	player_car.surface = SDL_LoadBMP("./car.bmp");
	if (player_car.surface == NULL)
	{
		printf("SDL_LoadBMP(car.bmp) error: %s\n", SDL_GetError());
		is_running = false;
	}
	else
	{
		player_car.x = SCREEN_WIDTH / 2;
		player_car.y = SCREEN_HEIGHT / 2;
	}

	Object tree;
	tree.surface = SDL_LoadBMP("./tree.bmp");
	if (tree.surface == NULL)
	{
		printf("SDL_LoadBMP(tree.bmp) error: %s \n");
		is_running = false;
	}
	else
	{
		for (int i = 0; i < NUMBER_OF_TREES; i++)
		{
			if(i % 2 == 0)
				tree.x = SCREEN_WIDTH / 12;
			else
				tree.x = SCREEN_WIDTH - SCREEN_WIDTH / 12;
			
			tree.y = SCREEN_HEIGHT / NUMBER_OF_TREES * i;
			trees[i] = tree;
		}
	}

	t1 = SDL_GetTicks();
	ColorsInit();
	LoadSaveNames();

}

void Game::ColorsInit()
{
	black = SDL_MapRGB(static_surfaces.Get(0)->format, 0x00, 0x00, 0x00);
	green = SDL_MapRGB(static_surfaces.Get(0)->format, 0x00, 0x80, 0x00);
	red = SDL_MapRGB(static_surfaces.Get(0)->format, 0xFF, 0x00, 0x00);
	blue = SDL_MapRGB(static_surfaces.Get(0)->format, 0x11, 0x11, 0xCC);
	grey = SDL_MapRGB(static_surfaces.Get(0)->format, 0x80, 0x80, 0x80);
	white = SDL_MapRGB(static_surfaces.Get(0)->format, 0xFF, 0xFF, 0xFF);
	brown = SDL_MapRGB(static_surfaces.Get(0)->format, 0x8B, 0x45, 0x13);
}

void Game::Time()
{
	t2 = SDL_GetTicks();
	delta = (t2 - t1) * 0.001;
	t1 = t2;
	if(!game_pause && !game_end)
		worldTime += delta;

	if(offroad == false)
		total_distance += player_speed * delta;

	fpsTimer += delta;

	if (fpsTimer > 0.5)
	{
		fps = frames * 2;
		frames = 0;
		fpsTimer -= 0.5;
	}
	offroad = false;
}


void Game::Events()
{
	const Uint8* keystates = SDL_GetKeyboardState(NULL);
	if (!load)
	{

	}
	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
		case SDL_KEYDOWN:
		{
			
			if (event.key.keysym.sym == SDLK_ESCAPE)
				is_running = false;
			else if (event.key.keysym.sym == SDLK_p)
				Pause();
			else if (event.key.keysym.sym == SDLK_n)
				StartNewGame();
			else if (event.key.keysym.sym == SDLK_f)
				GameEnd();
			else if (event.key.keysym.sym == SDLK_s)
				Save();
			else if (event.key.keysym.sym == SDLK_l)
			{
				load = true;
			}
			else if (event.key.keysym.sym == SDLK_DOWN && load && current_save_number < save_names.Size() - 1)
				current_save_number++;
			else if (event.key.keysym.sym == SDLK_UP && load && current_save_number > 0)
				current_save_number--;
			else if (keystates[SDL_SCANCODE_RETURN] && load)
				Load(save_names.Get(current_save_number));
			
			break;
		}
		case SDL_QUIT:
		{
			is_running = false;
			break;
		}
		}
	}

	if (!game_pause && !game_end && !load)
	{
		if (keystates[SDL_SCANCODE_UP] && !keystates[SDL_SCANCODE_DOWN])
		{
			player_speed = NORMAL_SPEED * 3;
			if (player_car.y > SCREEN_HEIGHT / 3)
				player_car.y -= player_speed / 2;
		}
		else if (keystates[SDL_SCANCODE_DOWN] && !keystates[SDL_SCANCODE_UP])
		{
			player_speed = NORMAL_SPEED * 0.3;
			if (player_car.y < SCREEN_HEIGHT / 3 * 2)
				player_car.y += player_speed * 2;
		}
		else
			player_speed = NORMAL_SPEED;

		if (keystates[SDL_SCANCODE_LEFT] && !keystates[SDL_SCANCODE_RIGHT])
			player_car.x -= 1;
		else if (keystates[SDL_SCANCODE_RIGHT] && !keystates[SDL_SCANCODE_LEFT])
			player_car.x += 1;

		Colisions();
	}

	frames++;
	
}

void Game::Render()
{
	if (!load)
	{
		SDL_FillRect(static_surfaces.Get(0), NULL, green);
		DrawRectangle(static_surfaces.Get(0), SCREEN_WIDTH / 12 * 3, 0, ROAD_WIDTH, SCREEN_HEIGHT, black, black);
		DrawRectangle(static_surfaces.Get(0), SCREEN_WIDTH / 12 * 2, 0, SHOULDER_WIDTH, SCREEN_HEIGHT, brown, brown);
		DrawRectangle(static_surfaces.Get(0), SCREEN_WIDTH / 12 * 9, 0, SHOULDER_WIDTH, SCREEN_HEIGHT, brown, brown);
		RoadLines();
		DrawSurface(static_surfaces.Get(0), player_car.surface, player_car.x, player_car.y);
		DrawTrees();

		if (game_pause)
			GameIsPausedWindow();

		if (game_end)
			GameEnd();

		InstructionWindow();
	}
	else
	{
		LoadRender();
	}
	
	SDL_RenderPresent(renderer);
}

void Game::DrawTrees()
{
	for (int i = 0; i < NUMBER_OF_TREES; i++)
	{
		if(!game_pause)
			trees[i].y += player_speed / 2;

		if (trees[i].y > SCREEN_HEIGHT)
			trees[i].y = 0;

		DrawSurface(static_surfaces.Get(0), trees[i].surface, trees[i].x, trees[i].y);
	}
}

void Game::Colisions()
{
	double current_speed = player_speed;
	if ((player_car.x + player_car.surface->w / 2) >= SCREEN_WIDTH / 12 * 10)
		lost_life = true;
	else if ((player_car.x - player_car.surface->w / 2) <= SCREEN_WIDTH / 12 * 2)
		lost_life = true;
	else if ((player_car.x + player_car.surface->w / 2) >= SCREEN_WIDTH / 12 * 9)
	{
		player_speed = 0.3;
		offroad = true;
	}
	else if ((player_car.x - player_car.surface->w / 2) <= SCREEN_WIDTH / 12 * 3)
	{
		player_speed = 0.3;
		offroad = true;
	}
	else
		player_speed = current_speed;

	if (lost_life)
		PlayerLostLife();
		
}


void Game::PlayerLostLife()
{
	lifes--;
	if (lifes > 0)
	{
		player_car.x = SCREEN_WIDTH / 2;
		lost_life = false;
	}
	else
	{
		GameEnd();
	}
}

void Game::RoadLines()
{
	
	ftest += 300 * player_speed / (fps + 1);

	road_line_offset = (int)ftest;
	int count_road_lines = 0;

	if (ftest > (ROAD_LINE_HEIGHT * 1.5))
		ftest -= ROAD_LINE_HEIGHT * 1.5;


	for (int current_road_line_offset = road_line_offset; current_road_line_offset < SCREEN_HEIGHT;
		current_road_line_offset += ROAD_LINE_HEIGHT + ROAD_LINE_HEIGHT / 2)
	{
		if (current_road_line_offset + ROAD_LINE_HEIGHT > SCREEN_HEIGHT)
		{
			DrawRectangle(static_surfaces.Get(0), SCREEN_WIDTH / 2 - (ROAD_LINE_WIDTH / 2),
				current_road_line_offset, ROAD_LINE_WIDTH,
				ROAD_LINE_HEIGHT - (current_road_line_offset + ROAD_LINE_HEIGHT - SCREEN_HEIGHT), white, white);
		}
		else
		{
			DrawRectangle(static_surfaces.Get(0), SCREEN_WIDTH / 2 - (ROAD_LINE_WIDTH / 2),
				current_road_line_offset, ROAD_LINE_WIDTH, ROAD_LINE_HEIGHT, white, white);
		}

	}
}

void Game::InstructionWindow()
{
	char text[128];

	DrawRectangle(static_surfaces.Get(0), 4, 4, static_surfaces.Get(0)->w - 8, SCREEN_HEIGHT / 12, red, blue);
	sprintf(text, "Artur Binczyk 193138");
	DrawString(static_surfaces.Get(0), static_surfaces.Get(0)->w / 2 - strlen(text) * 8 / 2, 10, text, static_surfaces.Get(1));
	sprintf(text, "Game duration = %.1lf s  %.0lf points", worldTime, ceil(total_distance * 100));
	DrawString(static_surfaces.Get(0), static_surfaces.Get(0)->w / 2 - strlen(text) * 8 / 2, 26, text, static_surfaces.Get(1));

	DrawRectangle(static_surfaces.Get(0), SCREEN_WIDTH / 12 * 10 + 10, SCREEN_HEIGHT / 4 * 3, SCREEN_WIDTH / 12 * 2 -10, SCREEN_HEIGHT / 4 - 10, red, blue);
	sprintf(text, "arr-move");
	DrawString(static_surfaces.Get(0),  SCREEN_WIDTH / 12 * 10 + 12, SCREEN_HEIGHT / 4 * 3 + 1, text, static_surfaces.Get(1));
	sprintf(text, "n-new game");
	DrawString(static_surfaces.Get(0), SCREEN_WIDTH / 12 * 10 + 12, SCREEN_HEIGHT / 4 * 3 + 11, text, static_surfaces.Get(1));
	sprintf(text, "p-pause");
	DrawString(static_surfaces.Get(0), SCREEN_WIDTH / 12 * 10 + 12, SCREEN_HEIGHT / 4 * 3 + 21, text, static_surfaces.Get(1));
	sprintf(text, "f-end game");
	DrawString(static_surfaces.Get(0), SCREEN_WIDTH / 12 * 10 + 12, SCREEN_HEIGHT / 4 * 3 + 31, text, static_surfaces.Get(1));
	sprintf(text, "ESC-leave");
	DrawString(static_surfaces.Get(0), SCREEN_WIDTH / 12 * 10 + 12, SCREEN_HEIGHT / 4 * 3 + 41, text, static_surfaces.Get(1));
	sprintf(text, "s-save");
	DrawString(static_surfaces.Get(0), SCREEN_WIDTH / 12 * 10 + 12, SCREEN_HEIGHT / 4 * 3 + 51, text, static_surfaces.Get(1));
	sprintf(text, "l-load");
	DrawString(static_surfaces.Get(0), SCREEN_WIDTH / 12 * 10 + 12, SCREEN_HEIGHT / 4 * 3 + 61, text, static_surfaces.Get(1));

	SDL_UpdateTexture(scrtex, NULL, static_surfaces.Get(0)->pixels, static_surfaces.Get(0)->pitch);
	SDL_RenderCopy(renderer, scrtex, NULL, NULL);

}

void Game::CleanSurfaces()
{
	for (int i = 0; i < static_surfaces.Size(); i++)
	{
		SDL_FreeSurface(static_surfaces.Get(i));
	}

	for (int i = 0; i < NUMBER_OF_TREES; i++)
	{
		SDL_FreeSurface(trees[i].surface);
	}
}


bool Game::Running()
{
	return is_running;
}

void Game::Pause()
{
	if (game_pause)
	{
		game_pause = false;
		player_speed = NORMAL_SPEED;

	}
	else
	{
		game_pause = true;
		player_speed = 0;
	}
}

void Game::GameIsPausedWindow()
{
	char text[128];
	DrawRectangle(static_surfaces.Get(0), 4, SCREEN_HEIGHT / 2 - 15, static_surfaces.Get(0)->w - 8, SCREEN_HEIGHT / 12, red, blue);
	sprintf(text, "Game is Paused press p to unpause");
	DrawString(static_surfaces.Get(0), static_surfaces.Get(0)->w / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 2, text, static_surfaces.Get(1));
	SDL_UpdateTexture(scrtex, NULL, static_surfaces.Get(0)->pixels, static_surfaces.Get(0)->pitch);
	SDL_RenderCopy(renderer, scrtex, NULL, NULL);
}

void Game::StartNewGame()
{
	game_end = false;
	player_car.x = SCREEN_WIDTH / 2;
	player_car.y = SCREEN_HEIGHT / 2;
	total_distance = 0;
	worldTime = 0;
	lifes = 5;
	if(!game_pause)
		Pause();

}

void Game::GameEnd()
{
	game_end = true;
	char text[128];
	player_speed = 0;
	DrawRectangle(static_surfaces.Get(0), 4, SCREEN_HEIGHT / 2 - 15, static_surfaces.Get(0)->w - 8, SCREEN_HEIGHT / 12, red, blue);
	sprintf(text, "Game ended! Your result: %i", (int)(ceil(total_distance * 100)));
	DrawString(static_surfaces.Get(0), static_surfaces.Get(0)->w / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 2, text, static_surfaces.Get(1));
	SDL_UpdateTexture(scrtex, NULL, static_surfaces.Get(0)->pixels, static_surfaces.Get(0)->pitch);
	SDL_RenderCopy(renderer, scrtex, NULL, NULL);
}

void Game::Save()
{	
	time_t mytime = time(NULL);
	char* file_name = ctime(&mytime);
	for (int i = 0; i < strlen(file_name); i++)
	{
		if (file_name[i] == ':' || file_name[i] == ' ')
			file_name[i] = '.';
	}
	file_name[strlen(file_name) - 1] = '\0';
	
	char serv_name[1000];
	snprintf(serv_name, sizeof(serv_name), "Saves/%s.txt", file_name);
	FILE* save;
	save = fopen(serv_name, "w");
	fprintf(save, "%i %lf, %lf %i", lifes, worldTime, total_distance, road_line_offset);
	fclose(save);
	save = fopen("save_names", "a");
	fprintf(save, "%s ", file_name);
	save_names.Emplace_back(file_name);
	fclose(save);
	current_save_number = 0;
	if (!game_pause)
		Pause();
}

void Game::Load(char *filename)
{
	load = false;
	FILE* load;
	char serv_name[1000];
	snprintf(serv_name, sizeof(serv_name), "Saves/%s.txt", filename);
	load = fopen(serv_name, "r");
	if (load != NULL)
	{
		int temp_int;
		double temp_double;
		fscanf(load, "%i", &temp_int);
		lifes = temp_int;
		fscanf(load, "%lf", &temp_double);
		worldTime = temp_double;
		fscanf(load, "%lf", &temp_double);
		total_distance = temp_double;
		fscanf(load, "%i", &temp_int);
		road_line_offset = temp_int;
	}

	if (!game_pause)
		Pause();
}

void Game::LoadSaveNames()
{
	FILE* load;
	load = fopen("save_names", "r");
	char check = ' ';
	int i = 0;
	if (!load == NULL)
	{
		while (check != EOF)
		{
			char txt[1000];
			fscanf(load, "%s", txt);
			char* temp = new char[strlen(txt) + 1];
			strcpy(temp, txt);
			check = getc(load);
			save_names.Emplace_back(temp);
		}
		save_names.Pop();

		fclose(load);
		current_save_number = 0;
	}
}

void Game::LoadRender()
{
	char text[100];
	SDL_FillRect(static_surfaces.Get(0), NULL, blue);
	if (save_names.Size() <= 5)
	{
		for (int i = 0; i < save_names.Size(); i++)
		{
			sprintf(text, "%s", save_names.Get(i));
			if (current_save_number == i)
				DrawRectangle(static_surfaces.Get(0), static_surfaces.Get(0)->w / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 3 + 10 * i,
					190, 10, red, red);

			DrawString(static_surfaces.Get(0), static_surfaces.Get(0)->w / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 3 + 10 * i, text, static_surfaces.Get(1));

		}
	}
	else
	{
		
		int temp = 0;
		printf("%i\n", current_save_number);
		for (int i = current_save_number; i < current_save_number + 5; i++)
		{
			if (i < save_names.Size())
			{
				sprintf(text, "%s", save_names.Get(i));
				if (current_save_number == i)
					DrawRectangle(static_surfaces.Get(0), static_surfaces.Get(0)->w / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 3 + 10 * temp,
						190, 10, red, red);

				DrawString(static_surfaces.Get(0), static_surfaces.Get(0)->w / 2 - strlen(text) * 8 / 2, SCREEN_HEIGHT / 3 + 10 * temp, text, static_surfaces.Get(1));
				temp++;
			}
			
		}
	}
	
	SDL_UpdateTexture(scrtex, NULL, static_surfaces.Get(0)->pixels, static_surfaces.Get(0)->pitch);
	SDL_RenderCopy(renderer, scrtex, NULL, NULL);
}