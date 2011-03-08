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

#include "shack.h"
#include "game.h"

// this is just a non-comment - testing cvs - delete this line

void planet_menu(void);

void StartGame() {
	
	int i;
	char obuf[256],ibuf[256];
	struct tm *tp;
	time_t t;

	CatFileNoPause("gamelogo");
	printf("\n");			
	srand(time(NULL));
	
	if(!exists_player_state())  mkplayer();

	if(get_player() == -1) mkplayer();
	else printf("\033[0;31mRetrieving Player Archives...\n");

	printf("\033[0;35mIni\033[1mtia\033[0;35mlizing..\033[1m..\n\n");
	printf("\033[0;35mCopyleft (\033[0;36mC\033[0;35m) \033[0;36m2oo1\n");
	printf("\033[0;32mDesigned and Authored by KrEaToR!\n");
	printf("\033[1;31mtelnet://toga.cx\n");
	printf("\033[0;1;37;44m%s\033[0;31;44m, Welcome to \033[0;1;33;44mSTaR FLiGhT! \033[0m\n",get_user_name());
	t = time(NULL);
	tp = localtime(&t);
	printf("\033[0;35mStardate\033[1;34m:\033[0;36m %s",asctime(tp));
	fflush(stdout);


	if((player.shipname[0] == '\0')
	|| (!strcmp(player.shipname,get_user_name()))) {
		for(;;) {
			printf("\n");
			printf("\033[0;36mWhat name shall be your \033[1mnew Ship\033[0;36m?\n");
			printf("\033[1;36m[-\033[0;36m------------------\033[1m-]\n\033[0;36m:\033[1m");
			fflush(stdout);
			GetStr(player.shipname,20,0);
			if(strlen(player.shipname) < 3) {
				printf("\n\033[1;33mToo short!\n");
				continue;
			}
			printf("\n\033[0;36m%s\033[0;35m, is this correct? [\033[1;31my,n\033[0;35m]? ",player.shipname);
			for(;;) { i = GetKey(0); if(i=='y'||i=='n') break; }
			printf("%c\n",i);
			if(i=='y') break;
		}
	}
	printf("\n");
	printf("\033[0;35mView the diary? \033[1;33m[\033[0;31mYes\033[1;33m]: ");
	i = GetKey(0);
	printf("\033[1;31mNo..\n");
	view_player_stat();
	update_player();
	planet_menu();
	printf("\n\033[0mReturning to bBS");
	update_player();
}



