#include <stdio.h>
#include <stdlib.h>
#include "src/CardManager.h"

int main(int argc, char *argv[]) {
    if (argc != 3) {
        puts("Usage: ./server <port_server> <user_data_files>");
        exit(0);
    }

    Database * db = createDatabase();
    fillDatabase(db, argv[2]);
    printDatabase(*db);
    printCard(*getCard(db, "111789"));
    
    puts("Server on");
    
    return 0;
}