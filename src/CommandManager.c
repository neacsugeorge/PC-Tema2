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
    MESSAGE_CMD,
    END_CONNECTION_CMD,
    CONFIRM_TRANSFER_CMD,
    CONFIRM_UNLOCK_CMD
};

UnlockOperation * addUnlock(UnlockOperation * start, UnlockOperation * unlock) {
    unlock -> next = NULL;

    if (start == NULL) {
        return unlock;
    }

    UnlockOperation * head = start;
    while (start -> next != NULL) {
        start = start -> next;
    }
    start -> next = unlock;

    return head;
}
UnlockOperation * findUnlock(UnlockOperation * start, char card[7]) {
    while (start != NULL) {
        if (strncmp(start -> card, card, 6) == 0) {
            return start;
        }

        start = start -> next;
    }

    return start;
}
UnlockOperation * removeUnlock(UnlockOperation * start, char card[7]) {
    if (start == NULL) {
        return start;
    }

    UnlockOperation * head = start;
    UnlockOperation * walk = NULL;

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

Login * addLogin(Login * start, Login * login) {
    login -> next = NULL;

    if (start == NULL) {
        return login;
    }

    Login * head = start;
    while (start -> next != NULL) {
        start = start -> next;
    }
    start -> next = login;

    return head;
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

Login * findActiveLogin(Login * start, char card[7]) {
    start = findLoginByCard(start, card);
    if (start == NULL) {
        return start;
    }

    while (start != NULL && !start -> active) {
        start = findLoginByCard(start -> next, card);
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
    manager -> last_card[0] = 0;

    manager -> connection = NULL;

    manager -> db = NULL;
    manager -> logger = NULL;

    manager -> loginManager = NULL;
    manager -> unlockManager = NULL;

    return manager;
}

void maiBineDadeamLaASE(Manager * manager) {
    void * rawCommand = NULL;
    char identifiable[BUFFER_LENGTH];

    if (manager -> type == MANAGER_CLIENT) {
        rawCommand = clientGetCommand(manager -> connection);
        // clientPrintCommand(rawCommand);
        strcpy(identifiable, ((ClientCommand *)rawCommand) -> command);
    }
    else {
        rawCommand = serverGetCommand(manager -> connection);
        // serverPrintCommand(rawCommand);
        strcpy(identifiable, ((ServerCommand *)rawCommand) -> command);
    }

    int ID = getCommandID(identifiable);
    if (isUserInput(manager, rawCommand)) {
        log_message(manager -> logger, NULL, identifiable, USER_INPUT);
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
        case LOGOUT:
                handleLogout(manager, rawCommand);
            break;
        case LISTSOLD:
                handleListSold(manager, rawCommand);
            break;
        case TRANSFER:
                handleTransfer(manager, rawCommand, TRANSFER_STEP_INIT);
            break;
        case CONFIRM_TRANSFER:
                if (manager -> type == MANAGER_SERVER) {
                    strcpy(((ServerCommand *)rawCommand) -> command, ((ServerCommand *)rawCommand) -> command + 7);
                    handleTransfer(manager, rawCommand, TRANSFER_STEP_DONE);
                }
            break;
        case UNLOCK_ID:
                handleUnlock(manager, rawCommand, UNLOCK_STEP_INIT);
            break;
        case CONFIRM_UNLOCK:
                if (manager -> type == MANAGER_SERVER) {
                    strcpy(((ServerCommand *)rawCommand) -> command, ((ServerCommand *)rawCommand) -> command + 7);
                    handleUnlock(manager, rawCommand, UNLOCK_STEP_DONE);
                }
            break;
        case QUIT:
                handleQuit(manager, rawCommand);
            break;
        case END_CONNECTION:
                handleEndConnection(manager, rawCommand);
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
        strcpy(message, ((ClientCommand *)command) -> command + 8);
        
        int socket_type = ((ClientCommand *)command) -> socket_type;
        if (socket_type == CLIENT_TCP_SOCKET) {
            log_message(manager -> logger, IBANK, message, NOT_USER_INPUT);

            // Connected
            if (strncmp(message, "Welcome", 7) == 0) {
                manager -> loggedIn = 1;
            }
            // Disconnected
            if (strncmp(message, "Clientul a fost deconectat", 26) == 0) {
                manager -> loggedIn = 0;
            }
        }
        else if (socket_type == CLIENT_UDP_SOCKET) {
            log_message(manager -> logger, UNLOCK, message, NOT_USER_INPUT);
        }
    }
}

void handleLogin(Manager * manager, void * command) {
    char numar_card[7];
    char pin[5];

    if (manager -> type == MANAGER_CLIENT) {
        if (manager -> loggedIn) {
            log_error(manager -> logger, IBANK, ERROR_SESSION_EXISTS, NULL);
            return;
        }

        sscanf(((ClientCommand *)command) -> command, "login %6s %4s", (char *)manager -> last_card, (char *)pin);

        ((ClientCommand *)command) -> socket_type = CLIENT_TCP_SOCKET;
        clientSendCommand(manager -> connection, command);
    }
    else {
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
                Login * myLogin = findLoginBySocket(manager -> loginManager, socket),
                      * activeLogin = findActiveLogin(manager -> loginManager, numar_card);

                if (activeLogin != NULL) {
                    snprintf(((ServerCommand *)command) -> command, BUFFER_LENGTH, "error %d", ERROR_SESSION_EXISTS);
                } else if (login == myLogin) {
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
                else {
                    if (myLogin == NULL) {
                        myLogin = (Login *)malloc(sizeof(Login));
                        manager -> loginManager = addLogin(manager -> loginManager, myLogin);
                        
                        myLogin -> socket = socket;
                    }

                    strcpy(myLogin -> card, numar_card);
                    myLogin -> attempts = 0;
                    myLogin -> active = 1;

                    snprintf(((ServerCommand *)command) -> command, BUFFER_LENGTH, "message Welcome %s %s",
                            card -> nume, card -> prenume);
                }
            }
            
            serverSendCommand(manager -> connection, command);
        }
    }
}

void handleLogout(Manager * manager, void * command) {
    if (manager -> type == MANAGER_CLIENT) {
        if (!manager -> loggedIn) {
            log_error(manager -> logger, NULL, ERROR_NOT_AUTHENTICATED, NULL);
            return;
        }

        ((ClientCommand *)command) -> socket_type = CLIENT_TCP_SOCKET;
        clientSendCommand(manager -> connection, command);
    }
    else {
        handleEndConnection(manager, command);
        snprintf(((ServerCommand *)command) -> command, BUFFER_LENGTH, "message Clientul a fost deconectat");

        serverSendCommand(manager -> connection, command);
    }
}

void handleListSold(Manager * manager, void * command) {
    if (manager -> type == MANAGER_CLIENT) {
        if (!manager -> loggedIn) {
            log_error(manager -> logger, NULL, ERROR_NOT_AUTHENTICATED, NULL);
            return;
        }

        ((ClientCommand *)command) -> socket_type = CLIENT_TCP_SOCKET;
        clientSendCommand(manager -> connection, command);
    }
    else {
        int socket = ((ServerCommand *)command) -> socket;

        Login * login = findLoginBySocket(manager -> loginManager, socket);

        // Not authenticated
        if (login == NULL || !login -> active) {
            snprintf(((ServerCommand *)command) -> command, BUFFER_LENGTH, "error %d", ERROR_OPERATION_FAIL);
        }
        else {
            Card * card = getCard(manager -> db, login -> card);

            if (card == NULL) {
                snprintf(((ServerCommand *)command) -> command, BUFFER_LENGTH, "error %d", ERROR_OPERATION_FAIL);
            }
            else {
                snprintf(((ServerCommand *)command) -> command, BUFFER_LENGTH, "message %.2lf", card -> sold);
            }
        }

        serverSendCommand(manager -> connection, command);
    }
}

void handleTransfer(Manager * manager, void * command, int step) {
    char numar_card[7];
    char nume[13], prenume[13];
    double amount;

    if (manager -> type == MANAGER_CLIENT) {
        if (!manager -> loggedIn) {
            log_error(manager -> logger, NULL, ERROR_NOT_AUTHENTICATED, NULL);
            return;
        }

        if (((ClientCommand *)command) -> socket_type == CLIENT_INPUT) {
            ((ClientCommand *)command) -> socket_type = CLIENT_TCP_SOCKET;
            clientSendCommand(manager -> connection, command);
        }
        else {
            sscanf(((ClientCommand *)command) -> command, "transfer %s %lf %12s %12s",
                    (char*)numar_card, &amount, (char *)nume, (char *)prenume);
            
            if (amount - (int)amount > EPSILON) {
                snprintf(((ClientCommand *)command) -> command, BUFFER_LENGTH, "Transfer %.2lf catre %s %s? [y/n]", amount, nume, prenume);
            }
            else {
                snprintf(((ClientCommand *)command) -> command, BUFFER_LENGTH, "Transfer %d catre %s %s? [y/n]", (int)amount, nume, prenume);
            }
            
            log_message(manager -> logger, IBANK, ((ClientCommand *)command) -> command, BEFORE_USER_INPUT);

            ClientCommand * answer = clientGetCommand(manager -> connection);
            log_message(manager -> logger, NULL, answer -> command, USER_INPUT);

            if (answer -> command[0] == 'y' && answer -> command[1] == '\n') {
                snprintf(((ClientCommand *)command) -> command, BUFFER_LENGTH, "confirmtransfer %s %.2lf", numar_card, amount);
                clientSendCommand(manager -> connection, command);
            }
            else {
                log_error(manager -> logger, IBANK, ERROR_OPERATION_CANCELED, NULL);
            }
        }
    }
    else {
        int socket = ((ServerCommand *)command) -> socket;

        Login * login = findLoginBySocket(manager -> loginManager, socket);

        if (login == NULL || !login -> active) {
            snprintf(((ServerCommand *)command) -> command, BUFFER_LENGTH, "error %d", ERROR_OPERATION_FAIL);
        }
        else {
            sscanf(((ServerCommand *)command) -> command, "transfer %s %lf", numar_card, &amount);

            Card * card = getCard(manager -> db, login -> card),
                 * destination = getCard(manager -> db, numar_card);

            if (card == NULL) {
                snprintf(((ServerCommand *)command) -> command, BUFFER_LENGTH, "error %d", ERROR_OPERATION_FAIL);
            }
            else {

                if (step == TRANSFER_STEP_INIT) {
                    if (destination == NULL) {
                        snprintf(((ServerCommand *)command) -> command, BUFFER_LENGTH, "error %d", ERROR_UNKNOWN_CARD);
                    }
                    else if (amount < 0) {
                        snprintf(((ServerCommand *)command) -> command, BUFFER_LENGTH, "error %d", ERROR_OPERATION_FAIL);
                    }
                    else if (!canTransfer(card, amount)) {
                        snprintf(((ServerCommand *)command) -> command, BUFFER_LENGTH, "error %d", ERROR_INSUFFICIENT_FUNDS);
                    }
                    else {
                        snprintf(((ServerCommand *)command) -> command, BUFFER_LENGTH, "transfer %s %.2lf %s %s",
                            (char *)numar_card, amount, destination -> nume, destination -> prenume);
                    }
                }
                else {
                    transfer(card, destination, amount);
                    snprintf(((ServerCommand *)command) -> command, BUFFER_LENGTH, "message Transfer realizat cu succes");
                }

            }

        }
        
        // Finally send the command back
        serverSendCommand(manager -> connection, command);
    }
}

void handleUnlock(Manager * manager, void * command, int step) {
    if (manager -> type == MANAGER_CLIENT) {
        if (((ClientCommand *)command) -> socket_type == CLIENT_INPUT) {
            snprintf(((ClientCommand *)command) -> command + 6, BUFFER_LENGTH - 6, " %s", (char *)manager -> last_card);
            
            ((ClientCommand *)command) -> socket_type = CLIENT_UDP_SOCKET;
        }
        else {
            log_message(manager -> logger, UNLOCK, "Trimite parola secreta", BEFORE_USER_INPUT);

            ClientCommand * answer = clientGetCommand(manager -> connection);
            
            int len = strlen(((ClientCommand *)command) -> command);
            if (((ClientCommand *)command) -> command[len - 1] == '\n') {
                ((ClientCommand *)command) -> command[len - 1] = 0;
            }

            snprintf(((ClientCommand *)command) -> command + len, BUFFER_LENGTH - len, " %s", answer -> command);
            snprintf(answer -> command, BUFFER_LENGTH, "confirm%s", ((ClientCommand *)command) -> command);
            strcpy(((ClientCommand *)command) -> command, answer -> command);
        }
        
        clientSendCommand(manager -> connection, command);
    }
    else {
        char numar_card[7];
        char password[9];

        Card * card = NULL;

        if (step == UNLOCK_STEP_INIT) {
            sscanf(((ServerCommand *)command) -> command, "unlock %s", (char *)numar_card);

            card = getCard(manager -> db, numar_card);
            if (card == NULL) {
                snprintf(((ServerCommand *)command) -> command, BUFFER_LENGTH, "error %d", ERROR_UNKNOWN_CARD);
            }
            else if (!isLocked(card)) {
                snprintf(((ServerCommand *)command) -> command, BUFFER_LENGTH, "error %d", ERROR_OPERATION_FAIL);
            }
            else if (findUnlock(manager -> unlockManager, numar_card) != NULL) {
                snprintf(((ServerCommand *)command) -> command, BUFFER_LENGTH, "error %d", ERROR_UNLOCK_FAIL);
            }
            else {
                UnlockOperation * unlockOperation = (UnlockOperation *)malloc(sizeof(UnlockOperation));
                strcpy(unlockOperation -> card, numar_card);
                manager -> unlockManager = addUnlock(manager -> unlockManager, unlockOperation);
            }
        }
        else {
            sscanf(((ServerCommand *)command) -> command, "unlock %s %s", (char *)numar_card, (char *)password);

            card = getCard(manager -> db, numar_card);
            if (card == NULL) {
                snprintf(((ServerCommand *)command) -> command, BUFFER_LENGTH, "error %d", ERROR_UNKNOWN_CARD);
            }
            else if (unlock(card, password)) {
                manager -> unlockManager = removeUnlock(manager -> unlockManager, numar_card);
                snprintf(((ServerCommand *)command) -> command, BUFFER_LENGTH, "message Card deblocat");
            }
            else {
                manager -> unlockManager = removeUnlock(manager -> unlockManager, numar_card);                
                snprintf(((ServerCommand *)command) -> command, BUFFER_LENGTH, "error %d", ERROR_UNLOCK_FAIL);
            }
        }

        serverSendCommand(manager -> connection, command);
    }
}

void handleQuit(Manager * manager, void * command) {
    if (manager -> type == MANAGER_CLIENT) {
        if (((ClientCommand *)command) -> socket_type == CLIENT_INPUT) {
            ((ClientCommand *)command) -> socket_type = CLIENT_TCP_SOCKET;
            clientSendCommand(manager -> connection, command);
        }

        closeLogger(manager -> logger);
        closeClient(manager -> connection);
        exit(0);
    }
    else {
        Server * server = manager -> connection;
        if (((ServerCommand *)command) -> type == SERVER_INPUT) {
            ServerCommand quit = {
                .type = SERVER_TCP_RECEIVE,
                .command = "quit",
                .socket = 0
            };
            ServerCommand goodByeCruelWorld = {
                .type = SERVER_TCP_RECEIVE,
                .command = "message Server goes offline! Goodbye cruel world!",
                .socket = 0
            };

            int i = 1;
            for (; i < server -> max_descriptor_id; i++) {
                if (FD_ISSET(i, &server -> descriptors) && i != server -> tcp_socket && i != server -> udp_socket) {
                    quit.socket = goodByeCruelWorld.socket = i;
                    serverSendCommand(server, &goodByeCruelWorld);
                    serverSendCommand(server, &quit);
                }
            }

            closeLogger(manager -> logger);
            closeServer(server);
            exit(0);
        }
        else {
            // I don't care about the client here
            // Will be handled by handleEndConnection
        }
    }
}

void handleEndConnection(Manager * manager, void * command) {
    if (manager -> type == MANAGER_SERVER) {
        int socket = ((ServerCommand *)command) -> socket;

        int removeCount = 0;
        Login * login = findLoginBySocket(manager -> loginManager, socket);

        while (login != NULL) {
            login = findLoginBySocket(login -> next, socket);
            removeCount++;
        }

        while (removeCount --) {
            manager -> loginManager = removeLoginBySocket(manager -> loginManager, socket);
        }
    }
}