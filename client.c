#include <stdio.h>      // Standard I/O functions
#include <stdlib.h>     // For exit(), malloc(), etc.
#include <string.h>     // For string manipulation functions
#include <unistd.h>     // For close(), read(), write()
#include <arpa/inet.h>  // For socket functions and inet_pton()
#include <pthread.h>    // For threading

int sock; // Global socket descriptor used by both main and receive thread

// Thread function to continuously receive messages from the server
void *receive_msg(void *arg) {
    char buffer[1024];

    while (1) {
        // Receive data from server
        int len = recv(sock, buffer, sizeof(buffer), 0);

        if (len <= 0) {
            // If server disconnects or error occurs
            printf("Disconnected from server\n");
            close(sock);
            exit(0);
        }

        buffer[len] = '\0'; // Null-terminate the received string
        printf("%s", buffer); // Print server message
    }
}

int main() {
    // Create TCP socket
    sock = socket(AF_INET, SOCK_STREAM, 0);

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;           // IPv4
    server_addr.sin_port = htons(6333);         // Port number (network byte order)
    inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr); // Convert IP string to binary

    // Connect to server
    if(connect(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0){
        perror("connect"); // Print error if connection fails
        close(sock);
        exit(0);
    }

    char username[50], password[50];
    int choice;

    printf("INFO: Connected to the Server\n\n");
    printf("1. Register\n2. Login\n\nEnter your Option: ");

    // Read user choice (Register or Login)
    if(scanf("%d", &choice) != 1){
        printf("ERROR: Invalid Choice\n");
        close(sock);
        exit(0);
    }

    if(choice != 1 && choice != 2){
        printf("ERROR: Invalid Choice!!\n");
        close(sock);
        exit(0);
    }

    getchar(); // Consume newline left by scanf

    // Get username
    printf("Username: ");
    fgets(username, sizeof(username), stdin);
    username[strcspn(username, "\n")] = 0; // Remove newline

    // Get password
    printf("Password: ");
    fgets(password, sizeof(password), stdin);
    password[strcspn(password, "\n")] = 0; // Remove newline

    // Send credentials to server
    send(sock, &choice, sizeof(choice), 0);
    send(sock, username, sizeof(username), 0);
    send(sock, password, sizeof(password), 0);

    // Receive server response (registration/login result)
    char buffer[1024];
    int len = recv(sock, buffer, sizeof(buffer) - 1, 0);

    if (len <= 0) {
        printf("Server disconnected\n");
        close(sock);
        exit(0);
    }

    buffer[len] = '\0';
    printf("%s", buffer);

    // If login fails, duplicate login, or already logged in → exit
    if (strstr(buffer, "failed") || strstr(buffer, "Duplicate") || strstr(buffer,"Logged")) {
        close(sock);
        exit(0);
    }

    // Start a thread to receive messages asynchronously
    pthread_t recv_thread;
    pthread_create(&recv_thread, NULL, receive_msg, NULL);
    pthread_detach(recv_thread);

    // Main loop: read user input and send to server
    while (1) {
        if(fgets(buffer, sizeof(buffer), stdin) == NULL)
            break;

        buffer[strcspn(buffer,"\n")] = '\0'; // Remove newline

        // If user types /exit → close connection
        if(strcmp(buffer,"/exit") == 0){
            send(sock, buffer, strlen(buffer), 0);
            close(sock);
            exit(0);
        }

        // Send message to server
        int s = send(sock, buffer, strlen(buffer), 0);
        if (s <= 0) {
            printf("Server closed connection\n");
            close(sock);
            exit(0);
        }
    }
}

