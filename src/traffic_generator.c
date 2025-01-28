#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define ROADS 4
#define LANES 3

typedef struct {
    int vehicle_id;  
    char road_id;    
    int lane;
} Vehicle;

char getRandomRoad() {
    char roads[] = {'A', 'B', 'C', 'D'};
    return roads[rand() % ROADS];
}

Vehicle generate_vehicle() {
    Vehicle v;

    v.vehicle_id = rand() % 100;
    v.road_id = getRandomRoad();
    v.lane = rand() % LANES + 1;

    return v;
}

int main() {
    srand(time(NULL));

    while (1) {
        Vehicle vehicle = generate_vehicle();

        printf("Generated Vehicle ID: %d on Road %c Lane %cL%d\n",
               vehicle.vehicle_id, vehicle.road_id, vehicle.road_id, vehicle.lane);



        sleep(rand() % 3 + 1.5); // Sleep for 1.5 to 3 seconds
    }

    return 0;
}
