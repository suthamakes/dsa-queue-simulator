#include "client.h"
#include "renderer.h"
#define MAX_VEHICLES 100

void receive_data(int sock) {
    Vehicle received_data;
    ssize_t bytes_received = recv(sock, &received_data, sizeof(received_data), MSG_DONTWAIT);
    if (bytes_received > 0) {
        Vehicle *v = (Vehicle *)malloc(sizeof(Vehicle));
        v->vehicle_id = received_data.vehicle_id;
        v->road_id = received_data.road_id;
        v->lane = received_data.lane;
        v->speed = received_data.speed;
        v->rect.w = 20;  // Hardcoded as in original
        v->rect.h = 20;  // Hardcoded as in original
        v->targetRoad = received_data.targetRoad;
        v->targetLane = received_data.targetLane;
        getLaneCenter(v->road_id, v->lane, &v->rect.x, &v->rect.y);
    } else if (bytes_received == 0) {
        printf("Server disconnected\n");
    } else if (errno != EAGAIN && errno != EWOULDBLOCK) {
        perror("Receive failed");
    }
}

    static SDL_Window *window = NULL;
    static SDL_Renderer *renderer = NULL;

typedef struct {
    Vehicle *vehicles[MAX_VEHICLES];
    int front;
    int rear;
    int size;
} VehicleQueue;


    VehicleQueue queue;

void initQueue(VehicleQueue *q) {
    q->front = 0;
    q->rear = -1;
    q->size = 0;
}

int isQueueFull(VehicleQueue *q) {
    return q->size >= MAX_VEHICLES;
}

int isQueueEmpty(VehicleQueue *q) {
    return q->size == 0;
}

void enqueue(VehicleQueue *q, Vehicle *v) {
    if (isQueueFull(q)) {
        printf("Queue is full! Cannot enqueue vehicle %d\n", v->vehicle_id);
        free(v);
        return;
    }
    q->rear = (q->rear + 1) % MAX_VEHICLES;
    q->vehicles[q->rear] = v;
    q->size++;
    printf("Enqueued vehicle %d on Road %c Lane %d\n", v->vehicle_id, v->road_id, v->lane);
}

Vehicle* dequeue(VehicleQueue *q) {
    if (isQueueEmpty(q)) {
        printf("Queue is empty!\n");
        return NULL;
    }
    Vehicle *v = q->vehicles[q->front];
    q->front = (q->front + 1) % MAX_VEHICLES;
    q->size--;
    return v;
}


