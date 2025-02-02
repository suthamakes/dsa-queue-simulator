#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>

int InitializeSDL(void);
SDL_Window* CreateWindow(const char *title, int width, int height);
SDL_Renderer* CreateRenderer(SDL_Window *window);
void DrawDashedLine(SDL_Renderer *renderer, int x1, int y1, int x2, int y2, int dashLength);
void DrawBackground(SDL_Renderer *renderer);


#endif