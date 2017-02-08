/* Includes */
#include "deck.h"


/** Initalises a given card with a provided suit and value.
  *
  * Parameters:
  *     card *newCard       - card to be initalised
  *     int suit            - suit of the card
  *     int value           - value of the card
  *
  * Returns:
  *     card *newCard       - initialised card 
  */
void init_card(card *newCard, int suit, int value) {
    newCard->suit = suit;
    newCard->value = value;
}


/** Initalises the deck with an ordered deck
  *
  * Parameters:
  *     void
  *
  * Returns:
  *     deck *d             - initalised deck
  */
deck *init_deck() {
    int i, j;
    deck *d;
    
    d = malloc(sizeof(deck));
    d->num_cards = NUMBER_OF_SUITS*NUMBER_OF_VALUES;
    d->deck_index = 0;
    d->cards = malloc( (sizeof(card)) * (d->num_cards));
    
    for(i=0; i<NUMBER_OF_SUITS; i++) {
        for(j=0; j<NUMBER_OF_VALUES; j++) {
            init_card(&d->cards[i*NUMBER_OF_VALUES + j], i, j+1);
        }
    }

    return d;
}


/** Releases the deck
  *
  * Parameters:
  *     deck *d             - a deck
  *
  * Returns:
  *     void
  */
void free_deck(deck *d) {
    if(d != NULL) {
        free(d->cards);
    }
    free(d);
}


/** Draws a card from the given deck
  *
  * Parameters:
  *     deck *d             - a deck
  *
  * Returns:
  *     card *c             - if deck is not empty, returns drawn card
  *     NULL                - if deck is empty, returns NULL
  */
card *draw_card(deck *d) {
    if(d->deck_index >= d->num_cards) {
        //If deck is empty then shuffle and draw
        shuffle_deck(d);
    }
    return &d->cards[d->deck_index++];
}


/**
  *
  */
void shuffle_deck(deck *d) {
    int i, j;
    card tmp;

    printf("Shuffling deck\r\n");
    for(i=0; i<d->num_cards; i++) {
        j = rand() % d->num_cards;

        //Iterate through deck, randomly swapping cards
        tmp = d->cards[i];
        d->cards[i] = d->cards[j];
        d->cards[j] = tmp;
    }

    d->deck_index = 0;      //Reset index to start of deck
}
