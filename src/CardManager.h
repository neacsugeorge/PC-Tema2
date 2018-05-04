#ifndef CARD_MANAGER
#define CARD_MANAGER

typedef struct {
    char nume[13];
    char prenume[13];
    char numar_card[7];
    char pin[5];
    char parola_secreta[9];
    double sold;
} Card;

typedef struct {
    int length;
    int max_size;
    Card ** cards;
} Database;

Database * createDatabase();
void resizeDatabase(Database * db);
void fillDatabase(Database * db, char * filename);
Card * getCard(Database * db, char numar_card[6]);

void printCard(Card card);
void printDatabase(Database db);


#endif