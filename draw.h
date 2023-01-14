#pragma once

extern "C" {
#include"./SDL2-2.0.10/include/SDL.h"
#include"./SDL2-2.0.10/include/SDL_main.h"
}

void DrawString(SDL_Surface*, int, int, const char*, SDL_Surface*);
void DrawSurface(SDL_Surface*, SDL_Surface*, int, int);
void DrawPixel(SDL_Surface*, int, int, Uint32);
void DrawLine(SDL_Surface*, int, int, int, int, int, Uint32);
void DrawRectangle(SDL_Surface*, int, int, int, int,
	Uint32, Uint32);

