# TCP-IP Chat Room

A multi-client chat room application developed using TCP/IP socket programming in C on Linux.  
This project demonstrates concepts like socket programming, multithreading, client-server communication, synchronization, and real-time messaging.

# Features

- Multiple clients can connect simultaneously
- User registration and login system
- Support private message 
- Real-time group chat
- TCP socket communication
- Multithreaded server handling
- Graceful client disconnect handling
- Broadcast messaging
- Linux terminal-based application

---

# Technologies Used

- C Programming
- Linux System Calls
- TCP/IP Sockets
- POSIX Threads (`pthread`)
- GCC Compiler

---

# Project Structure

├── server.c
├── client.c
├── Makefile
├── README.md

# Compilation

Compile Server
    -> gcc server.c funct.c -o server -lpthread
Compile Client
    -> gcc client.c -o client -lpthread

# Execution
Run Server
./server
-> Server starts listening on a specific port.

Run Client
Open another terminal:
./client
-> You can run multiple clients in different terminals.

# Workflow
1.Start the server
2.Clients connect to the server
3.User can:
  1.Register
  2.Login
4.Connected users can chat in real time
5.Messages are broadcast to all active users

# Socket Functions Used

| Function    | Purpose              |
| ----------- | -------------------- |
| socket()    | Create socket        |
| bind()      | Bind IP and port     |
| listen()    | Wait for connections |
| accept()    | Accept client        |
| connect()   | Connect to server    |
| send()      | Send data            |
| recv()      | Receive data         |
| close()     | Close socket         |

# Thread Functions Used

| Function           | Purpose          |
| ------------------ | ---------------- |
| pthread_create()   | Create thread    |
| pthread_join()     | Wait for thread  |
| pthread_exit()     | Terminate thread |

# Concepts Covered
-> TCP/IP Communication
-> Client-Server Architecture
-> Multithreading
-> Concurrent Client Handling
-> Socket Programming
-> Process Synchronization
-> Network Programming in Linux

# Future Improvements
-> File sharing
-> Message encryption
-> GUI version
->Chat history storage

# Sample Output
# Server
 ./server
Chat Server: Server Port Information: 127.0.0.1:6333
Chat Server: Incoming Registration Request 127.0.0.1:52936
Chat Server: Incoming Registration Request 127.0.0.1:51220
Chat Server: abhi left the chat
Chat Server: saurabh left the chat

# Client1
./client
INFO: Connected to the Server

1. Register
2. Login

Enter your Option: 2
Username: abhi
Password: 2345
Login successful
saurabh joined the chat
saurabh: Hii
Hello
/exit

# Client2
 ./client
INFO: Connected to the Server

1. Register
2. Login

Enter your Option: 2
Username: saurabh
Password: 1234
Login successful
Hii
abhi: Hello
abhi left the chat
/exit
