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

/*
 * wumpus.c --- a faithful translation of the classic "Hunt The Wumpus" game.
 *
 * Translator: Eric S. Raymond <esr@snark.thyrsus.com>
 * Version: $Id: wumpus.c,v 1.1 2004/12/07 21:11:33 kreator Exp $
 *
 * This was the state of the art 20 years ago, in 1972.  We've come a long
 * way, baby.
 *
 * The BASIC source is that posted by Magnus Olsson in USENET article
 * <9207071854.AA21847@thep.lu.se>: he wrote
 *
 * >Below is the source code for _one_ (rather simple) Wumpus version,
 * >which I found in the PC-BLUE collection on SIMTEL20. I believe this is
 * >pretty much the same version that was published in David Ahl's "101
 * >Basic Computer Games" (or, possibly, in the sequel). 
 *
 * I have staunchly resisted the temptation to "improve" this game.  It
 * is functionally identical to the BASIC version (source for which
 * appears in the comments).  I fixed some typos in the help text.
 *
 * Language hackers may be interested to know that he most difficult thing
 * about the translation was tracking the details required to translate from
 * 1-origin to 0-origin array indexing.
 *
 * The only enhancement is a an -s command-line switch for setting the
 * random number seed.
 *
 * So, pretend for a little while that your workstation is an ASR-33 and
 * limber up your fingers for a trip to nostalgia-land...
 */  
  
//..

#include "shack.h"
#include "user.h"

  
/*       *** HUNT THE WUMPUS ***					*/ 
  
static int path[5];
static int j, k, arrows, scratchloc;
static char inp[BUFSIZ];	/* common input buffer */

#define YOU	0
#define WUMPUS	1
#define PIT1	2
#define PIT2	3
#define BATS1	4
#define BATS2	5
#define LOCS	6
static int loc[LOCS], save[LOCS];	/* locations */

#define NOT	0
#define WIN	1
#define LOSE	-1
static int finished;

static int cave[20][3] = 
  { 
{1, 4, 7}, 
{0, 2, 9}, 
{1, 3, 11}, 
{2, 4, 13}, 
{0, 3, 5}, 
{4, 6, 14}, 
{5, 7, 16}, 
{0, 6, 8}, 
{7, 9, 17}, 
{1, 8, 10}, 
{9, 11, 18}, 
{2, 10, 12}, {11, 13, 19}, {3, 12, 14}, {5, 13, 15}, {14, 16, 19}, {6, 15, 17}, {8, 16, 18}, {10, 17, 19}, {12, 15, 18}, 
};

#define FNA() (rand() % 20) 
#define FNB() (rand() % 3) 
#define FNC() (rand() % 4) 

int move_or_shoot();


int getnum (char *prompt) {
	printf ("%s? [  ]\b\b\b", prompt);
	GetStr(inp,2,NO_SPACE|NO_ATSIGN|NO_DASH);
	return (atoi (inp));
}

int getlet (char *prompt) {
	printf ("%s? [ ]\b\b", prompt);
	inp[0] = GetKey(0);
	printf("%c",toupper(inp[0]));
	printf("\n");
	return (toupper(inp[0]));
}

