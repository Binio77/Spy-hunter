#pragma once
extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}

class Object
{
public:
	float x, y;
	SDL_Surface* surface;
	Object();
};

