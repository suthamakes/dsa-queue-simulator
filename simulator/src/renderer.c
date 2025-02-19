#include "renderer.h"

int SCREEN_WIDTH=600;
int SCREEN_HEIGHT=600;
int LANE_WIDTH=100;

int InitializeSDL(void) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        SDL_Log("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
        return -1;
    }
    return 0;
}

SDL_Window* CreateWindow(const char *title, int width, int height) {
    SDL_Window *window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED, width, height, SDL_WINDOW_SHOWN | SDL_WINDOW_ALLOW_HIGHDPI);
    if (!window) {
        SDL_Log("Window could not be created! SDL_Error: %s\n", SDL_GetError());
    }
    return window;
}

SDL_Renderer* CreateRenderer(SDL_Window *window) {
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_Log("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
    }
    return renderer;
}

void DrawDashedLine(SDL_Renderer *renderer, int x1, int y1, int x2, int y2, int dashLength) {
    int dx = x2 - x1;
    int dy = y2 - y1;
    int steps = (abs(dx) > abs(dy)) ? abs(dx) : abs(dy);
    
    float xIncrement = (float)dx / steps;
    float yIncrement = (float)dy / steps;

    float x = x1;
    float y = y1;

    for (int i = 0; i <= steps; i++) {
        if ((i / dashLength) % 2 < 1) {
            SDL_RenderDrawPoint(renderer, (int)x, (int)y);
        }
        x += xIncrement;
        y += yIncrement;
    }
}

void DrawTrafficLight(SDL_Renderer *renderer, int XPos, int YPos, int isGreen, char *orientation) {
    
    const int width = 30;
    const int height = 90;

    if (isGreen) {
        SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    } else {
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
    }

    SDL_Rect trafficLightRect;
    
    if (strcmp(orientation, "vertical") == 0) {
        trafficLightRect = (SDL_Rect){XPos, YPos, width, height}; // Horizontal orientation
    } else if (strcmp(orientation, "horizontal") == 0) {
        trafficLightRect = (SDL_Rect){XPos, YPos, height, width}; // Vertical orientation
    } else {
        printf("Invalid orientation: %s\n", orientation);
        return;
    }
    
    SDL_RenderFillRect(renderer, &trafficLightRect);
}

void DrawLaneMarking(SDL_Renderer *renderer){
    SDL_Color laneMarking = {247, 233, 23, 255};
    
    SDL_SetRenderDrawColor(renderer, laneMarking.r, laneMarking.g, laneMarking.b, laneMarking.a);
    DrawDashedLine(renderer, 0, 250, 150, 250, 10);
    DrawDashedLine(renderer, 0, 350, 150, 350, 10);

    DrawDashedLine(renderer, 450, 250, 600, 250, 10);
    DrawDashedLine(renderer, 450, 350, 600, 350, 10);

    DrawDashedLine(renderer, 250, 0, 250, 150, 10);
    DrawDashedLine(renderer, 350, 0, 350, 150, 10);

    DrawDashedLine(renderer, 250, 450, 250, 600, 10);
    DrawDashedLine(renderer, 350, 450, 350, 600, 10);
}

void DrawBackground(SDL_Renderer *renderer) {
    SDL_Color grassGreen = {34,139,34,255}; 
    SDL_Color roadGray = {50, 50, 50, 255};

    // Set background color (green for grass)
    SDL_SetRenderDrawColor(renderer, grassGreen.r, grassGreen.g, grassGreen.b, grassGreen.a);
    SDL_RenderClear(renderer);

    // Set road color (gray)
    SDL_SetRenderDrawColor(renderer, roadGray.r, roadGray.g, roadGray.b, roadGray.a);

    // Horizontal road
    SDL_Rect horizontalRoad = {0, SCREEN_HEIGHT/4, SCREEN_WIDTH, SCREEN_HEIGHT/2}; 
    SDL_RenderFillRect(renderer, &horizontalRoad);

    // Vertical road
    SDL_Rect verticalRoad = {SCREEN_WIDTH/4, 0, SCREEN_WIDTH/2, SCREEN_HEIGHT}; 
    SDL_RenderFillRect(renderer, &verticalRoad);
    
    DrawLaneMarking(renderer);
}

void TrafficLightState(SDL_Renderer *renderer){
    DrawTrafficLight(renderer, 175, 255, 1, "vertical");
    DrawTrafficLight(renderer, 395, 255, 1, "vertical");
    DrawTrafficLight(renderer, 255, 175, 0, "horizontal");
    DrawTrafficLight(renderer, 255, 395, 1, "horizontal");
    SDL_RenderPresent(renderer);
    SDL_Delay(16); // Delay for ~60 FPS
}