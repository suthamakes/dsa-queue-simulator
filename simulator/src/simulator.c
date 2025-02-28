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

    Vehicle vehicle5= {
    {0, 0, 20, 20},  // Dimensions and initial position (will be updated by getLaneCenter)
    5,               // vehicle_id = 5
    'D',             // Starting from road D (left side)
    2,               // Using lane 2 (middle lane)
    2,               // Speed
    'C',             // Target road C (right side)
    2                // Target lane 2 (middle lane)
};
getLaneCenter(vehicle5.road_id, vehicle5.lane, &vehicle5.rect.x, &vehicle5.rect.y);

Vehicle vehicle6= {
    {0, 0, 20, 20},  // Dimensions and initial position (will be updated by getLaneCenter)
    6,               // vehicle_id = 6
    'C',             // Starting from road D (left side)
    2,               // Using lane 2 (middle lane)
    2,               // Speed
    'D',             // Target road C (right side)
    2                // Target lane 2 (middle lane)
};
getLaneCenter(vehicle6.road_id, vehicle6.lane, &vehicle6.rect.x, &vehicle6.rect.y);

Vehicle vehicle7= {
    {0, 0, 20, 20},  // Dimensions and initial position (will be updated by getLaneCenter)
    7,               // vehicle_id = 6
    'A',             // Starting from road D (left side)
    2,               // Using lane 2 (middle lane)
    2,               // Speed
    'B',             // Target road C (right side)
    2                // Target lane 2 (middle lane)
};
getLaneCenter(vehicle7.road_id, vehicle7.lane, &vehicle7.rect.x, &vehicle7.rect.y);


Vehicle vehicle8= {
    {0, 0, 20, 20},  // Dimensions and initial position (will be updated by getLaneCenter)
    8,               // vehicle_id = 6
    'B',             // Starting from road D (left side)
    2,               // Using lane 2 (middle lane)
    2,               // Speed
    'A',             // Target road C (right side)
    2                // Target lane 2 (middle lane)
};
getLaneCenter(vehicle8.road_id, vehicle8.lane, &vehicle8.rect.x, &vehicle8.rect.y);

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
        moveVehicle(&vehicle7);
        moveVehicle(&vehicle8);
        
        DrawBackground(renderer);
        
        TrafficLightState(renderer,northSouthGreen,eastWestGreen); 

        drawVehicle(renderer, &vehicle1);
        drawVehicle(renderer, &vehicle2);
        drawVehicle(renderer, &vehicle3);
        drawVehicle(renderer, &vehicle4);
        drawVehicle(renderer, &vehicle5);
        drawVehicle(renderer, &vehicle6);
        drawVehicle(renderer, &vehicle7);
        drawVehicle(renderer, &vehicle8);

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