#include "header.h"   

int main() {

    int server_sock;   // Socket descriptor for the server

    struct sockaddr_in server_addr, client_addr; // Structures for server and client addresses
    socklen_t addr_size; // Size of client address structure

    // Initialize mutex lock (used in handle_client for shared resources like client list)
    pthread_mutex_init(&lock, NULL);

    // Create a TCP socket
    server_sock = socket(AF_INET, SOCK_STREAM, 0);

    // Configure server address
    server_addr.sin_family = AF_INET;              // IPv4
    server_addr.sin_port = htons(PORT);            // Port (defined in header.h)
    server_addr.sin_addr.s_addr = inet_addr("127.0.0.1"); // Bind to localhost (loopback)

    // Bind socket to IP and port
    bind(server_sock, (struct sockaddr*)&server_addr, sizeof(server_addr));

    // Start listening for incoming connections (backlog = 10)
    listen(server_sock, 10);

    // Print server info
    printf("Chat Server: Server Port Information: %s:%d\n",
           inet_ntoa(server_addr.sin_addr), PORT);

    int cport;       // Client port
    char cadd[15];   // Client IP address (string)

    // Infinite loop to accept multiple clients
    while (1) {
        addr_size = sizeof(client_addr);

        // Accept a new client connection
        int *client_sock = malloc(sizeof(int)); // Allocate memory for client socket
        *client_sock = accept(server_sock, (struct sockaddr*)&client_addr, &addr_size);

        // Extract client IP and port
        strcpy(cadd, inet_ntoa(client_addr.sin_addr));
        cport = ntohs(client_addr.sin_port);

        // Log incoming connection
        printf("Chat Server: Incoming Registration Request %s:%d\n", cadd, cport);

        // Create a new thread to handle this client
        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, client_sock);

        // Detach thread (no need to join later, resources auto-cleaned)
        pthread_detach(tid);
    }

    return 0; // Program should never reach here (infinite loop)
}

