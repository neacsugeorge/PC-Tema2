#include "CardManager.h"
#include "Logger.h"
#include "ConnectionManager.h"

#ifndef COMMAND_MANAGER
#define COMMAND_MANAGER

#define MANAGER_CLIENT 0
#define MANAGER_SERVER 1

#define NO 0
#define YES 1

#define ERROR_NOT_FOUND -1
#define ERROR 0
#define LOGIN 1
#define LOGOUT 2
#define CMD_COUNT 3

#define ERROR_CMD "error"
#define LOGIN_CMD "login"
#define LOGOUT_CMD "logout"

typedef struct Login {
    int socket;
    int attempts;
    int active;
    char card[7];
    struct Login * next;
} Login;

typedef struct {
    int type;
    int loggedIn;

    void * connection;

    Database * db;
    Logger * logger;
    Login * loginManager;

} Manager;

Login * addLogin(Login * start, Login * login);
Login * findLoginByCard(Login * start, char card[7]);
Login * findLoginBySocket(Login * start, int socket);
Login * removeLoginByCard(Login * start, char card[7]);
Login * removeLoginBySocket(Login * start, int socket);
void printLogin(Login * login);

Manager * createManager(int type);
void maiBineDadeamLaASE(Manager * manager);
int getCommandID(char * command);
int isUserInput(Manager * manager, void * command);

void handleError(Manager * manager, void * command);
void handleLogin(Manager * manager, void * command);


#endif