/* Includes */
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdbool.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pthread.h>

#include "deck.h"


/* Defines */
#define MAX_CARDS_IN_HAND 10

//Constant strings
#define HELP "Main menu, enter the following:\r\n\tn: Start a new Game\r\n\ts: Session statistics\r\n\tq: Quit game\r\n\tr: Reset statistics and start a new game\r\n\0"
#define HELP_ACTION "Available commands:\r\n\th: Hit\r\n\ts: Stand\r\n\tv: Split\r\n\td: Double Down\r\n\tq: Surrender\r\n\0"
#define INVALID "Invalid command, please enter a valid command.\r\n\0"
#define STATS "Your statistics:\r\n\tCash %d\r\n\tGames played: %d\r\n\tWin/Loss Ratio: %f\r\n\0"
#define EXIT "Goodbye, thanks for playing...\r\n\0"
#define NEWGAME "Starting new game...\r\n\0"
#define INVALID_BET "Invalid bet, not enough cash...\r\n\0"
#define BUST "Bust\r\nYour hand is over 21\r\n\0"
#define DRAW "Draw\r\nBoth you and the house have the same value hand. Returning bet\r\n\0"
#define WIN "Congratulations you won. "


/* Typedefs & structures */
typedef enum state_t {
  LOBBY,
  INGAME,
  QUIT,
  WAIT
} state;

typedef struct hand_t {
  int value;
  int numberOfCardsInHand;
  card **cards;
} hand;

typedef struct game_t {
  int cash;
  int playerBet;
  int numberOfGames;
  int wins;
  deck *d;
  hand *client;
  hand *server;
} game;



/** Initalise a players new hand
  *
  * Parameters:
  *     void
  *
  * Returns:
  *     hand *h         - initalised hand
  */
hand *init_hand() {
    int i;

    hand *h = malloc(sizeof(hand));
    h->value = 0;
    h->numberOfCardsInHand = 0;
    h->cards = malloc( sizeof(card) * MAX_CARDS_IN_HAND);

    for(i=0; i<MAX_CARDS_IN_HAND; i++) {
        h->cards[i] = NULL;
    }

    return h;
}



/** Evaluates the given hand to determine the value of
  * the players hand.
  *
  * Parameters:
  *     hand *h             - players hand
  *
  * Returns:
  *     int value           - value of the players hand
  */
int evaluate_hand(hand *h){
    int i, value=0, cardVal=0;
    int aceCards=0;

    for(i=0; i<h->numberOfCardsInHand; i++) {
        cardVal = h->cards[i]->value;
        if(cardVal > 10) {
            cardVal = 10;
        }
        if(cardVal == 1) {
            aceCards++;
        } else {
            value += cardVal;
        }
    }

    //Select the best option for ACE card's
    while(aceCards != 0) {
        if(aceCards == 1 && value < 10) {
            value += 11;
        } else {
            value += 1;
        }
        aceCards--;
    }
    return value;
}



/** Initalises a game of blackjack
  *
  * Parameters:
  *     int cash            - amount of starting cash for player
  *
  * Returns:
  *     game *g             - pointer to initalised game
  */
game *init_game(int cash) {

    game *g = malloc(sizeof(game));

    g->cash = cash;
    g->playerBet = 0;
    g->numberOfGames = 0;
    g->wins = 0;

    g->d = init_deck();

    return g;
}



/** Adds card to hand and returns 
  *
  * Parameters:
  *     hand *h             - hand to add card to
  *     deck *d             - deck to draw card from
  *
  * Returns:
  *     int                 - 0, if hand is under 21
  *                         - 1, if hand is 21
  *                         - 2, if hand is bust
  */
int add_card_to_hand(hand *h, deck *d) {
  int value;

  //Draw card
  if(h->numberOfCardsInHand >= 10) {
    return 1;
  }
  h->cards[h->numberOfCardsInHand++] = draw_card(d);

  //Evaluate hand
  value = evaluate_hand(h);
  if(value < 21) {
    return 0;
  } else if(value == 21){
    return 1;
  } else {
    return 2;
  }
}



