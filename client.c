#include <stdio.h>
#include <stdlib.h>
#include "src/ConnectionManager.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        puts("Usage: ./client <IP_server> <port_server>");
        exit(0);
    }

    puts("Client on");
    Client * client = createClient(argv[1], argv[2], NULL);

    int i = 0;
    while (1) {
        ClientCommand * command = clientGetCommand(client);
        clientPrintCommand(command);
        
        if (command -> socket_type == CLIENT_INPUT) {
            clientSendCommand(client, command -> command, (i++ % 2) + 1);
        }
    }

    return 0;
}