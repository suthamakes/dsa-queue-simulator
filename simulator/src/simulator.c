#include "client.h"
#include "renderer.h"

void receive_data(int sock) {
    Vehicle received_data;

    while (1) {
        ssize_t bytes_received = recv(sock, &received_data, sizeof(received_data), 0);
        
        if (bytes_received == 0) {
            printf("Server disconnected.\n");
            break;
        } else if (bytes_received < 0) {
            perror("Receive failed");
            break;
        }
        
        printf("Received Data: Vehicle ID %d on Road %c Lane %cL%d\n",
               received_data.vehicle_id, received_data.road_id, received_data.road_id, received_data.lane);
        
        sleep(1);
    }
}

    static SDL_Window *window = NULL;
    static SDL_Renderer *renderer = NULL;

    typedef struct {
        int x_start, x_end;
        int y_start, y_end;
    } LanePosition;

LanePosition lanePositions[4][3] = {
    // A road lanes (North to South) (A1, A2, A3)
    { {150, 250, -30, -30}, {250, 350, -30, -30}, {350, 450, -30, -30} },
    
    // B road lanes (South to North) (B1, B2, B3)
    {{350, 450, 630, 630}, {250, 350, 630, 630}, {150, 250, 630, 630}},
    
    // C road lanes (East to West) (C1, C2, C3)
    { {630, 630, 150, 250}, {630, 630, 250, 350}, {630, 630, 350, 450} },
    
    // D road lanes (West to East) (D1, D2, D3)
    { {-30, -30, 350, 450}, {-30, -30, 250, 350}, {-30, -30, 150, 250} }
};

void getLaneCenter(char road, int lane, int *x, int *y) {
    int roadIndex = road - 'A';  // Convert 'A'-'D' to index 0-3
    int laneIndex = lane - 1;    // Convert 1-3 to index 0-2
    
    // For horizontal roads (D and C)
    if (road == 'D') {
        *x = -30;  // Start beyond left edge
        *y = (lanePositions[roadIndex][laneIndex].y_start + lanePositions[roadIndex][laneIndex].y_end) / 2 - 20 / 2;
    }
    else if (road == 'C') {
        *x = SCREEN_WIDTH + 10;  // Start beyond right edge
        *y = (lanePositions[roadIndex][laneIndex].y_start + lanePositions[roadIndex][laneIndex].y_end) / 2 - 20 / 2;
    }
    // For vertical roads (A and B)
    else if (road == 'A') {
        *x = (lanePositions[roadIndex][laneIndex].x_start + lanePositions[roadIndex][laneIndex].x_end) / 2 - 20 / 2;
        *y = -30;  // Start beyond top edge
    }
    else if (road == 'B') {
        *x = (lanePositions[roadIndex][laneIndex].x_start + lanePositions[roadIndex][laneIndex].x_end) / 2 - 20 / 2;
        *y = SCREEN_HEIGHT + 10;  // Start beyond bottom edge
    }
}

