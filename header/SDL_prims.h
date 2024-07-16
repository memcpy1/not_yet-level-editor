#pragma once
#include <SDL.h>

extern void SDL_DrawPolygon(SDL_Renderer*& render, SDL_Point *v, int n, const SDL_Color& c);
extern void SDL_FillPolygon(SDL_Renderer*& render, SDL_Surface* s, SDL_Point *v, int n, const SDL_Color& c);

