#include "header.h"

// Global array to store connected clients
client_t clients[MAX_CLIENTS];
int client_count = 0;          // Current number of connected clients
pthread_mutex_t lock;          // Mutex lock for thread-safe access to clients[]

// -------------------- USER MANAGEMENT --------------------

// Register a new user by saving username and password to users.txt
int register_user(char *username, char *password) {

    FILE *fp = fopen("users.txt", "a+"); // Open file for append/read

    char u[50], p[50];
    rewind(fp); // Move file pointer to start

    // Check if username already exists
    while (fscanf(fp, "%s %s", u, p) != EOF) {
        if (strcmp(u, username) == 0) {
            fclose(fp);
            return 0; // Duplicate username
        }
    }

    // Write new user credentials
    fprintf(fp, "%s %s\n", username, password);
    fclose(fp);
    return 1; // Registration successful
}

// Login user by verifying credentials from users.txt
int login_user(char *username, char *password) {

    FILE *fp = fopen("users.txt", "r");

    char u[50], p[50];

    // Scan file for matching username/password
    while (fscanf(fp, "%s %s", u, p) != EOF) {
        if (strcmp(u, username) == 0 && strcmp(p, password) == 0) {
            fclose(fp);
            return 1; // Login successful
        }
    }

    fclose(fp);
    return 0; // Login failed
}

// -------------------- MESSAGE HANDLING --------------------

// Broadcast message to all connected clients except sender
void broadcast(char *msg, int sender_sock) {

    pthread_mutex_lock(&lock);

    for (int i = 0; i < client_count; i++) {
        if (clients[i].sock != sender_sock) {
            send(clients[i].sock, msg, strlen(msg), 0);
        }
    }

    pthread_mutex_unlock(&lock);
}

// Send private message to a specific target user
void private_msg(char *sender, char *target, char *msg) {

    pthread_mutex_lock(&lock);

    for (int i = 0; i < client_count; i++) {
        if (strcmp(clients[i].username, target) == 0) {
            char buffer[1024];
            sprintf(buffer, "[Private from %s]: %s\n", sender, msg);
            send(clients[i].sock, buffer, strlen(buffer), 0);
        }
    }

    pthread_mutex_unlock(&lock);
}

// Show list of online users to requesting client
void online_client_list(int sender) {
   
    pthread_mutex_lock(&lock);

    char buff[1024] = "Online Users:\n";

    for (int i = 0; i < client_count; i++) {
        if (clients[i].sock != sender) {
            strcat(buff, clients[i].username);
            strcat(buff, "\n");
        }
    }

    send(sender, buff, strlen(buff), 0);

    pthread_mutex_unlock(&lock);
}

// Check if user is already logged in
int already_login(char *user) {
    
    for (int i = 0; i < client_count; i++) {
        if (strcmp(user, clients[i].username) == 0)
            return 1; // Found duplicate login
    }
    return 0;
}

// -------------------- CLIENT HANDLER --------------------

// Thread function to handle each client connection
void *handle_client(void *arg) {
    
    int sock = *(int *)arg;
    free(arg); // Free allocated memory for socket

    char buffer[1024], username[50], password[50];
    int choice, n = 0;

    // Receive user choice (Register/Login)
    recv(sock, &choice, sizeof(choice), 0);

    // Receive username
    n = recv(sock, username, sizeof(username), 0);
    if (n <= 0) {
        close(sock);
        pthread_exit(NULL);
    }
    username[n] = '\0';

    // Receive password
    int n1 = recv(sock, password, sizeof(password), 0);
    if (n1 <= 0) {
        close(sock);
        pthread_exit(NULL);
    }
    password[n1] = '\0';

    int auth = 0; // Authentication flag

    // Handle registration
    if (choice == 1) {
        if (register_user(username, password)) {
            send(sock, "Registered successfully\n", 25, 0);
            auth = 1;
        } else {
            send(sock, "Duplicate username\n", 19, 0);
        }
    }
    // Handle login
    else if (choice == 2) {
        if (login_user(username, password)) {
            pthread_mutex_lock(&lock);

            if (already_login(username)) {
                send(sock, "User Already Logged in\n", 24, 0);
                close(sock);
                pthread_exit(NULL);
            }

            pthread_mutex_unlock(&lock);

            send(sock, "Login successful\n", 17, 0);
            auth = 1;
        } else {
            send(sock, "Login failed\n", 13, 0);
        }
    }

    // If authentication fails → disconnect
    if (!auth) {
        close(sock);
        pthread_exit(NULL);
    }

    // Add client to global list
    pthread_mutex_lock(&lock);
    
    clients[client_count].sock = sock;
    strcpy(clients[client_count].username, username);
    client_count++;
    
    pthread_mutex_unlock(&lock);

    // Notify others that user joined
    sprintf(buffer, "%s joined the chat\n", username);
    broadcast(buffer, sock);

    // -------------------- CHAT LOOP --------------------
    while (1) {
        int len = recv(sock, buffer, sizeof(buffer), 0);
        if (len <= 0)
            break; // Client disconnected

        buffer[len] = '\0';
        buffer[strcspn(buffer, "\n")] = '\0'; // Remove newline

        // Handle commands
        if (strncmp(buffer, "/private", 8) == 0) {
            char target[50], msg[900];
            sscanf(buffer, "/private %s %[^\n]", target, msg);
            private_msg(username, target, msg);
        } else if (strcmp(buffer, "/online") == 0) {
            online_client_list(sock);
        } else if (strcmp(buffer, "/exit") == 0) {
            break;
        } else {
            // Normal broadcast message
            char msg[1100];
            sprintf(msg, "%s: %s\n", username, buffer);
            broadcast(msg, sock);
        }
    }

    // -------------------- CLIENT DISCONNECT --------------------
    pthread_mutex_lock(&lock);

    // Remove client from list
    for (int i = 0; i < client_count; i++) {
        if (clients[i].sock == sock) {
            for (int j = i; j < client_count - 1; j++) {
                clients[j] = clients[j + 1];
            }
            client_count--;
            break;
        }
    }

    pthread_mutex_unlock(&lock);

    // Notify others that user left
    sprintf(buffer, "%s left the chat\n", username);
    broadcast(buffer, sock);
    printf("Chat Server: %s", buffer);

    close(sock);
    pthread_exit(NULL);
}

