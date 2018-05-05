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

    Manager * manager = createManager(MANAGER_SERVER);
    manager -> db = createDatabase();
    manager -> connection = createServer(argv[1]);

    fillDatabase(manager -> db, argv[2]);
    puts("Server on");

    while (1) {
        maiBineDadeamLaASE(manager);
    }
    
    return 0;
}