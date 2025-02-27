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

    if (dashLength == 0) {
        // If dashLength is 0, draw a continuous line
        for (int i = 0; i <= steps; i++) {
            SDL_RenderDrawPoint(renderer, (int)x, (int)y);
            x += xIncrement;
            y += yIncrement;
        }
    } else {
        // Draw a dashed line
        for (int i = 0; i <= steps; i++) {
            if ((i / dashLength) % 2 < 1) {
                SDL_RenderDrawPoint(renderer, (int)x, (int)y);
            }
            x += xIncrement;
            y += yIncrement;
        }
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
    SDL_Color laneMarkingRed = {247, 0, 0, 255};
    
    SDL_SetRenderDrawColor(renderer, laneMarking.r, laneMarking.g, laneMarking.b, laneMarking.a);
    // Horizontal road markings
    DrawDashedLine(renderer, 0, 250, 150, 250, 10);
    DrawDashedLine(renderer, 0, 350, 150, 350, 10);
    DrawDashedLine(renderer, 450, 250, 600, 250, 10);
    DrawDashedLine(renderer, 450, 350, 600, 350, 10);

    SDL_SetRenderDrawColor(renderer, laneMarkingRed.r, laneMarkingRed.g, laneMarkingRed.b, laneMarkingRed.a);
    // Split middle lane markings - horizontal
    DrawDashedLine(renderer, 600, 300, 450, 300, 0);  // Middle divider
    DrawDashedLine(renderer, 0, 300, 150, 300, 0);  // Middle divider

    SDL_SetRenderDrawColor(renderer, laneMarking.r, laneMarking.g, laneMarking.b, laneMarking.a);
    // Vertical road markings
    DrawDashedLine(renderer, 250, 0, 250, 150, 10);
    DrawDashedLine(renderer, 350, 0, 350, 150, 10);
    DrawDashedLine(renderer, 250, 450, 250, 600, 10);
    DrawDashedLine(renderer, 350, 450, 350, 600, 10);

    SDL_SetRenderDrawColor(renderer, laneMarkingRed.r, laneMarkingRed.g, laneMarkingRed.b, laneMarkingRed.a);
    // Split middle lane markings - vertical
    DrawDashedLine(renderer, 300, 0, 300, 150, 0);  // Middle divider
    DrawDashedLine(renderer, 300, 600, 300, 450, 0);    // Middle divider
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
}

void drawVehicle(SDL_Renderer *renderer, Vehicle *vehicle) {
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);  // Red vehicle
    SDL_RenderFillRect(renderer, &vehicle->rect);
}

typedef struct {
        int x_start, x_end;
        int y_start, y_end;
    } LanePosition;

LanePosition lanePositions[4][3] = {
    // A road lanes (North to South) (A1, A2, A3)
    // A2 is split into two - leftmost (outgoing), rightmost (incoming)
    { {150, 250, -30, -30}, {270, 300, -30, -30}, {350, 450, -30, -30} },
    
    // B road lanes (South to North) (B1, B2, B3)
    // B2 is split into two - leftmost (incoming), rightmost (outgoing)
    { {350, 450, 630, 630}, {300, 330, 630, 630}, {150, 250, 630, 630} },
    
    // C road lanes (East to West) (C1, C2, C3)
    // C2 is split into two - uppermost (outgoing), lowermost (incoming)
    { {630, 630, 150, 250}, {630, 630, 270, 300}, {630, 630, 350, 450} },
    
    // D road lanes (West to East) (D1, D2, D3)
    // D2 is split into two - uppermost (incoming), lowermost (outgoing)
    { {-30, -30, 350, 450}, {-30, -30, 300, 330}, {-30, -30, 150, 250} }
};