/** 
  * 
  * 
  * 
  * 
  * 
  */
char *process_message(char *message, state currState, game *g) {
    char *returnMsg = NULL;

    if(message[1]!='\r' || message[1]!='\n') {
        return INVALID;
    }

    switch(currState){
      /* Main state for setting up the game */
      case LOBBY:
        switch(message[0]) {
          case 'n':   /* New Game */    
            //Take bet
            if(g->cash < g->playerBet) {
              return INVALID_BET;
            }
            g->cash -= g->playerBet;

            //Draw the cards for both players
            add_card_to_hand(g->client, g->d);
            add_card_to_hand(g->server, g->d);
            add_card_to_hand(g->client, g->d);
            add_card_to_hand(g->server, g->d);

            //Check if either player has 21

            //Move to action state
            currState=INGAME;
            break;

          case 'b':   /* Set bet */
            
            break;

          case 's':   /* Get game statistics */
            
            return STATS;

          case 'q':
            //free game
            currState=QUIT;
            return EXIT;

          case 'r':
            break;

          default:
            return INVALID;
        }
        break;

      case INGAME:
        /* In-game state for playing the game */
        switch(message[0]) {
          case 'h':
            //Draw a card
            switch(add_card_to_hand(g->client, g->d)) {
              case 0:
                break;
              case 1:
                return WIN;
                break;
              case 2:
                return BUST;
                break;
            }
            break;

          case 's':
            //evaluate hand
            //draw cards for house
            //compare hands to determine winner
            break;

          case 'v':
            break;

          case 'd':
            break;

          case 'q':
            break;

          default:
            return INVALID;
        }
        break;

      case WAIT:
        break;

      case QUIT:
        break;

      default:
        printf("Invalid state, returning to menu...");
        currState = WAIT;
    }

    return returnMsg;
}



/** 
  * 
  * 
  * 
  * 
  */
void *client_handler(void *socket_desc) {
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    int read_size, cash=100;
    char *message, client_message[200];
    game *g;
    state s = WAIT;

    printf("Started handler\r\n");

    //Send intro messages to the client
    message = "Welcome to the Blackjack server\r\n";
    write(sock, message, strlen(message));
    write(sock, HELP, strlen(HELP));

    //Start game server
    g = init_game(cash);
    while( (read_size=recv(sock, client_message, 200, 0)) > 0){
      printf("Message received: %s\r\n", client_message);
      fflush(stdout);

      message = process_message(client_message, s, g);
      write(sock, message, strlen(message));
    }

    if(read_size == 0) {
        puts("Client has disconnected from server");
        fflush(stdout);
    } else if(read_size == -1) {
        perror("recv failed");
    }

    free(socket_desc);

    return (void *)NULL;
}




/** 
  * 
  * 
  * 
  * 
  * 
  */
int main(int argc, char *argv[]) {
    int socket_desc, client_sock, c, *new_sock;
    struct sockaddr_in server, client;
   
    //Create socket
    socket_desc = socket(AF_INET, SOCK_STREAM, 0);
    if(socket_desc == -1) {
        printf("Could not create socket");
    }

    //Prepare the sockaddr_in structure
    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(8888);

    //Bind
    if(bind(socket_desc, (struct sockaddr *)&server, sizeof(server)) < 0) {
        //print the error message
        perror("bind failed. Error");
        return 1;
    }

    //Listen
    listen(socket_desc, 3);

    //Accept and incoming connection
    printf("Server is ready...\r\nWaiting for incoming connections...\r\n");
    c = sizeof(struct sockaddr_in);
    while((client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c))) {
        puts("Connection accpeted");
       
        pthread_t sniffer_thread;
        new_sock = malloc(1);
        *new_sock = client_sock;

        if(pthread_create(&sniffer_thread, NULL, client_handler, (void*)new_sock)<0) {
            perror("Could not create thread");
            return 1;
        }

        //Join the thread
        pthread_join(sniffer_thread, NULL);
        puts("Handler assigned");
    }

    if(client_sock < 0) {
        perror("Accept failed");
        return 1;
    }

    return 0;
}