void print_instructions () {
	char ebuf[BUFSIZ];
  
	printf("\n");
	puts ("WELCOME TO 'HUNT THE WUMPUS'");
 	puts ("~~~~~~~ ~~  ~~~~ ~~~ ~~~~~~");
	puts ("  THE WUMPUS LIVES IN A CAVE OF 20 AREAS. EACH AREA"); 
	puts ("  HAS 3 PATHS LEADING TO OTHER AREAS. (LOOK AT A");
	puts ("  DODECAHEDRON TO SEE HOW THIS WORKS-IF YOU DON'T KNOW");
	puts ("  WHAT A DODECAHEDRON IS, ASK SOMEONE)");
 	puts ("");
	puts (" HAZARDS:");
	puts (" ~~~~~~~~");
	puts (" BOTTOMLESS PITS - TWO AREAS HAVE BOTTOMLESS PITS IN THEM");
	puts ("                   IF YOU GO THERE, YOU FALL INTO THE PIT (& LOSE!)");
	puts ("");	
	puts (" SUPER BATS -      TWO OTHER AREAS HAVE SUPER BATS. IF YOU");
	puts ("                   GO THERE, A BAT GRABS YOU AND TAKES YOU TO SOME");
	puts ("                   OTHER AREA AT RANDOM. (WHICH MAY BE TROUBLESOME)");
	printf("\n");
    
	if(!More()) return;
    
	printf("\n");
	puts (" WUMPUS:");
	puts (" ~~~~~~~");
	puts ("   THE WUMPUS IS NOT BOTHERED BY HAZARDS (HE HAS SUCKER");
	puts ("   FEET AND IS TOO BIG FOR A BAT TO LIFT).  USUALLY");
	puts ("   HE IS ASLEEP.  TWO THINGS WAKE HIM UP: YOU SHOOTING AN");
	puts ("   ARROW OR YOU ENTERING HIS AREA.");
	puts ("");
	puts ("   IF THE WUMPUS WAKES HE MOVES (P=.75) ONE AREA");
	puts ("   OR STAYS STILL (P=.25).  AFTER THAT, IF HE IS WHERE YOU");
	puts ("   ARE, HE EATS YOU UP AND YOU LOSE!");
	printf("\n");
    
	if(!More()) return;
    
	printf("\n");
	puts (" YOU:");
	puts (" ~~~~");
	puts ("   EACH TURN YOU MAY MOVE OR SHOOT A CROOKED ARROW");
	puts ("   MOVING:  YOU CAN MOVE ONE AREA (THRU ONE TUNNEL)");
	puts ("   ARROWS:  YOU HAVE 5 ARROWS.  YOU LOSE WHEN YOU RUN OUT");
	puts ("   EACH ARROW CAN GO FROM 1 TO 5 AREAS. YOU AIM BY TELLING");
	puts ("   THE COMPUTER THE AREA#S YOU WANT THE ARROW TO GO TO.");
	puts ("   IF THE ARROW CAN'T GO THAT WAY (IF NO TUNNEL) IT MOVES");
	puts ("   AT RANDOM TO THE NEXT AREA.");
	puts ("     IF THE ARROW HITS THE WUMPUS, YOU WIN.");
	puts ("     IF THE ARROW HITS YOU, YOU LOSE.");
	printf("\n");
  
 	if(!More()) return;
 
	printf("\n"); 
	puts (" WARNINGS:");
	puts (" ~~~~~~~~~");
	puts ("     WHEN YOU ARE ONE AREA AWAY FROM A WUMPUS OR HAZARD,");
	puts ("     THE COMPUTER SAYS:");
	puts ("");
	puts (" WUMPUS:  'I SMELL A WUMPUS'");
	puts (" BAT   :  'BATS NEARBY'");
	puts (" PIT   :  'I FEEL A DRAFT'");
	puts ("");  
	Pause3();
	return;
}


void check_hazards () {
  
	(void) puts ("");

	for (k = 0; k < 3; k++) {
      		int room = cave[loc[YOU]][k];
		if (room == loc[WUMPUS])
			(void) puts ("I SMELL A WUMPUS!");
      
		else if (room == loc[PIT1] || room == loc[PIT2])
			(void) puts ("I FEEL A DRAFT");
      
		else if (room == loc[BATS1] || room == loc[BATS2])
			(void) puts ("BATS NEARBY!");
	}

	(void) printf ("\x1b[0;35mYOU ARE IN AREA \x1b[36m%d\n", loc[YOU] + 1);
  
	(void) printf ("\x1b[0;32mPATHS LEAD TO \033[1m%d %d %d\n\033[0;33m", cave[loc[YOU]][0] + 1,
		cave[loc[YOU]][1] + 1, cave[loc[YOU]][2] + 1);
  
	(void) puts ("");
}


int move_or_shoot () {
	int c;
  
	badin:
	c = getlet ("\033[0;1;33m(\033[31ms\033[33m)\033[32mhoot \033[34mOR \033[33m(\033[31mm\033[33m)\033[32move \033[34mOR \033[33m(\033[31mq\033[33m)\033[32muit, Huh");
  
	if (c == 'S') return (1);
  
	else if (c == 'M') return (0);
    
	else if (c == 'Q') return (2);
  
	else
		goto badin;
}


void shoot () {
	extern void check_shot (), move_wumpus ();
	int j9;
  
	finished = NOT;
  
	badrange:
	j9 = getnum ("\x1b[1;35mNumber Of Shots \033[33m(\033[31m1-5\033[33m)");
  
	if (j9 < 1 || j9 > 5)
		goto badrange;
  
	for (k = 0; k < j9; k++) {
		path[k] = getnum ("AREA #") - 1;
      		if (k <= 1) continue;
		if (path[k] != path[k - 2]) continue;
      
		(void) puts ("ARROWS AREN'T THAT CROOKED - TRY ANOTHER AREA");
		k--;
	}
  
	scratchloc = loc[YOU];
	for (k = 0; k < j9; k++) {
		int k1;
      		for (k1 = 0; k1 < 3; k1++) {
			if (cave[scratchloc][k1] == path[k]) {
	      
			/*
			 * This is the only bit of the translation I'm not sure
			 * about.  It requires the trajectory of the arrow to
			 * be a path.  Without it, all rooms on the trajectory
			 * would be required by the above to be adjacent to the
			 * player, making for a trivial game --- just move to where
			 * you smell a wumpus and shoot into all adjacent passages!
			 * However, I can't find an equivalent in the BASIC.
			 */ 
			scratchloc = path[k];
	      
			/* this simulates logic at 895 in the BASIC code */ 
			check_shot ();
			if (finished != NOT) return;
			}
	  
		}
		scratchloc = cave[scratchloc][FNB ()];
		check_shot ();
	}
	ammo:if (finished == NOT)
    
	{
		(void) puts ("\033[0;1;35mMISSED\033[0m");
		scratchloc = loc[YOU];
		move_wumpus ();
		if (--arrows <= 0) finished = LOSE;
	}
  
}


