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
 * Records used in the game
 * - these should be conplete
 *   for further expansion.
 */



typedef struct Player {
	char shipname[31];
	char shipbrand[31];
	
	/* gernal turns used to limit action per day */
	int turns_left;
	
	/* warping capablities of the ship */
	int max_warps;
	int warps_left;
	int turns_per_warp;
	
	/* defence and offence of the ship */
	int fighters;
	int shields;
	int ship_hits;	
	int cargo_bays;

	int max_fighters;
	int max_shields;
	int max_ship_hits;
	int max_cargo_bays;
	
	/* personal stats */	
	int phits;
	int max_phits;
	int chr;
	int str;
	int dex;
	int alingment;
	
	/* other stats */
	unsigned long loan;
	unsigned long credit_chips;
	unsigned long score;
	unsigned long time_lastin;
} PLAYER; 

PLAYER player;

void StartGame();

void PlayerList(void);
void view_player_stat();
int get_player();
int update_player();
int mkplayer();
int exists_player_state();
		
