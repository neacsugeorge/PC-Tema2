#include <stdio.h>
#include <stdlib.h>
#include "src/CardManager.h"
#include "src/ConnectionManager.h"
#include "src/Logger.h"
#include "src/CommandManager.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        puts("Usage: ./server <port_server> <user_data_files>");
        exit(0);
    }

    Database * db = createDatabase();
    fillDatabase(db, argv[2]);
    printDatabase(*db);
    printCard(*getCard(db, "111789"));
    
    Server * server = createServer(argv[1]);
    puts("Server on");

    while (1) {
        ServerCommand * command = serverGetCommand(server);
        serverPrintCommand(command);

        if (command -> type == SERVER_TCP_RECEIVE || command -> type == SERVER_UDP_RECEIVE) {
            serverSendCommand(server, command);
        }
    }
    
    return 0;
}