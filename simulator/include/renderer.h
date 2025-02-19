#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>

typedef struct{
    SDL_Rect rect;
    int vehicle_id;
    char road_id;
    int lane;
    int speed;
    int targetX;
    int targetY;
} Vehicle;

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern int LANE_WIDTH;

int InitializeSDL(void);
SDL_Window* CreateWindow(const char *title, int width, int height);
SDL_Renderer* CreateRenderer(SDL_Window *window);
void DrawBackground(SDL_Renderer *renderer);

void TrafficLightState(SDL_Renderer *renderer);

#endif