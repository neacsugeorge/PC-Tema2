#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include "Logger.h"

char * ERROR_STRINGS[] = {
    SUCCESS_IN_LIFE,
    ERROR_NOT_AUTHENTICATED_STR,
    ERROR_SESSION_EXISTS_STR,
    ERROR_WRONG_PIN_STR,
    ERROR_UNKNOWN_CARD_STR,
    ERROR_CARD_BLOCKED_STR,
    ERROR_OPERATION_FAIL_STR,
    ERROR_UNLOCK_FAIL_STR,
    ERROR_INSUFFICIENT_FUNDS_STR,
    ERROR_OPERATION_CANCELED_STR,
    ERROR_OPERATION_CALL_STR
};

Logger * createLogger() {
    Logger * logger = NULL;
    char filename[30];

    logger = (Logger *)malloc(sizeof(Logger));

    sprintf(filename, "client-%d.log", getpid());
    logger -> logfile = fopen(filename, "w");

    return logger;
}

void log_error (Logger * logger, char * prefix, int code, char * extra) {
    char * printable = NULL;

    int prefix_len = 0;
    if (prefix != NULL) {
        prefix_len = strlen(prefix);
    }

    char * error_code_string = ERROR_STRINGS[abs(code)];
    int error_code_len = strlen(error_code_string);

    int len = prefix_len + error_code_len + SPACES_AND_OTHER;
    int extra_len = 0;
    if (extra != NULL) {
        extra_len = strlen(extra);
        len += extra_len;
    }

    printable = (char *)malloc(len);
    if (extra != NULL) {
        char * partial = (char *)malloc(error_code_len + extra_len + 1);

        sprintf(partial, error_code_string, extra);
        snprintf(printable, len, "%s%d : %s\n\n", prefix != NULL ? prefix : "", code, partial);
        free (partial);
    }
    else {
        snprintf(printable, len, "%s%d : %s\n\n", prefix != NULL ? prefix : "", code, error_code_string);
    }

    printf("%s", printable);
    if (logger != NULL) {
        fprintf(logger -> logfile, "%s", printable);
    }

    free (printable);
}
void log_message (Logger * logger, char * prefix, char * message, int userInput) {
    char * printable = NULL;

    int prefix_len = 0;
    int message_len = 0;

    if (prefix != NULL) {
        prefix_len = strlen(prefix);
    }

    if (message != NULL) {
        message_len = strlen(message);
    }

    if (prefix_len == 0 && message_len == 0) {
        return;
    }

    int len = prefix_len + message_len + SPACES_AND_OTHER;
    printable = (char *)malloc(len);

    snprintf(printable, len, "%s%s\n%s",
        prefix != NULL ? prefix : "",
        message != NULL ? message : "",
        userInput ? "" : "\n");

    if (userInput == NOT_USER_INPUT || userInput == BEFORE_USER_INPUT) {
        printf("%s", printable);
    }
    if (logger != NULL) {
        fprintf(logger -> logfile, "%s", printable);
    }

    free(printable);
}

void closeLogger(Logger * logger) {
    if (logger != NULL) {
        fclose(logger -> logfile);
        free(logger);
    }
}