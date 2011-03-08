/***********
***********
**
**  Copyright (c) 1999 by Jon Bradley
**  Homebrew is distributed under the terms of the GNU General Public License.
**
**    This file is part of Homebrew-bbs.
**
**    Homebrew-bbs is free software: you can redistribute it and/or modify
**    it under the terms of the GNU General Public License as published by
**    the Free Software Foundation, either version 3 of the License, or
**    (at your option) any later version.
**
**    Homebrew is distributed in the hope that it will be useful,
**    but WITHOUT ANY WARRANTY; without even the implied warranty of
**    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**    GNU General Public License for more details.
**
**    You should have received a copy of the GNU General Public License
**    along with Homebrew.  If not, see <http://www.gnu.org/licenses/>.
**
***********
************/

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "shack.h"
#include "user.h"

#define	NDEALS		3000	/* number of deals */
#define NPLAYERS	2	/* number of players */

typedef enum {clubs, diamonds, hearts, spades}	cdhs;

struct card {
	int	pips;
	cdhs	suit;
};

typedef	struct card	card;

card assign_values(int pips, cdhs suit);
void prn_card_values(card *c_ptr);
int  play_poker(card deck[52]);
void shuffle(card deck[52]);
void swap(card *p, card *q);
void deal_the_cards(card deck[52], card hand[NPLAYERS][5]);
void prn_faceup_hand(card h[5]);
void prn_facedown_hand(void);
int  is_flush(card h[5]);
int  is_kind(card h[5]);
int  is_single(card h[5]);
int  is_straight(card h[5]);
int  TopCard(card h[5]);

int Poker(void) {
	cdhs	suit;
	int	i, pips;
	card	deck[52];
	
	for (i = 0; i < 52; ++i) {
		pips = i % 13 + 1;
		if (i < 13)
			suit = clubs;
		else if (i < 26)
			suit = diamonds;
		else if (i < 39)
			suit = hearts;
		else
			suit = spades;
		deck[i] = assign_values(pips, suit);
	}
	play_poker(deck);
	return 0;
}

card assign_values(int pips, cdhs suit) {
	card c;
	
	c.pips = pips;
	c.suit = suit;
	return c;
}

void prn_card_values(card *c_ptr) {
	int	pips = c_ptr -> pips;
	cdhs	suit = c_ptr -> suit;
	char	suit_name[10];
	
	
	if (suit == clubs)
		strcpy(suit_name,"clubs");
	else if (suit == diamonds)
		strcpy(suit_name,"diamonds");
	else if (suit == hearts)
		strcpy(suit_name,"hearts");
	else if (suit == spades)
		strcpy(suit_name,"spades");
	printf("card: %2d of %s\n", pips, suit_name);
}

void prn_facedown_hand(void) {
	printf("\x1b[1;34m +-----+  +-----+  +-----+  +-----+  +-----+\n");
	printf(" |\x1b[0;33m#####\x1b[1;34m|  |\x1b[0;33m#####\x1b[1;34m|  |\x1b[0;33m#####\x1b[1;34m|  |\x1b[0;33m#####\x1b[1;34m|  |\x1b[0;33m#####\x1b[1;34m|\n");
	printf(" |\x1b[0;33m#####\x1b[1;34m|  |\x1b[0;33m#####\x1b[1;34m|  |\x1b[0;33m#####\x1b[1;34m|  |\x1b[0;33m#####\x1b[1;34m|  |\x1b[0;33m#####\x1b[1;34m|\n");
	printf(" |\x1b[0;33m#####\x1b[1;34m|  |\x1b[0;33m#####\x1b[1;34m|  |\x1b[0;33m#####\x1b[1;34m|  |\x1b[0;33m#####\x1b[1;34m|  |\x1b[0;33m#####\x1b[1;34m|\n");
	printf(" +-----+  +-----+  +-----+  +-----+  +-----+\n\x1b[0m");
}

