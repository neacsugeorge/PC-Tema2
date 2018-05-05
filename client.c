#include <stdio.h>
#include <stdlib.h>
#include "src/ConnectionManager.h"
#include "src/CommandManager.h"
#include "src/Logger.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        puts("Usage: ./client <IP_server> <port_server>");
        exit(0);
    }

    puts("Client on");

    Manager * manager = createManager(MANAGER_CLIENT);
    manager -> connection = createClient(argv[1], argv[2], NULL);;
    manager -> logger = createLogger();

    while (1) {
        maiBineDadeamLaASE(manager);
    }

    return 0;
}