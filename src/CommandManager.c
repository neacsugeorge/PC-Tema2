#include <string.h>
#include <stdlib.h>
#include "CommandManager.h"

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