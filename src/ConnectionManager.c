#include <arpa/inet.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "ConnectionManager.h"
#include "Logger.h"

Client * createClient(char * server_ip, char * server_port, Logger * logger) {
    Client * client = NULL;
    client = (Client *)malloc(sizeof(Client));

    strcpy(client -> server_ip, server_ip);
    strcpy(client -> server_port, server_port);

    FD_ZERO(&client -> descriptors);
    FD_SET(0, &client -> descriptors);
    client -> max_descriptor_id = 1;

    client -> tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    client -> udp_socket = socket(AF_INET, SOCK_DGRAM, 0);

    struct sockaddr_in address;
    address.sin_family = AF_INET;
    address.sin_port = htons(atoi(client -> server_port));
    inet_pton(AF_INET, client -> server_ip, &(address.sin_addr));

    socklen_t address_len = sizeof(address);
    if (connect(client -> tcp_socket, (const struct sockaddr *)(&address), address_len) == -1) {
        log_error(logger, NULL, ERROR_OPERATION_CALL, "connect tcp");
    }
    else {
        FD_SET(client -> tcp_socket, &client -> descriptors);
        if (client -> tcp_socket + 1 > client -> max_descriptor_id) {
            client -> max_descriptor_id = client -> tcp_socket + 1;
        }
    }

    if (connect(client -> udp_socket, (const struct sockaddr *)(&address), address_len) == -1) {
        log_error(logger, NULL, ERROR_OPERATION_CALL, "connect udp");
    }
    else {
        FD_SET(client -> udp_socket, &client -> descriptors);
        if (client -> udp_socket + 1 > client -> max_descriptor_id) {
            client -> max_descriptor_id = client -> udp_socket + 1;
        }
    }

    return client;
}

ClientCommand * clientGetCommand(Client * client) {
    ClientCommand * command = NULL;
    command = (ClientCommand *)malloc(sizeof(ClientCommand));
    memset(command -> command, 0, BUFFER_LENGTH);

    fd_set descriptors = client -> descriptors;

    if (select(client -> max_descriptor_id, &descriptors, NULL, NULL, NULL) > 0) {
        int i = 0;
        for (; i < client -> max_descriptor_id; i++) {
            if (FD_ISSET(i, &descriptors)) {

                if (i == 0) {
                    fgets(command -> command, BUFFER_LENGTH, stdin);
                    command -> socket_type = CLIENT_INPUT;
                }
                else if (i == client -> tcp_socket) {
                    recv(i, command -> command, BUFFER_LENGTH, 0);
                    command -> socket_type = CLIENT_TCP_SOCKET;
                }
                else if (i == client -> udp_socket) {
                    recv(i, command -> command, BUFFER_LENGTH, 0);
                    command -> socket_type = CLIENT_UDP_SOCKET;
                }
                else {
                    strcpy(command -> command, "quit");
                    command -> socket_type = CLIENT_INPUT;
                }

            }
        }
    }
    else {
        log_error(NULL, NULL, ERROR_OPERATION_CALL, "client select");
    }

    return command;
}

void clientSendCommand(Client * client, char command[BUFFER_LENGTH], int socket_type) {
    int socket = 0;
    
    if (socket_type == CLIENT_TCP_SOCKET) {
        socket = client -> tcp_socket;
    }
    else if (socket_type == CLIENT_UDP_SOCKET) {
        socket = client -> udp_socket;
    }
    else {
        return;
    }

    send(socket, command, BUFFER_LENGTH, 0);
}

void clientClearCommand(ClientCommand * command) {
    free (command);
}

void clientPrintCommand(ClientCommand * command) {
    const char * TYPE[] = {"INPUT", "TCP_SOCKET", "UDP_SOCKET"};

    int len = strlen(command -> command) - 1;
    if (command -> command[len] == '\n') {
        command -> command[len] = 0;
    }

    puts("COMMAND---------------------------------");
    printf("Command: %s\nSource: %s\n", command -> command, TYPE[command -> socket_type]);
    puts("ENDCOMMAND------------------------------");
}

