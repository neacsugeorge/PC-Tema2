#include <stdio.h>

#ifndef LOGGER
#define LOGGER

#define SUCCESS_IN_LIFE "42"

#define ERROR_NOT_AUTHENTICATED -1
#define ERROR_NOT_AUTHENTICATED_STR "Clientul nu este autentificat"

#define ERROR_SESSION_EXISTS -2
#define ERROR_SESSION_EXISTS_STR "Sesiune deja deschisa"

#define ERROR_WRONG_PIN -3
#define ERROR_WRONG_PIN_STR "Pin gresit"

#define ERROR_UNKNOWN_CARD -4
#define ERROR_UNKNOWN_CARD_STR "Numar card inexistent"

#define ERROR_CARD_BLOCKED -5
#define ERROR_CARD_BLOCKED_STR "Card blocat"

#define ERROR_OPERATION_FAIL -6
#define ERROR_OPERATION_FAIL_STR "Operatie esuata"

#define ERROR_UNLOCK_FAIL -7
#define ERROR_UNLOCK_FAIL_STR "Deblocare esuata"

#define ERROR_INSUFFICIENT_FUNDS -8
#define ERROR_INSUFFICIENT_FUNDS_STR "Fonduri insuficiente"

#define ERROR_OPERATION_CANCELED -9
#define ERROR_OPERATION_CANCELED_STR "Operatie anulata"

#define ERROR_OPERATION_CALL -10
#define ERROR_OPERATION_CALL_STR "Eroare la apel %s"

#define IBANK "IBANK> "
#define UNLOCK "UNLOCK> "

#define FILENAME_LENGTH 12
#define SPACES_AND_OTHER 20

typedef struct {
    FILE * logfile;
} Logger;

Logger * createLogger();
void log_error (Logger * logger, char * prefix, int code, char * extra);
void log_message (Logger * logger, char * prefix, char * message);
void closeLogger(Logger * logger);

#endif