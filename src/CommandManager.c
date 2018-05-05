#include <string.h>
#include <stdlib.h>
#include "CommandManager.h"

char * COMMAND[] = {
    ERROR_CMD,
    LOGIN_CMD,
    LOGOUT_CMD,
    LISTSOLD_CMD,
    TRANSFER_CMD,
    UNLOCK_CMD,
    QUIT_CMD,
    MESSAGE_CMD
};

Login * addLogin(Login * start, Login * login) {
    login -> next = NULL;

    if (start == NULL) {
        return login;
    }

    while (start -> next != NULL) {
        start = start -> next;
    }
    start -> next = login;

    return start;
}

Login * findLoginByCard(Login * start, char card[7]) {
    while (start != NULL) {
        if (strncmp(start -> card, card, 6) == 0) {
            return start;
        }

        start = start -> next;
    }

    return start;
}

Login * findLoginBySocket(Login * start, int socket) {
    while (start != NULL) {
        if (start -> socket == socket) {
            return start;
        }

        start = start -> next;
    }

    return start;
}

Login * removeLoginByCard(Login * start, char card[7]) {
    if (start == NULL) {
        return start;
    }

    Login * head = start;
    Login * walk = NULL;

    if (strncmp(start -> card, card, 6) == 0) {
        head = start -> next;
        free(start);
        return head;
    }

    while (start -> next != NULL) {
        if (strncmp(start -> next -> card, card, 6) == 0) {
            walk = start -> next;
            start -> next = start -> next -> next;
            free(walk);
            break;
        }

        start = start -> next;
    }

    return head;
}

Login * removeLoginBySocket(Login * start, int socket) {
    if (start == NULL) {
        return start;
    }

    Login * head = start;
    Login * walk = NULL;
    
    if (start -> socket == socket) {
        head = start -> next;
        free(start);
        return head;
    }


    while (start -> next != NULL) {
        if (start -> next -> socket == socket) {
            walk = start -> next;
            start -> next = start -> next -> next;
            free(walk);
            break;
        }

        start = start -> next;
    }

    return head;
}

void printLogin(Login * login) {
    if (login == NULL) {
        return;
    }

    puts("LOGIN-----------------------------");
    printf("Socket: %d\nAttempts: %d\nActive: %d\nCard: %s\n",
        login -> socket, login -> attempts, login -> active, login -> card);
    puts("ENDLOGIN--------------------------");
}

Manager * createManager(int type) {
    Manager * manager = NULL;
    manager = (Manager *)malloc(sizeof(Manager));

    manager -> type = type;
    manager -> loggedIn = 0;

    manager -> connection = NULL;

    manager -> db = NULL;
    manager -> logger = NULL;
    manager -> loginManager = NULL;

    return manager;
}

void maiBineDadeamLaASE(Manager * manager) {
    void * rawCommand = NULL;
    char identifiable[BUFFER_LENGTH];

    if (manager -> type == MANAGER_CLIENT) {
        rawCommand = clientGetCommand(manager -> connection);
        clientPrintCommand(rawCommand);
        strcpy(identifiable, ((ClientCommand *)rawCommand) -> command);
    }
    else {
        rawCommand = serverGetCommand(manager -> connection);
        serverPrintCommand(rawCommand);
        strcpy(identifiable, ((ServerCommand *)rawCommand) -> command);
    }

    int ID = getCommandID(identifiable);
    if (isUserInput(manager, rawCommand)) {
        log_message(manager -> logger, NULL, identifiable, 1);
    }

    switch(ID) {
        case ERROR:
                handleError(manager, rawCommand);
            break;
        case MESSAGE:
                handleMessage(manager, rawCommand);
            break;
        case LOGIN:
                handleLogin(manager, rawCommand);
            break;
    }
}

int getCommandID(char * command) {
    if (command == NULL) {
        return ERROR_NOT_FOUND;
    }

    int i = 0;
    for (; i < CMD_COUNT; i++) {
        char len = strlen(COMMAND[i]);
        if (strncmp(COMMAND[i], command, len) == 0) {
            return i;
        }
    }

    return ERROR_NOT_FOUND;
}

int isUserInput(Manager * manager, void * command) {
    if (manager -> type == MANAGER_CLIENT) {
        return ((ClientCommand *)command) -> socket_type == CLIENT_INPUT;
    }

    return ((ServerCommand *)command) -> type == SERVER_INPUT;
}

