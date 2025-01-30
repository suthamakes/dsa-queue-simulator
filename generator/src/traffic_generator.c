#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <arpa/inet.h>

#include "server.h"

#define ROADS 4
#define LANES 3

typedef struct
{
    int vehicle_id;
    char road_id;
    int lane;
} Vehicle;

void send_data(int socket_fd, Vehicle *data)
{
    if (send(socket_fd, data, sizeof(*data), 0) < 0)
    {
        perror("Send failed");
        exit(EXIT_FAILURE);
    }
    printf("Data sent to client: Vehicle ID: %d on Road %c Lane %cL%d\n",
           data->vehicle_id, data->road_id, data->road_id, data->lane);
}

char getRandomRoad()
{
    char roads[] = {'A', 'B', 'C', 'D'};
    return roads[rand() % ROADS];
}

Vehicle generate_vehicle()
{
    Vehicle v;

    v.vehicle_id = rand() % 100;
    v.road_id = getRandomRoad();
    v.lane = rand() % LANES + 1;

    return v;
}

int main()
{

    int server_fd;
    struct sockaddr_in address;
    srand(time(NULL));

    server_fd = create_socket();
    set_socket_options(server_fd);

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(PORT);

    bind_socket(server_fd, &address);
    printf("Server listening on port %d\n", PORT);
    listen_for_connections(server_fd);

    int new_socket = accept_connection(server_fd, &address);

    while (1)
    {
        Vehicle vehicle = generate_vehicle();

        send_data(new_socket, &vehicle);

        printf("Generated Vehicle ID: %d on Road %c Lane %cL%d\n",
               vehicle.vehicle_id, vehicle.road_id, vehicle.road_id, vehicle.lane);

        sleep(rand() % 3 + 1.5); // Sleep for 1.5 to 3 seconds
    }

    return 0;
}
