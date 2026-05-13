#ifndef HEADER_H
#define HEADER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>

#define PORT 6333
#define MAX_CLIENTS 100

typedef struct {
    int sock;
    char username[50];
} client_t;

extern client_t clients[MAX_CLIENTS];
extern int client_count;
extern pthread_mutex_t lock;

// Function prototypes
int register_user(char *username, char *password);
int login_user(char *username, char *password);
int already_login(char *user);
void online_client_list(int sender);
void broadcast(char *msg, int sender_sock);
void private_msg(char *sender, char *target, char *msg);
void *handle_client(void *arg);

#endif

