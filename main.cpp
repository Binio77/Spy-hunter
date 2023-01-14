#include"game.h"



extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}

#define SCREEN_WIDTH	640
#define SCREEN_HEIGHT	480
Game* game = nullptr;
// main
#ifdef __cplusplus
extern "C"
#endif
int main(int argc, char** argv)
{


	game = new Game();
	game->Init("Spy Hunter", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, false);
	while (game->Running())
	{
		game->Time();
		game->Render();
		game->Events();
		
	}
	delete(game);
	

	return 0;
}
