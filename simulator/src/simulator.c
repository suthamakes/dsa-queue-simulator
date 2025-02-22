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
    // A road lanes (A1, A2, A3)
    { {150, 250, 0, 150}, {250, 350, 0, 150}, {350, 450, 0, 150} }, //check y_end imp!!!
    
    // B road lanes (B1, B2, B3)
    {{350, 450, 450, 600}, {250, 350, 450, 600}, {150, 250, 450, 600}},
    
    // C road lanes (C1, C2, C3)
    { {450, 600, 150, 250}, {450, 600, 250, 350}, {450, 600, 350, 450} },
    
    // D road lanes (D1, D2, D3)
    {  {0, 150, 350, 450}, {0, 150, 250, 350},{0, 150, 150, 250} }
};
   
void getLaneCenter(char road, int lane, int *x, int *y) {
    int roadIndex = road - 'A';  // Convert 'A'-'D' to index 0-3
    int laneIndex = lane - 1;    // Convert 1-3 to index 0-2

    *x = (lanePositions[roadIndex][laneIndex].x_start + lanePositions[roadIndex][laneIndex].x_end) / 2 - 20 / 2;
    *y = (lanePositions[roadIndex][laneIndex].y_start + lanePositions[roadIndex][laneIndex].y_end) / 2 - 20 / 2;

    // Debugging output
    printf("Vehicle starting at Road %c, Lane %d → Center (%d, %d)\n", road, lane, *x, *y);
}

void moveVehicle(Vehicle *vehicle) {
    int targetX, targetY;
    getLaneCenter(vehicle->targetRoad, vehicle->targetLane, &targetX, &targetY);

    int reachedX = (abs(vehicle->rect.x - targetX) < vehicle->speed);
    int reachedY = (abs(vehicle->rect.y - targetY) < vehicle->speed);

    // Move right or left first
    if (!reachedX) {
        if (vehicle->rect.x < targetX) vehicle->rect.x += vehicle->speed;
        else vehicle->rect.x -= vehicle->speed;
    } 
    // Move up or down after reaching correct X position
    else if (!reachedY) {
        if (vehicle->rect.y < targetY) vehicle->rect.y += vehicle->speed;
        else vehicle->rect.y -= vehicle->speed;
    }

    // Snap into position if very close
    if (reachedX) vehicle->rect.x = targetX;
    if (reachedY) vehicle->rect.y = targetY;

    // Debugging output
    printf("Vehicle %d Position: (%d, %d) Target: (%d, %d)\n", vehicle->vehicle_id, vehicle->rect.x, vehicle->rect.y, targetX, targetY);
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
    {0, 0, 20, 20},  // Temporary position (updated below)
    1, 'D', 3, 2,    // ID=1, starts at road 'D', lane 3, speed=3
    'A', 1          // Target is road 'A', lane 1
    };
    getLaneCenter(vehicle1.road_id, vehicle1.lane, &vehicle1.rect.x, &vehicle1.rect.y);

    Vehicle vehicle2 = {
    {0, 0, 20, 20},  // Temporary values (updated below)
    2, 'A', 3, 2,  // ID=2, starts at road 'A', lane 3
    'C', 1                     // Target is road 'C', lane 1
    };

    // Set correct initial position (centered in A3)
    getLaneCenter(vehicle2.road_id, vehicle2.lane, &vehicle2.rect.x, &vehicle2.rect.y);

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
        
        DrawBackground(renderer);
        TrafficLightState(renderer);
        drawVehicle(renderer, &vehicle1);
        drawVehicle(renderer, &vehicle2);

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