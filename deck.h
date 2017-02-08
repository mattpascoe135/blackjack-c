#ifndef _DECK_H_
#define _DECK_H_

/** Written by Matt Pascoe
  *
  * Header file for the deck.c
  *
  */
#define NUMBER_OF_SUITS 4
#define NUMBER_OF_VALUES 13

#include <stdio.h>
#include <stdlib.h>

struct card_t {
    int suit;
    int value;
};

typedef struct card_t card;

struct deck_t {
    int num_cards;
    int deck_index;
    card *cards;
};

//struct card_t;
//struct deck_t;

typedef struct deck_t deck;

void init_card(card *newCard, int suit, int value);
deck *init_deck(void);
void free_deck(deck *d);
card *draw_card(deck *d);
void shuffle_deck(deck *d);


#endif