Server * createServer(char * server_port) {
    Server * server = NULL;
    server = (Server *)malloc(sizeof(Server));

    strcpy(server -> server_port, server_port);

    FD_ZERO(&server -> descriptors);
    FD_SET(0, &server -> descriptors);
    server -> max_descriptor_id = 1;

    server -> tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
    server -> udp_socket = socket(AF_INET, SOCK_DGRAM, 0);

    int optval = 1;
    setsockopt(server -> udp_socket, SOL_SOCKET, SO_REUSEADDR, (const void *)&optval, sizeof(int));

    struct sockaddr_in address;
    memset((char *)&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = htons(atoi(server -> server_port));
    address.sin_addr.s_addr = INADDR_ANY;

    socklen_t address_len = sizeof(address);    
    if (bind(server -> tcp_socket, (const struct sockaddr *)&address, address_len) < 0) {
        log_error(NULL, NULL, ERROR_OPERATION_CALL, "server bind tcp");
    }
    else {
        FD_SET(server -> tcp_socket, &server -> descriptors);
        if (server -> tcp_socket + 1 > server -> max_descriptor_id) {
            server -> max_descriptor_id = server -> tcp_socket + 1;
        }
    }

    if (bind(server -> udp_socket, (const struct sockaddr *)&address, address_len) < 0) {
        log_error(NULL, NULL, ERROR_OPERATION_CALL, "server bind udp");
    }
    else {
        FD_SET(server -> udp_socket, &server -> descriptors);
        if (server -> udp_socket + 1 > server -> max_descriptor_id) {
            server -> max_descriptor_id = server -> udp_socket + 1;
        }
    }

    if (listen(server -> tcp_socket, SERVER_MAX_CLIENTS) < 0) {
        log_error(NULL, NULL, ERROR_OPERATION_CALL, "server listen tcp");
    }

    return server;
}

ServerCommand * serverGetCommand(Server * server) {
    ServerCommand * command = NULL;
    command = (ServerCommand *)malloc(sizeof(ServerCommand));
    
    memset(command -> command, 0, BUFFER_LENGTH);
    memset((char *)&command -> source, 0, sizeof(command -> source));
    command -> socket = 0;

    socklen_t source_size = sizeof(command -> source);
    int bytes_received = 0;    

    fd_set descriptors = server -> descriptors;

    if (select(server -> max_descriptor_id, &descriptors, NULL, NULL, NULL) > 0) {
        int i = 0;
        for (; i < server -> max_descriptor_id; i++) {
            if (FD_ISSET(i, &descriptors)) {

                if (i == 0) {
                    fgets(command -> command, BUFFER_LENGTH, stdin);
                    command -> type = SERVER_INPUT;
                }
                else if (i == server -> tcp_socket) {
                    int new_socket = 0;

                    if ((new_socket = accept(server -> tcp_socket, (struct sockaddr *)&command -> source, &source_size)) == -1) {
                        log_error(NULL, NULL, ERROR_OPERATION_CALL, "tcp accept");
                    }
                    else {
                        FD_SET(new_socket, &server -> descriptors);
                        if (new_socket + 1 > server -> max_descriptor_id) {
                            server -> max_descriptor_id = new_socket + 1;
                        }
                    }

                    command -> socket = new_socket;
                    command -> type = SERVER_TCP_CONNECT;
                    snprintf(command -> command, BUFFER_LENGTH, "connection %s %d",
                        inet_ntoa(command -> source.sin_addr), ntohs(command -> source.sin_port));
                }
                else if (i == server -> udp_socket) {                    
                    if ((bytes_received = recvfrom(
                        server -> udp_socket, command -> command, BUFFER_LENGTH, 0,
                        (struct sockaddr *)&command -> source, &source_size)) < 0
                    ) {
                        log_error(NULL, NULL, ERROR_OPERATION_CALL, "udp recvfrom");
                    }

                    command -> type = SERVER_UDP_RECEIVE;
                }
                else {
                    if ((bytes_received = recvfrom(
                        i, command -> command, BUFFER_LENGTH, 0,
                        (struct sockaddr *)&command -> source, &source_size)) <= 0
                    ) {
                        if (bytes_received == 0) {
                            snprintf(command -> command, BUFFER_LENGTH, "endconnection %s %d",
                                inet_ntoa(command -> source.sin_addr), ntohs(command -> source.sin_port));
                            command -> type = SERVER_TCP_DISCONNECT;
                        }
                        else {
                            log_error(NULL, NULL, ERROR_OPERATION_CALL, "tcp recvfrom");
                        }
                        close(i);
                        FD_CLR(i, &server -> descriptors);
                    }
                    else {
                        command -> type = SERVER_TCP_RECEIVE;
                    }
                    
                    command -> socket = i;
                }

            }
        }
    }
    else {
        log_error(NULL, NULL, ERROR_OPERATION_CALL, "server select");
    }

    return command;
}

void serverSendCommand(Server * server, ServerCommand * command) {
    int command_length = strlen(command -> command);
    socklen_t address_len = sizeof(command -> source);

    if (command -> type == SERVER_TCP_RECEIVE) {
        send(command -> socket, command -> command, command_length, 0);
    }
    else if (command -> type == SERVER_UDP_RECEIVE) {
        sendto(server -> udp_socket, command -> command, command_length, 0,
                (const struct sockaddr *)&command -> source, address_len);
    }
}

void serverPrintCommand(ServerCommand * command) {
    const char * TYPE[] = {"INPUT", "TCP_CONNECT", "TCP_DISCONNECT", "TCP_RECEIVE", "UDP_RECEIVE"};

    int len = strlen(command -> command) - 1;
    if (command -> command[len] == '\n') {
        command -> command[len] = 0;
    }

    puts("COMMAND---------------------------------");
    printf("Command: %s\nType: %s\nSocket: %d\nSource: %s:%d\n",
        command -> command, TYPE[command -> type],
        command -> socket, inet_ntoa(command -> source.sin_addr), ntohs(command -> source.sin_port)
        );
    puts("ENDCOMMAND------------------------------");
}