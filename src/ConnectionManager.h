#include <sys/time.h>
#include <netinet/in.h>

#include "Logger.h"

#ifndef CONNECTION_MANAGER
#define CONNECTION_MANAGER

#define CLIENT_INPUT 0
#define CLIENT_TCP_SOCKET 1
#define CLIENT_UDP_SOCKET 2

#define SERVER_INPUT 0
#define SERVER_TCP_CONNECT 1
#define SERVER_TCP_DISCONNECT 2
#define SERVER_TCP_RECEIVE 3
#define SERVER_UDP_RECEIVE 4

#define BUFFER_LENGTH 255
#define SERVER_MAX_CLIENTS 25

typedef struct {
    char server_ip[16];
    char server_port[6];
    fd_set descriptors;
    int max_descriptor_id;
    int tcp_socket;
    int udp_socket;
} Client;

typedef struct {
    char command[BUFFER_LENGTH];
    int socket_type;
} ClientCommand;

Client * createClient(char * server_ip, char * server_port, Logger * logger);
ClientCommand * clientGetCommand(Client * client);
void clientSendCommand(Client * client, ClientCommand * command);
void clientClearCommand(ClientCommand * command);
void clientPrintCommand(ClientCommand * command);
void closeClient(Client * client);

typedef struct {
    char server_port[6];
    fd_set descriptors;
    int max_descriptor_id;
    int tcp_socket;
    int udp_socket;
} Server;

typedef struct {
    char command[BUFFER_LENGTH];
    int socket;
    struct sockaddr_in source;
    int type;
} ServerCommand;

Server * createServer(char * server_port);
ServerCommand * serverGetCommand(Server * server);
void serverSendCommand(Server * server, ServerCommand * command);
void serverPrintCommand(ServerCommand * command);
void closeServer(Server * server);

#endif