void prn_faceup_hand(card h[5]) {

	char c[5][3];
	char s[5][2];	
	int  i;

	for(i=0;i<5;++i) {
		switch(h[i].pips) {
		case 1: strcpy(c[i],"A "); break;
		case 2: strcpy(c[i],"2 "); break;		
		case 3: strcpy(c[i],"3 "); break;		
		case 4: strcpy(c[i],"4 "); break;		
		case 5: strcpy(c[i],"5 "); break;		
		case 6: strcpy(c[i],"6 "); break;		
		case 7: strcpy(c[i],"7 "); break;		
		case 8: strcpy(c[i],"8 "); break;		
		case 9: strcpy(c[i],"9 "); break;		
		case 10: strcpy(c[i],"10"); break;		
		case 11: strcpy(c[i],"J "); break;		
		case 12: strcpy(c[i],"Q "); break;		
		case 13: strcpy(c[i],"K "); break;
		default: strcpy(c[i],"EE"); break;		
		}	
	}
	
	for(i=0;i<5;++i) {
		if (h[i].suit == clubs)
			strcpy(s[i],"C");
		else if (h[i].suit == diamonds)
			strcpy(s[i],"D");
		else if (h[i].suit == hearts)
			strcpy(s[i],"H");
		else if (h[i].suit == spades)
			strcpy(s[i],"S");
	}
	
	printf("\x1b[1;34m +-----+  +-----+  +-----+  +-----+  +-----+\n");
	printf(" | \x1b[33m%s\x1b[34m  |  | \x1b[33m%s\x1b[34m  |  | \x1b[33m%s\x1b[34m  |  | \x1b[33m%s\x1b[34m  |  | \x1b[33m%s\x1b[34m  |\n",c[0],c[1],c[2],c[3],c[4]);
	printf(" |     |  |     |  |     |  |     |  |     |\n");
	printf(" |   \x1b[33m%s\x1b[34m |  |   \x1b[33m%s\x1b[34m |  |   \x1b[33m%s\x1b[34m |  |   \x1b[33m%s\x1b[34m |  |   \x1b[33m%s\x1b[34m |\n",s[0],s[1],s[2],s[3],s[4]);
	printf(" +-----+  +-----+  +-----+  +-----+  +-----+\n");
}

int play_poker(card deck[52]) {
	int	flush_cnt = 0, hand_cnt = 0;
	int	i, j, x, y, done = FALSE, fold = FALSE, menu = TRUE;
	long	bet=0, pot=0;
	char    ch, buf[128];
	card	hand[NPLAYERS][5];	/* each player is dealt 5 cards */
	
	srand(time(NULL));	/* seed the random number generator */
	shuffle(deck);
	deal_the_cards(deck, hand);
	bet = 10;
	printf("Antie is $%ld\n\n", bet);
	if(character.trading_credits < 10) {
		printf("You don't even have enough to Antie.\n");
		printf("The dealer kicks you out of the game,\n");
		printf("And says \"Go Wumpus Hunting!\"\n\n");
		return 0;
	} else {
		printf("Welcome to poker.\n");
		printf("Game is five card, no draw.\n");
		printf("\nYou against the dealer.\n");
		printf("The dealer Anties 10 dollars.\n\n");
		character.trading_credits -= 10;
		pot += 20;
		Pause2();
	}
	
	while(!done) {
		if(menu) {
			printf("\nDealer's Hand\n");
			prn_facedown_hand();
			printf("\nYour Hand\n");			
			prn_faceup_hand(hand[1]);
   			printf("\x1b[0;35m    1        2        3        4        5\x1b[0m\n");

			printf("\n(r)=raise, (c)=call, (f)=fold\n");
        	}
        	printf("\nPot is %ld gold\n",pot);
        	printf("[(?)=help] -> ");
        	for(;;) {
        		/*****************/
        		/* get keys here */
        		/*****************/
			ch = GetKey(0);
			if(ch == '1' ||
			ch == '2' ||
			ch == '3' ||
			ch == '4' ||
			ch == '5' ||
			ch == 'f' ||
			ch == 'q' ||
			ch == 'r' ||
			ch == 'c' ||
			ch == '?') break;
        	}
        	switch(tolower(ch)) {
		case 'r':
			printf("raise..\n\n");
			
			printf("You have $%d in pocket\n\n", character.trading_credits);
			printf("Raise how much? ");
			GetStr(buf, 10, 0);
			bet = atol(buf);
			if(bet == 0) {
				printf("\nYou can't raise by zero trading_credits, try calling.\n\n");
				done = TRUE;
				break;
			} else if(character.trading_credits >= bet) {
				printf("\nBet Amount is now $%d\n", bet);
				character.trading_credits -= bet;
				pot += (bet * 2);
				printf("Dealer see's you and calls!\n");
				/***********************************/
				/* here the dealer has to decide   */
				/* if he is going to raise or call */
				/* the bet.                        */
				/***********************************/
			} else {
				printf("\nYou don't have enough trading_credits!\n");
			}
			menu = FALSE;
			done = FALSE;
			fold = FALSE;
			break;
		case 'c':
			printf("call..\n");
			
			printf("\nDealer calls as well.\n");
			/****************************/
			/* what does the dealer do? */
			/****************************/
			menu = FALSE;
			done = TRUE;
			fold = FALSE;
			break;
        	case 'q':
        	case 'f':
        		printf("fold..\n\n");
        		printf("\x1b[0mDealer's Hand\n");
        		prn_facedown_hand();
        		printf("\x1b[0mYour Hand\n");
        		prn_facedown_hand();
        		printf("\nYou lost a pot of %ld gold to the dealer.\n", pot);
        		
        		fold = TRUE;
        		menu = FALSE;
        		done = TRUE;
        		break;
		case '1':
		case '2':
		case '3':
		case '4':
		case '5':
			y = atoi(&ch);
			printf("\x1b[1;32mremove card #%d\n\n", y);
			/********************************************/
			/* questions:                               */
			/* how to change a character to an integer? */
			/* how to take one card from the hand, then */
			/* add another card from deck, to the hand? */
			/********************************************/
			printf("\x1b[0mYou can't do that in this game.\n");
			done = FALSE;
			menu = FALSE;
			fold = FALSE;
			break;
		case '?':
			printf("\x1b[0mhelp..\n\n");
			menu = TRUE;
			done = FALSE;
			fold = FALSE;
			break;
		default:
			break;
		}
	}
	
	if(!fold) {
		int dc, yc;
		printf("Dealer: ");
		dc = TopCard(hand[0]);
		printf("   YOU: ");
		yc = TopCard(hand[1]);
		
		printf("\n\x1b[0mDealer's Hand\n");
		prn_faceup_hand(hand[0]);
         	printf("\n\x1b[0mYour Hand\n");
 		prn_faceup_hand(hand[1]);        	
		printf("\x1b[0m\n");
		
		if(dc == yc) {
			printf("A draw. (pot is split)\n");
			character.trading_credits += (pot/2);
		} else if(dc > yc) {
			printf("Dealer wins!\n");
		} else if(dc < yc) {			
			printf("You win!\n");
			printf("Awarding %d gold and %d exp point.\n",pot,pot/2);
			character.trading_credits += pot;
			character.exp += pot / 2;
		}
	}
	update_empire();				
	return 0;
}