int main() {
    int sock = create_socket();

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

    VehicleQueue queue;
    initQueue(&queue);
    connect_to_server(sock, "127.0.0.1");

//     Vehicle vehicle1 = {
//     {0, 0, 20, 20},
//     1, 'B', 3, 2,
//     'D', 1
//     };
//     getLaneCenter(vehicle1.road_id, vehicle1.lane, &vehicle1.rect.x, &vehicle1.rect.y);

//     Vehicle vehicle2 = {
//     {600, 600, 20, 20},
//     2, 'A', 3, 2,
//     'C', 1
//     };
//     getLaneCenter(vehicle2.road_id, vehicle2.lane, &vehicle2.rect.x, &vehicle2.rect.y);

//     Vehicle vehicle3 = {
//     {0, 0, 20, 20},
//     1, 'C', 3, 2,
//     'B', 1
//     };
//     getLaneCenter(vehicle3.road_id, vehicle3.lane, &vehicle3.rect.x, &vehicle3.rect.y);

//     Vehicle vehicle4 = {
//     {600, 600, 20, 20},
//     2, 'D', 3, 2,
//     'A', 1
//     };
//     getLaneCenter(vehicle4.road_id, vehicle4.lane, &vehicle4.rect.x, &vehicle4.rect.y);

//     Vehicle vehicle5= {
//     {0, 0, 20, 20},  // Dimensions and initial position (will be updated by getLaneCenter)
//     5,               // vehicle_id = 5
//     'D',             // Starting from road D (left side)
//     2,               // Using lane 2 (middle lane)
//     2,               // Speed
//     'C',             // Target road C (right side)
//     2                // Target lane 2 (middle lane)
// };
// getLaneCenter(vehicle5.road_id, vehicle5.lane, &vehicle5.rect.x, &vehicle5.rect.y);

// Vehicle vehicle6= {
//     {0, 0, 20, 20},  // Dimensions and initial position (will be updated by getLaneCenter)
//     6,               // vehicle_id = 6
//     'C',             // Starting from road D (left side)
//     2,               // Using lane 2 (middle lane)
//     2,               // Speed
//     'D',             // Target road C (right side)
//     2                // Target lane 2 (middle lane)
// };
// getLaneCenter(vehicle6.road_id, vehicle6.lane, &vehicle6.rect.x, &vehicle6.rect.y);

// Vehicle vehicle7= {
//     {0, 0, 20, 20},  // Dimensions and initial position (will be updated by getLaneCenter)
//     7,               // vehicle_id = 6
//     'A',             // Starting from road D (left side)
//     2,               // Using lane 2 (middle lane)
//     2,               // Speed
//     'B',             // Target road C (right side)
//     2                // Target lane 2 (middle lane)
// };
// getLaneCenter(vehicle7.road_id, vehicle7.lane, &vehicle7.rect.x, &vehicle7.rect.y);


// Vehicle vehicle8= {
//     {0, 0, 20, 20},  // Dimensions and initial position (will be updated by getLaneCenter)
//     8,               // vehicle_id = 6
//     'B',             // Starting from road D (left side)
//     2,               // Using lane 2 (middle lane)
//     2,               // Speed
//     'A',             // Target road C (right side)
//     2                // Target lane 2 (middle lane)
// };
// getLaneCenter(vehicle8.road_id, vehicle8.lane, &vehicle8.rect.x, &vehicle8.rect.y);

    Vehicle *active_vehicles[MAX_VEHICLES] = {0};
    int num_active_vehicles = 0;

    int running = 1;
    SDL_Event event;
    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT || (event.type == SDL_KEYDOWN)) {
                running = 0;
            }
        }

        receive_data(sock);

        while (!isQueueEmpty(&queue) && num_active_vehicles < MAX_VEHICLES) {
            Vehicle *v = dequeue(&queue);
            active_vehicles[num_active_vehicles++] = v;
        }

        updateTrafficLights();

        for (int i = 0; i < num_active_vehicles; i++) {
            if (active_vehicles[i]) {
                moveVehicle(active_vehicles[i]);
                int targetX, targetY;
                getLaneCenter(active_vehicles[i]->targetRoad, active_vehicles[i]->targetLane, &targetX, &targetY);
                if (abs(active_vehicles[i]->rect.x - targetX) <= active_vehicles[i]->speed && 
                    abs(active_vehicles[i]->rect.y - targetY) <= active_vehicles[i]->speed) {
                    printf("Vehicle %d reached target and is removed.\n", active_vehicles[i]->vehicle_id);
                    free(active_vehicles[i]);
                    active_vehicles[i] = NULL;
                }
            }
        }
        int write_idx = 0;
        for (int i = 0; i < num_active_vehicles; i++) {
            if (active_vehicles[i] != NULL) {
                active_vehicles[write_idx++] = active_vehicles[i];
            }
        }
        num_active_vehicles = write_idx;
        
        DrawBackground(renderer);
        
        TrafficLightState(renderer,northSouthGreen,eastWestGreen); 

        printf("Rendering %d active vehicles\n", num_active_vehicles);
            for (int i = 0; i < num_active_vehicles; i++) {
                if (active_vehicles[i]) {
                    drawVehicle(renderer, active_vehicles[i]);
                }
            }

        SDL_RenderPresent(renderer);
        SDL_Delay(30);

    }

    // Close socket
    close(sock);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
    
    return 0;
}