void moveVehicle(Vehicle *vehicle) {
    int targetX, targetY;
    
    // Set target positions beyond screen edges based on target road
    if (vehicle->targetRoad == 'A') {
        targetX = (lanePositions[0][vehicle->targetLane - 1].x_start + lanePositions[0][vehicle->targetLane - 1].x_end) / 2 - 20 / 2;
        targetY = -30;  // Move beyond top edge
    }
    else if (vehicle->targetRoad == 'B') {
        targetX = (lanePositions[1][vehicle->targetLane - 1].x_start + lanePositions[1][vehicle->targetLane - 1].x_end) / 2 - 20 / 2;
        targetY = SCREEN_HEIGHT + 10;  // Move beyond bottom edge
    }
    else if (vehicle->targetRoad == 'C') {
        targetX = SCREEN_WIDTH + 10;  // Move beyond right edge
        targetY = (lanePositions[2][vehicle->targetLane - 1].y_start + lanePositions[2][vehicle->targetLane - 1].y_end) / 2 - 20 / 2;
    }
    else if (vehicle->targetRoad == 'D') {
        targetX = -30;  // Move beyond left edge
        targetY = (lanePositions[3][vehicle->targetLane - 1].y_start + lanePositions[3][vehicle->targetLane - 1].y_end) / 2 - 20 / 2;
    }

    int reachedX = (abs(vehicle->rect.x - targetX) <= vehicle->speed);
    int reachedY = (abs(vehicle->rect.y - targetY) <= vehicle->speed);

    if ((vehicle->road_id == 'A' && vehicle->targetRoad == 'C') || 
        (vehicle->road_id == 'B' && vehicle->targetRoad == 'D')) {
        // Move Y first (for A3 → C1 and B3 → D1)
        if (!reachedY) {
            if (vehicle->rect.y < targetY) vehicle->rect.y += vehicle->speed;
            else vehicle->rect.y -= vehicle->speed;
        } else if (!reachedX) {
            if (vehicle->rect.x < targetX) vehicle->rect.x += vehicle->speed;
            else vehicle->rect.x -= vehicle->speed;
        }
    } else {
        // Default: Move X first (for D3 → A1 and C3 → B1)
        if (!reachedX) {
            if (vehicle->rect.x < targetX) vehicle->rect.x += vehicle->speed;
            else vehicle->rect.x -= vehicle->speed;
        } else if (!reachedY) {
            if (vehicle->rect.y < targetY) vehicle->rect.y += vehicle->speed;
            else vehicle->rect.y -= vehicle->speed;
        }
    }
}




int main() {
    // Socket related code commented out during the development of UI elements
    // int sock = create_socket();

    if (InitializeSDL() < 0) {
        return 1;
    }
    window = CreateWindow("Traffic Simulator", SCREEN_WIDTH, SCREEN_HEIGHT);
    if (!window) {   
        return 1;
    }
    renderer = CreateRenderer(window);
    if (!renderer) {
        return 1;
    }

    // connect_to_server(sock, "127.0.0.1");

    Vehicle vehicle1 = {
    {0, 0, 20, 20},
    1, 'B', 3, 2,
    'D', 1
    };
    getLaneCenter(vehicle1.road_id, vehicle1.lane, &vehicle1.rect.x, &vehicle1.rect.y);

    Vehicle vehicle2 = {
    {600, 600, 20, 20},
    2, 'A', 3, 2,
    'C', 1
    };
    getLaneCenter(vehicle2.road_id, vehicle2.lane, &vehicle2.rect.x, &vehicle2.rect.y);

    Vehicle vehicle3 = {
    {0, 0, 20, 20},
    1, 'C', 3, 2,
    'B', 1
    };
    getLaneCenter(vehicle3.road_id, vehicle3.lane, &vehicle3.rect.x, &vehicle3.rect.y);

    Vehicle vehicle4 = {
    {600, 600, 20, 20},
    2, 'D', 3, 2,
    'A', 1
    };
    getLaneCenter(vehicle4.road_id, vehicle4.lane, &vehicle4.rect.x, &vehicle4.rect.y);

    Vehicle vehicle5 = {
    {0, 0, 20, 20},
    1, 'D', 2, 2,
    'C', 2
    };
    getLaneCenter(vehicle5.road_id, vehicle5.lane, &vehicle5.rect.x, &vehicle5.rect.y);

    Vehicle vehicle6 = {
    {600, 600, 20, 20},
    2, 'A', 2, 3,
    'B', 2
    };
    getLaneCenter(vehicle6.road_id, vehicle6.lane, &vehicle6.rect.x, &vehicle6.rect.y);

    int running = 1;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN)) {
                running = 0;
            }
        }
        moveVehicle(&vehicle1);
        moveVehicle(&vehicle2);
        moveVehicle(&vehicle3);
        moveVehicle(&vehicle4);
        moveVehicle(&vehicle5);
        moveVehicle(&vehicle6);
        
        DrawBackground(renderer);
        TrafficLightState(renderer);
        drawVehicle(renderer, &vehicle1);
        drawVehicle(renderer, &vehicle2);
        drawVehicle(renderer, &vehicle3);
        drawVehicle(renderer, &vehicle4);
        drawVehicle(renderer, &vehicle5);
        drawVehicle(renderer, &vehicle6);

        SDL_RenderPresent(renderer);
        SDL_Delay(30);

    }

    // receive_data(sock);

    // Close socket
    // close(sock);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}