void shuffle(card deck[52]) {
	int i, j;
	
	for(i = 0; i < 52; ++i) {
		j = rand() % 52;
		swap(&deck[i], &deck[j]);
	}
}

void swap(card *p, card *q) {
	card tmp;
	
	tmp = *p;
	*p = *q;
	*q = tmp;
}

void deal_the_cards(card deck[52], card hand[NPLAYERS][5]) {
	int card_cnt = 0, i, j;
	
	for (j = 0; j < 5; ++j)
		for (i = 0; i < NPLAYERS; ++i)
			hand[i][j] = deck[card_cnt++];
}



/* These functions say which hand wins */

int is_flush(card h[5]) {
	int i;
	
	for (i = 1; i < 5; ++i)
		if (h[i].suit != h[0].suit)
			return 0;
	return 1;
}


/************************************/
/* function checks if is a straight */
/* not 100% sure on the bubble sort */
/* but it seems to work ok......    */
/************************************/
int is_straight(card h[5]) {
	int i,j;
	
	for(i=0;i<5;++i)
		for(j=4;j>i;--j)
			if(h[j-1].pips < h[j].pips) 
				swap(&h[j-1],&h[j]);
	for(i=0;i<5;++i)
		if(h[i].pips != (h[i+1].pips + 1))
			return 0;
	return 1;
} 				


	
int is_kind(card h[5]) {
	int i,j,y=0;
	int x = 0;

	for(j=0;j<5;++j) {
		for(i=j+1;i<5;++i) {
			if(i == j) continue;
			if(h[j].pips == h[i].pips) {
				x++;
				y=h[j].pips;
			}
		}
	}
	switch(x) {
		case 0: x=h[0].pips;    /** probably one of a kind **/
			for(i=0;i<5;++i)
				for(j=0;j>5;++j)
					if(h[j].pips > h[i].pips)
						if(h[j].pips > x) 
							x=h[j].pips;
			break;
		case 1: x = y+14; /** two of a kind **/
			break;
		case 2: x = y+28; /** two pair **/
			break;
		case 3: x = y+42; /** three of a kind **/
			break;
		case 4: x = y+56; /** full house **/
			break;
		case 6: x = y+70; /** four of a kind **/
			break;
		default: x = 0;
			break;
	}
	return x;	
}



int TopCard(card h[5]) {
	int x = 0;
			
	if(is_flush(h) && is_straight(h)) {	
		printf("Straight Flush.\n");
		x = 300;
	}
	else if(is_flush(h)) {
		printf("Flush.\n");
		x = 200;
	}
	else if(is_straight(h)) {
		printf("Straight.\n");
		x = 150;
	}
	else if(x = is_kind(h)) {
		printf("(%d)x of a kind.\n",x);
	}
	else { x = 0; }
	
	return x;
}
