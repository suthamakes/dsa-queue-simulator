#include "client.h"

typedef struct{
    int vehicle_id;
    char road_id;
    int lane;
} Vehicle;

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

int main() {
    int sock = create_socket();
    
    connect_to_server(sock, "127.0.0.1");

    receive_data(sock);

    // Close socket
    close(sock);
    
    return 0;
}