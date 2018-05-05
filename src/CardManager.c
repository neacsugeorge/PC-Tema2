#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <math.h>
#include "CardManager.h"

Database * createDatabase() {
    Database * db = NULL;

    db = (Database *)malloc(sizeof(Database));

    db -> length = 0;
    db -> max_size = 1;

    db -> cards = (Card **)malloc(db -> max_size * sizeof(Card *));

    int i = 0;
    for (; i < db -> max_size; i++) {
        db -> cards[i] = NULL;
    }

    return db;
}

void resizeDatabase(Database * db) {
    if (db -> length < db -> max_size) {
        return;
    }

    db -> max_size *= 2;

    Card ** more_cards = NULL;
    more_cards = (Card **)realloc(db -> cards, db -> max_size * sizeof(Card *));

    if (more_cards != NULL) {
        db -> cards = more_cards;
    }
}

void fillDatabase(Database * db, char * filename) {
    FILE * file = fopen(filename, "r");

    fscanf(file, "%d\n", &db -> length);

    while (db -> length > db -> max_size) {
        resizeDatabase(db);
    }

    int i = 0;
    for (; i < db -> length; i++) {
        Card * new_card = (Card *)malloc (sizeof(Card));

        new_card -> locked = 0;
        fscanf(file, "%12s %12s %6s %4s %8s %lf",
            (char *)&new_card -> nume, (char *)&new_card -> prenume,
            (char *)&new_card -> numar_card, (char *)&new_card -> pin,
            (char *)&new_card -> parola_secreta, &new_card ->sold);

        db -> cards[i] = new_card;
    }

    fclose(file);
}

Card * getCard(Database * db, char numar_card[7]) {
    int i = 0;

    for (; i < db -> length; i++) {
        if (strncmp(numar_card, db -> cards[i] -> numar_card, 6) == 0) {
            return db -> cards[i];
        }
    }

    return NULL;
}

int canTransfer(Card * from, double amount) {
    if (from -> sold - amount < EPSILON) {
        return 0;
    }

    return 1;
}

void transfer(Card * from, Card * to, double amount) {
    if (!canTransfer(from, amount)) {
        return;
    }

    from -> sold -= amount;
    to -> sold += amount;
}

void lock(Card * card) {
    card -> locked = 1;
}

unsigned char unlock(Card * card, char * parola_secreta) {
    if (strcmp(card -> parola_secreta, parola_secreta) == 0) {
        card -> locked = 0;
        return 1;
    }

    return 0;
}

unsigned char isLocked(Card * card) {
    return card -> locked;
}

void printCard(Card card) {
    puts("CARD---------------------------");
    printf("Nume: %12s\nPrenume: %12s\nNumar card: %6s\nPin: %4s\nParola secreta: %8s\nSold: %.2lf\n",
        card.nume, card.prenume, card.numar_card, card.pin, card.parola_secreta, card.sold);
    puts("ENDCARD------------------------\n");
}

void printDatabase(Database db) {
    puts("DATABASE-----------------------------------------");
    printf("Cards count: %d\nMax cards: %d\nCards:\n\n", db.length, db.max_size);
    int i = 0;
    for(; i < db.length; i++) {
        printCard(*db.cards[i]);
    }
    puts("ENDDATABASE--------------------------------------\n");
}