void handleError(Manager * manager, void * command) {
    int code = 0;

    if (manager -> type == MANAGER_CLIENT) {
        sscanf(command, "error %d", &code);
        
        int socket_type = ((ClientCommand *)command) -> socket_type;
        if (socket_type == CLIENT_TCP_SOCKET) {
            log_error(manager -> logger, IBANK, code, NULL);
        }
        else if (socket_type == CLIENT_UDP_SOCKET) {
            log_error(manager -> logger, UNLOCK, code, NULL);
        }
    }
}

void handleMessage(Manager * manager, void * command) {
    if (manager -> type == MANAGER_CLIENT) {
        char message[BUFFER_LENGTH];
        sscanf(command, "message %s", message);
        
        int socket_type = ((ClientCommand *)command) -> socket_type;
        if (socket_type == CLIENT_TCP_SOCKET) {
            log_message(manager -> logger, IBANK, message, 0);
        }
        else if (socket_type == CLIENT_UDP_SOCKET) {
            log_message(manager -> logger, UNLOCK, message, 0);
        }
    }
}

void handleLogin(Manager * manager, void * command) {
    if (manager -> type == MANAGER_CLIENT) {
        if (manager -> loggedIn) {
            log_error(manager -> logger, IBANK, ERROR_SESSION_EXISTS, NULL);
            return;
        }

        ((ClientCommand *)command) -> socket_type = CLIENT_TCP_SOCKET;
        clientSendCommand(manager -> connection, command);
    }
    else {
        char numar_card[7];
        char pin[5];

        int socket = ((ServerCommand *)command) -> socket;
        sscanf(((ServerCommand *)command) -> command, "login %6s %4s", (char *)numar_card, (char *)pin);

        Card * card = getCard(manager -> db, numar_card);
        if (card == NULL) {
            snprintf(((ServerCommand *)command) -> command, BUFFER_LENGTH, "error %d", ERROR_UNKNOWN_CARD);
            serverSendCommand(manager -> connection, command);
            return;
        }
        else {
            if (isLocked(card)) {
                snprintf(((ServerCommand *)command) -> command, BUFFER_LENGTH, "error %d", ERROR_CARD_BLOCKED);
                serverSendCommand(manager -> connection, command);
                return;
            }

            Login * login = NULL;
            if (strncmp(card -> pin, pin, 4) != 0) {
                // Wrong PIN
                login = findLoginBySocket(manager -> loginManager, socket);

                if (login == NULL) {
                    login = (Login *)malloc(sizeof(Login));
                    login -> socket = socket;
                    strcpy(login -> card, numar_card);
                    login -> attempts = 1;
                    login -> active = 0;

                    manager -> loginManager = addLogin(manager -> loginManager, login);
                }
                else {
                    if (findLoginByCard(login, numar_card) == login) {
                        login -> attempts ++;

                        if (login -> attempts > 2) {
                            lock(card);
                            handleLogin(manager, command);
                            return;
                        }
                    }
                    else {
                        strcpy(login -> card, numar_card);
                        login -> attempts = 1;
                    }
                }

                snprintf(((ServerCommand *)command) -> command, BUFFER_LENGTH, "error %d", ERROR_WRONG_PIN);
            }
            else {
                // Good PIN
                login = findLoginByCard(manager -> loginManager, numar_card);
                Login * myLogin = findLoginBySocket(manager -> loginManager, socket);

                if (login == myLogin) {
                    if (login == NULL) {
                        login = (Login *)malloc(sizeof(Login));
                        manager -> loginManager = addLogin(manager -> loginManager, login);

                        login -> socket = socket;
                        strcpy(login -> card, numar_card);
                    }

                    login -> attempts = 0;
                    login -> active = 1;

                    snprintf(((ServerCommand *)command) -> command, BUFFER_LENGTH, "message Welcome %s %s",
                            card -> nume, card -> prenume);
                }
                else if (login != NULL) {
                    while (login != NULL && !login -> active) {
                        login = findLoginByCard(login -> next, numar_card);
                    }

                    // Existing active session with this card
                    if (login != NULL) {
                        snprintf(((ServerCommand *)command) -> command, BUFFER_LENGTH, "error %d", ERROR_SESSION_EXISTS);
                    }
                    else {
                        login = (Login *)malloc(sizeof(Login));
                        manager -> loginManager = addLogin(manager -> loginManager, login);

                        login -> socket = socket;
                        strcpy(login -> card, numar_card);

                        login -> attempts = 0;
                        login -> active = 1;

                        snprintf(((ServerCommand *)command) -> command, BUFFER_LENGTH, "message Welcome %s %s",
                                card -> nume, card -> prenume);
                    }
                }
            }
            
            serverSendCommand(manager -> connection, command);
        }
    }
}