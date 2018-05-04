#include <stdlib.h>
#include <stdio.h>
#include <string.h>
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

        fscanf(file, "%12s %12s %6s %4s %8s %lf",
            &new_card -> nume, &new_card -> prenume,
            &new_card -> numar_card, &new_card -> pin,
            &new_card -> parola_secreta, &new_card ->sold);

        db -> cards[i] = new_card;
    }

    fclose(file);
}

Card * getCard(Database * db, char numar_card[6]) {
    int i = 0;

    for (; i < db -> length; i++) {
        if (strncmp(numar_card, db -> cards[i] -> numar_card, 6) == 0) {
            return db -> cards[i];
        }
    }

    return NULL;
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