void check_shot () {
  
	if (scratchloc == loc[WUMPUS]) {
		(void) puts ("\x1b[1;33mAHA! YOU GOT THE WUMPUS!\x1b[0;32m");
		finished = WIN;
	} else if (scratchloc == loc[YOU]) {
		(void) puts ("\x1b[1;33mOUCH! ARROW GOT YOU!\x1b[0;32m");
		finished = LOSE;
	}
}


void move_wumpus () {
  
	k = FNC ();
	if (k < 3) loc[WUMPUS] = cave[loc[WUMPUS]][k];
	if (loc[WUMPUS] != loc[YOU]) return;
	(void) puts ("TSK TSK TSK - WUMPUS GOT YOU!");
	finished = LOSE;
}


void move () {
	finished = NOT;
	badmove:
	scratchloc = getnum ("\x1b[1;35mWhere To");
  
	if (scratchloc < 1 || scratchloc > 20) goto badmove;
	scratchloc--;
  	for (k = 0; k < 3; k++) {
		if (cave[loc[YOU]][k] == scratchloc)
		goto goodmove;
	}
  
	if (scratchloc != loc[YOU]) {
		(void) puts ("NOT POSSIBLE -");
		goto badmove;
	}
	goodmove:
	loc[YOU] = scratchloc;
	if (scratchloc == loc[WUMPUS]) {
		(void) puts ("... OOPS! BUMPED A WUMPUS!");
		move_wumpus ();
	} else if (scratchloc == loc[PIT1] || scratchloc == loc[PIT2]) {
		(void) puts ("YYYYIIIIEEEE . . . FELL IN PIT");
		finished = LOSE;
	} else if (scratchloc == loc[BATS1] || scratchloc == loc[BATS2]) {
		printf("ZAP--SUPER BAT SNATCH!\n");
		printf("\x1b[0m\n");
          	printf("   (_    ,_,    _)\n");
		printf("   / `'--) (--'` \\\n");
		printf("  /  _,-'\\_/'-,_  \\\n");
		printf(" /.-'     \"     '-.\\\n");
		printf("\x1b[0;32m\n"); 
                printf("ELSEWHEREVILLE FOR YOU!\n");
		scratchloc = loc[YOU] = FNA ();
		goto goodmove;
	}
}



int Wumpus(void) {
	int c, ret, end=0;
	srand ((int) time ((long *) 0));    

  
	printf("\x1b[0m\n");
	badlocs:
	for (j = 0; j < LOCS; j++) loc[j] = save[j] = FNA ();
  
	for (j = 0; j < LOCS; j++)
		for (k = 0; k < LOCS; k++)
			if (j == k)
				continue;
  
	else if (loc[j] == loc[k])
		goto badlocs;
  
	newgame:
	arrows = 5;
	scratchloc = loc[YOU];

	printf("\033[1;33mYOU ARE IN A WUMPUS SWAMP\n");
	printf("\n");
	printf("  \033[0;35mA posted sign reads\033[1;31m,\n");
	printf("     \"\033[33mHunt the wumpus and you shall grow\n");
	printf("     rich\033[31m,\033[33m so rich\033[31m,\033[33m beond your wildest\n");
	printf("     dreams\033[31m,\033[33m Hahaha haha\033[31m!\"\n");
  
	#ifdef DEBUG
	(void) printf ("Wumpus is at %d, pits at %d & %d, bats at %d & %d\n", loc
		   [WUMPUS] + 1, loc[PIT1] + 1, loc[PIT2] + 1,
		   loc[BATS1] + 1, loc[BATS2] + 1);
  
	#endif	/*  */

	nextmove:
	check_hazards ();
  
	ret = move_or_shoot();
	if(ret == 1) {
		shoot ();
		if (finished == NOT) goto nextmove;
	} else if(ret == 0) {
		move ();
		if (finished == NOT) goto nextmove;
	} else if(ret == 2) {
		/* the end */
		return TRUE;
	}
  
	if (finished == LOSE) {
		printf("\x1b[0;33mHA HA HA - YOU LOSE!\n\n");
		printf("       +            +            +\n");
		printf("     .-\"-.        .-:-.        .-\"-.\n");
		printf("    / RIP \\      / RIP \\      / RIP \\\n");
		printf("    |     |      |     |      |     |\n");
		printf("   \\\\     |//  \\\\\\     |//  \\\\\\     |//\n");
		printf("jgs ` \" \"\" \"    ` ' \"\" \"     \" '  \"\"\" \"\n");
                                    
		end = TRUE;		
	} else {
		(void) puts ("\x1b[1;33mHEE HEE HEE - THE WUMPUS'LL GET YOU NEXT TIME!!");
		end = FALSE;
	}

	for (j = YOU; j < LOCS; j++) loc[j] = save[j];
  
	return end;
}


/**********************************************************/
/*              The Hunt The Wumpus Add Ons               */
/*                     version o.2                        */   
/**********************************************************/


/* wumpus.c ends here */ 