void getLaneCenter(char road, int lane, int *x, int *y) {
    int roadIndex = road - 'A';  // Convert 'A'-'D' to index 0-3
    int laneIndex = lane - 1;    // Convert 1-3 to index 0-2

    int middleLaneOffset = 0;
    if (lane == 2) {
        if (road == 'A') {
            middleLaneOffset = -15; // Move left for outgoing
        } else if (road == 'B') {
            middleLaneOffset = 15;  // Move right for incoming
        } else if (road == 'D') {
            middleLaneOffset = 15;  // Move down for outgoing
        } else if (road == 'C') {
            middleLaneOffset = -15; // Move up for incoming
        }
    }

    if (road == 'A' || road == 'B') {
        *x = ((lanePositions[roadIndex][laneIndex].x_start + lanePositions[roadIndex][laneIndex].x_end) / 2) + middleLaneOffset;
        *y = (road == 'A') ? -30 : SCREEN_HEIGHT + 10;
    } else {
        *x = (road == 'C') ? SCREEN_WIDTH + 10 : -30;
        *y = ((lanePositions[roadIndex][laneIndex].y_start + lanePositions[roadIndex][laneIndex].y_end) / 2) + middleLaneOffset;
    }

    printf("Road: %c, Lane: %d, X: %d, Y: %d, Offset: %d\n", road, lane, *x, *y, middleLaneOffset);
}

void moveVehicle(Vehicle *vehicle) {
    int targetX, targetY;
    getLaneCenter(vehicle->targetRoad, vehicle->targetLane, &targetX, &targetY);

    if (vehicle->targetLane == 1) {
        if (!((vehicle->road_id == 'D' && vehicle->lane == 3 && vehicle->targetRoad == 'A') ||
              (vehicle->road_id == 'A' && vehicle->lane == 3 && vehicle->targetRoad == 'C') ||
              (vehicle->road_id == 'C' && vehicle->lane == 3 && vehicle->targetRoad == 'B') ||
              (vehicle->road_id == 'B' && vehicle->lane == 3 && vehicle->targetRoad == 'D'))) {
            printf("Vehicle %d is not allowed to move to Lane 1! Stopping movement.\n", vehicle->vehicle_id);
            return;
        }
    }

    if (vehicle->targetLane == 2) {
        if (!((vehicle->road_id == 'A' && vehicle->lane == 2 && vehicle->targetRoad == 'B') ||
              (vehicle->road_id == 'A' && vehicle->lane == 2 && vehicle->targetRoad == 'C') ||
              (vehicle->road_id == 'C' && vehicle->lane == 2 && vehicle->targetRoad == 'A') ||
              (vehicle->road_id == 'C' && vehicle->lane == 2 && vehicle->targetRoad == 'D') ||
              (vehicle->road_id == 'B' && vehicle->lane == 2 && vehicle->targetRoad == 'A') ||
              (vehicle->road_id == 'B' && vehicle->lane == 2 && vehicle->targetRoad == 'D') ||
              (vehicle->road_id == 'D' && vehicle->lane == 2 && vehicle->targetRoad == 'C') ||
              (vehicle->road_id == 'D' && vehicle->lane == 2 && vehicle->targetRoad == 'B'))) {
            printf("Vehicle %d is not allowed to move to Lane 2! Stopping movement.\n", vehicle->vehicle_id);
            return;
        }
    }

    int reachedX = (abs(vehicle->rect.x - targetX) <= vehicle->speed);
    int reachedY = (abs(vehicle->rect.y - targetY) <= vehicle->speed);

    // Prioritize movement direction based on road layout
    if ((vehicle->road_id == 'A' && vehicle->targetRoad == 'C') || 
        (vehicle->road_id == 'B' && vehicle->targetRoad == 'D')) {
        // Move Y first
        if (!reachedY) {
            vehicle->rect.y += (vehicle->rect.y < targetY) ? vehicle->speed : -vehicle->speed;
        } else if (!reachedX) {
            vehicle->rect.x += (vehicle->rect.x < targetX) ? vehicle->speed : -vehicle->speed;
        }
    } else {
        // Move X first
        if (!reachedX) {
            vehicle->rect.x += (vehicle->rect.x < targetX) ? vehicle->speed : -vehicle->speed;
        } else if (!reachedY) {
            vehicle->rect.y += (vehicle->rect.y < targetY) ? vehicle->speed : -vehicle->speed;
        }
    }

    // Snap to target position
    if (reachedX) vehicle->rect.x = targetX;
    if (reachedY) vehicle->rect.y = targetY;

    // Debugging Output
    printf("Vehicle %d Position: (%d, %d) Target: (%d, %d)\n", 
            vehicle->vehicle_id, vehicle->rect.x, vehicle->rect.y, targetX, targetY);
}