void planet_menu(void) {
	FILE *fp;	
	char ch,done=FALSE,menu=TRUE,pmt=TRUE;

	printf("\n");
	while(!done) {
		if(player.turns_left<0) player.turns_left=0;
		if(player.warps_left<0) player.warps_left=0;
		if(menu) {

			printf(" \033[0;44m    \033[1;33;41m SHiP CoNtRoL'S ANd CoMPuTeR \033[0;44m    \033[0;40m\n");
			printf("\033[1;34m.====\033[32m<\033[0;35mComputer\033[1;32m>\033[34m====.===\033[32m<\033[0;35mNavagation\033[1;32m>\033[34m===.\n");
			printf("\033[1;34m| \033[35mS\033[33m]\033[32mcan an Object  \033[34m| \033[35mM\033[33m]\033[32map the Planets \033[34m|\n");
			printf("\033[1;34m| \033[35mU\033[33m]\033[32mplink Console  \033[34m| \033[35mF\033[33m]\033[32mind Warp Path  \033[34m|\n");
			printf("\033[1;34m| \033[35mE\033[33m]\033[32mxamine Logs    \033[34m| \033[35mW\033[33m]\033[32marp!           \033[34m|\n");
			printf("\033[1;34m.===\033[32m<\033[0;35mMore Stuff\033[1;32m>\033[34m===:=\033[32m<\033[0;35mGeneral Option\033[1;32m>\033[34m=.\n");
			printf("\033[1;34m| \033[35mA\033[33m]\033[32mttack a Player \033[34m| \033[35mL\033[33m]\033[32mist of Players \033[34m|\n");
			printf("\033[1;34m| \033[35mV\033[33m]\033[32miew Status     \033[34m| \033[35mQ\033[33m]\033[31muit to bbs     \033[34m|\n");
			printf("\033[1;34m.=\033[32m<\033[0;35mPlanet Actions\033[1;32m>\033[34m=:=\033[32m<\033[0;35mPlanet Actions\033[1;32m>\033[34m=.\n");
			printf("\033[1;34m| \033[30m1] Trade Goods   \033[34m| \033[30m2] Galactic Bank \033[34m|\n");
			printf("\033[1;34m| \033[30m3] Dock Shipyard \033[34m| \033[30m4] BeamTo Pub    \033[34m|\n");
			printf("\033[1;34m| \033[35m5\033[33m] \033[32mBeamTo Swamps \033[34m| \033[30m6] BeamTo Hills  \033[34m|\n");
			printf("\033[1;34m`==================:=================='\n");
			printf("\n");
			printf("\033[1;36m%s \033[1;32mstatus\033[1;34m: in Orbit \033[0;35m(%s\033[0;35m)\n\n",player.shipname,"\033[1;33mArticoa"); 
		}
		if(pmt) printf("\033[0;35m<\033[1;31mW:%d\033[0;35m> <\033[1;31mT:%d\033[0;35m> [\033[1;34mConsole\033[0;35m]\033[1;34m \033[0;35m(\033[1;31mH\033[0;35m)\033[1;31melp\033[33m? ",player.warps_left,player.turns_left);
		ch = GetKey(0);
		switch(ch) {
		case 'L':
		case 'l':
			printf("List\n\n");
			PlayerList();
			printf("\n");
			menu=FALSE;
			done=FALSE;
			pmt=TRUE;
			break;
		case '5':
			menu=TRUE;
			done=FALSE;
			pmt=TRUE;
			printf("Beam\n\n");
			if(player.turns_left < 5) {
				printf("\033[1;32mYou don't have enough turns left.\n\n");
				menu=FALSE;
				break;
			}
			printf("\033[0;32mPacking up weapons and supplies..\n");
			fflush(stdout); sleep(1);
			printf("\033[1;31m \"\033[33mTransporters ready, sir!\033[31m\"\n");
			fflush(stdout); sleep(1);
			printf("\033[1;31m   \"\033[33meNeRgiZe!!\033[31m\", yells the captain.\n\n");
			PrintSlow("\033[0;41m  \033[0;1;37;44m Beaming To The Swamps. \033[0;41m  \033[0;40m");
			sleep(1);
			printf("\n\n");
			Pause3();
			if(Wumpus() == TRUE) {
				printf("\n");
			} else {
				printf("\n");
				printf("\033[1;33mCongraduations! \033[0;35m100 \033[1;35mScore Points Gained!\n\n");
				player.score += 100;
			}
			player.turns_left -= 5;
			update_player();
			Pause3();
			PrintSlow("\n\033[0;41m  \033[0;1;37;44m Beam Me Up, Scotty! \033[0;41m  \033[0;40m\n\n");
			sleep(1);
			printf("\033[1;31m \"\033[33mCaptain On Deck!\033[31m\"\n");
			fflush(stdout); sleep(1);
			printf("\033[1;31m \"\033[33mReady for orders, sir!\033[31m\"\n");
			printf("\n");
			fflush(stdout); sleep(1);
			break;
		case 'A':
		case 'a': 
			printf("Attack\n");
			AttackPlayer();
			menu=FALSE;
			done=FALSE;
			pmt=TRUE;
			break;
		case 'S':
		case 's':
			printf("Scan\n\n \"Captain, the scanner is not operational.\"\n\n");
			menu=FALSE;
			done=FALSE;
			pmt=TRUE;
			break;
		case 'F':
		case 'f':
		case 'M':
		case 'm':
			printf("Map\n\n \"Captain, the scanner is broken.\"\n\n");
			menu=FALSE;
			done=FALSE;
			pmt=TRUE;
			break;
		case 'W':
		case 'w':
			printf("Wrap!\n\n \"Captain, wrap drives are down.\"\n\n");
			menu=FALSE;
			done=FALSE;
			pmt=TRUE;
			break;
		case 'U':
		case 'u':
			printf("Uplink\n\n \"Captain, uplink computer is offline.\"\n\n");
			menu=FALSE;
			done=FALSE;
			pmt=TRUE;
			break;
		case 'E':
		case 'e':
			printf("eLogs\n\n \"Captain, log harddrive has bad sectors.\"\n\n");
			menu=FALSE;
			done=FALSE;
			pmt=TRUE;
			break;
		case 'V':
		case 'v':
			printf("View\n");
			view_player_stat();
			printf("\n");
			menu=TRUE;
			done=FALSE;
			pmt=TRUE;
			break;
		case '\n':
			printf("\n\n\033[0;1;35m");
			fflush(stdout);
			system("fortune");
			printf("\n");
			menu=FALSE;
			done=FALSE;
			pmt=TRUE;
			break;
		case 'H':
		case 'h':
		case '?':
			printf("Menu\n\n");
			menu=TRUE;
			done=FALSE;
			pmt=TRUE;
			break;
		case 'Q':
		case 'q':
			printf("Quit\n");
			done=TRUE;
			break;
		default:
			menu=FALSE;
			done=FALSE;
			pmt=FALSE;
			continue;
		}
	}
	return;
}

