#ifndef RENDERER_H
#define RENDERER_H

#include <SDL2/SDL.h>

typedef struct{
    SDL_Rect rect;
    int vehicle_id;
    char road_id;
    int lane;
    int speed;
    char targetRoad;
    int targetLane; 
} Vehicle;

extern int SCREEN_WIDTH;
extern int SCREEN_HEIGHT;
extern int LANE_WIDTH;

int InitializeSDL(void);
SDL_Window* CreateWindow(const char *title, int width, int height);
SDL_Renderer* CreateRenderer(SDL_Window *window);
void DrawBackground(SDL_Renderer *renderer);

void drawVehicle(SDL_Renderer *renderer, Vehicle *vehicle);
void TrafficLightState(SDL_Renderer *renderer);
void getLaneCenter(char road, int lane, int *x, int *y);
void moveVehicle(Vehicle *vehicle);

#endif