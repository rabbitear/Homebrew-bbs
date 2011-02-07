#include "shack.h"
#include "user.h"
#include "game.h"
#include <dirent.h>
#include <ctype.h>

#define PLAYERFILE "player_state"
#define PITEMFILE "player_items"

int AttackPlayer(void);

/**********************************/
/* view_player_stat()             */
/* printout the player statistics */
/**********************************/
void view_player_stat() {
	int i;
	
	printf("\n\033[1;34m");
	for(i=0;i< (16+strlen(username) );i++) printf("=");
	printf("\n");
	printf("\033[1;32;44mSTATUS: \033[1;33mCaptain %s\033[0;40m",username);
	printf("\n\033[1;34m");
	for(i=0;i<(16+strlen(username));i++) printf("=");
	printf("\033[0m\n");

	printf("\033[35m  Turns Left\033[32m: \033[36m%d\n",player.turns_left);
	printf("\033[35m  Hit Points\033[32m: \033[36m%d \033[35mof \033[36m%d\n",player.phits,player.max_phits);
	printf("\033[35m       Charm\033[32m: \033[36m%d\n",player.chr);
	printf("\033[35m    Strength\033[32m: \033[36m%d\n",player.str);
	printf("\033[35m   Dexterity\033[32m: \033[36m%d\n",player.dex);
	printf("\033[35mCredit Chips\033[32m: \033[36m%ld \033[32m(\033[36m%ld \033[35mloan\033[32m)\n",player.credit_chips,player.loan);
	printf("\033[35m Total Score\033[32m: \033[36m%ld \033[32m(\033[35mAlingment=\033[36m%d\033[32m)\n",player.score,player.alingment);
	
	printf("\n\033[1;34m");
	for(i=0;i<(15+strlen(player.shipname));i++) printf("=");
	printf("\n");
	printf("\033[1;32;44mSTATUS: \033[1;33m%s Vessel\033[0;40m",player.shipname);
	printf("\n\033[1;34m");
	for(i=0;i<(15+strlen(player.shipname));i++) printf("=");
	printf("\033[0m\n");
	
	printf("\033[35m Ship Class\033[32m: %s\n",player.shipbrand);
	printf("\033[35m Warp Jumps\033[32m: \033[36m%d \033[35mof \033[36m%d (%d turns)\n",player.warps_left,player.max_warps,player.turns_per_warp);
	printf("\033[35mHull Points\033[32m: \033[36m%d \033[35mof \033[36m%d\n",player.ship_hits,player.max_ship_hits);
	printf("\033[35m Cargo Bays\033[32m: \033[36m%d\033[35m/\033[36m%d\n",player.cargo_bays,player.max_cargo_bays);
	printf("\033[35m   Fighters\033[32m: \033[36m%d\033[35m/\033[36m%d\n",player.fighters,player.max_fighters);
	printf("\033[35m    Shields\033[32m: \033[36m%d\033[35m/\033[36m%d\n",player.shields,player.max_shields);
	printf("\n");
	Pause3();
	return;
}


/********************************/
/* get_player()                 */
/* Loads the player information */
/* from the player status file. */
/********************************/

int get_player() {
	FILE *fp;
	char obuf[256];
	
	sprintf(obuf,"%s/users/%s/%s",BBSDIR,username,PLAYERFILE);
	if((fp = fopen(obuf,"rb")) == NULL) return -1;
	if(fread((char *)&player,sizeof(PLAYER),1,fp) != 1) return -2;
	fclose(fp);
	return 0;
}


/*********************************/
/* update_player()               */
/* updates the player state file */
/*********************************/

int update_player() {
	FILE *fp;
	char obuf[256];
	
	sprintf(obuf,"%s/users/%s/%s",BBSDIR,username,PLAYERFILE);
	if((fp = fopen(obuf,"wb")) == NULL) return -1;
	if(fwrite((char *)&player,sizeof(PLAYER),1,fp) != 1) return -2;
	fclose(fp);
	return 0;	
}

/*********************************/
/* mkplayer()                    */
/* Initializes a player's record */
/* Adding a new player           */
/*********************************/

int mkplayer() {
	char obuf[256];
	FILE *fp;
	struct stat st;

	PrintSlow("  \033[0;1;35;44m INCOMMING TRANSMISSION \033[0;1;33;40m\n");
	sleep(3);
	PrintSlow("\n");
	PrintSlow("New Mission Orders\n");
	sleep(1);
	PrintSlow("By Star Fleet Command\n\n");
	sleep(1); PrintSlow("The stardock 5 reporting alien ship of\n");
	sleep(1); PrintSlow("unknown origin found near the Dagaba\n");
	sleep(1); PrintSlow("System. Investigate this anomoly! Warp\n");
	sleep(1); PrintSlow("path being transfered to your ships\n");
	sleep(1); PrintSlow("computer. Proceed with coution.\n\n");
	Pause3();

	printf("\n");	
	printf("\033[1;35mA\033[0mhh, a new captain!\n");
	printf("\033[1;35mWel\033[0mcome..");
	printf("\033[1;35mCreatin\033[0mg Ship Fi\033[1mles..\n");	
	
	player.shipname[0] = '\0';
	strcpy(player.shipbrand,"Vaccum Runner");
	
	player.turns_left = 100;
	player.max_warps = 8;
	player.warps_left = 8;
	player.turns_per_warp = 3;

	player.fighters = 30;
	player.shields = 5;
	player.ship_hits = 20;	
	player.cargo_bays = 8;
	
	player.max_fighters = 60;
	player.max_shields = 100;
	player.max_ship_hits = 20;
	player.max_cargo_bays = 16;
	
	player.phits = 50;
	player.max_phits = 50;
	player.chr = 3;
	player.str = 5;
	player.dex = 5;
	
	player.loan = 0;
	player.credit_chips = 20000;
	player.score = 0;
	player.time_lastin = time(NULL);

	/** below writes the player data **/	
	sprintf(obuf,"%s/users/%s/%s",BBSDIR,username,PLAYERFILE);
	if((fp = fopen(obuf,"wb")) == NULL) return -1;
	if(fwrite((char *)&player,sizeof(PLAYER),1,fp) != 1) return -2;
	fclose(fp);	

	return 0;
}

/**********************************/
/* exists_player_state()          */
/* check to see if the playerfile */
/* exists and is valid size.      */
/**********************************/

int exists_player_state() {
	char obuf[256];
	struct stat st;
	sprintf(obuf,"%s/users/%s/player_state",BBSDIR,get_user_name(),PLAYERFILE);
	stat(obuf,&st);
	if(st.st_size != sizeof(PLAYER)) return 0;   /* wrong size */
	return 1;                                    /* right size */
}

/******************************************/
/* AttackPlayer()                         */
/*  Attack another player - still need to */
/*  add a bunch of stuff/features to this */
/*  -this function is not at all done!!!  */
/******************************************/

int AttackPlayer(void) {
	char obuf[256],ibuf[256];
	int i,j,a,b,c;
	struct stat st;
	FILE *fp;
	PLAYER defender;
	srand(time(NULL));
	return 0;

	if(player.turns_left < 7) {
		printf("\n\033[1;31mYou don't have enough turns left.\n\n");
		return 0;
	}
	if(player.fighters < 1) {
		printf("\n\033[1;31mYou don't have any fighters left.\n\n");
		return 0;
	}
	printf("\n\033[1;32mPlayer to Attack \033[0;35m[\033[0;31mNone[0;35m]? ");  
	GetStrName(obuf,31);
	if(strlen(obuf) < 3) {
		printf("\n \033[1;31m\"\033[33mAborting Attack\033[31m.\"\n\n");
		return 0;
	}
	if(!strcmp(obuf,get_user_name())) {
		printf("\n \033[1;31m\"\033[33mCaptain, are you nutz\033[31m?\"\n\n");
		return 0;
	}
	sprintf(ibuf,"%s/users/%s/player_state",BBSDIR,obuf);
	stat(ibuf,&st);
	if(st.st_size != sizeof(PLAYER)) {
		printf("\n \033[31m\"\033[33mCaptain, that player does not exist\033[31m!\"\n\n");
		return 0;
	}
	if((fp = fopen(ibuf,"rb")) == NULL) {
		printf("\n \033[1m\"Computer Malfunction, Aborting!\"\n\n");
		return 0;
	}
	if(fread((char *)&defender,sizeof(PLAYER),1,fp) != 1) {
		printf("\n \033[1m\"Computer Read Malfunction, Aborting!\"\n\n");
		fclose(fp);
		return 0;
	}
	fclose(fp);
	/* Now we have the defender data */
	/* Need to add to this */
	ibuf[0] = '\0';
	printf("\n \033[0;1;31;40m\"\033[33mPreparing Attck on \033[35m%s's \033[31m[\033[35m%s\033[31m]!\"\n\n",obuf,defender.shipname);
	printf(" \033[1;31m\"\033[33mThey have \033[36m%d\033[33m fighters, we have \033[36m%d\033[31m.\"\n\n",defender.fighters,player.fighters);
	printf("\033[1;32mHow Many Fighters To Send \033[31m(\033[35m%d max)\033[31m) [\033[35m0\033[31m]? ",player.fighters);
	GetStr(ibuf,5,0);
	for(i=0;ibuf[i]!='\0';i++) {
		if(!isdigit(ibuf[i])) {
			printf("\n \033[31m\"\033[33mCaptain, computer can't process data\033[31m,\033[33m aborting attack\033[31m!\"\n\n");
			return 0;
		}
	}	
	if(ibuf[0] == '\0' || ibuf[0] == '0') {
		printf("\n \033[1;31m\"\033[33mZero Fighters Sent\033[31m,\033[33m Attack Aborted\033[31m!\"\n\n");
		return 0;
	}
	
	a = player.fighters;
	b = defender.fighters;
	c = 1;
	
	for(i=atoi(ibuf);i>0;i--) {
		j = (rand() % 99);
		if(j<33) if(defender.fighters>1) {defender.fighters--;c++;}
		if(j>=33 && j<=66) continue;
		if(j>66) {player.fighters--;i--;}
	}
	printf("\n");
	for(i=0;i<6;i++) PrintSlow("\033[0;41m \033[0;42m \033[0;44m ");
	printf("\033[0;40m\n\n");
	printf("\033[0;32;40mGain Score for Attacking \033[1;36m%d \033[0;32mPoint\033[31m(\033[32ms\033[31m)\n",c);  	
	player.score += c;
	printf("\033[0;35m  You lost \033[36m%d\033[35m fighters, \033[1;36m%d\033[35m remain.\n",a-player.fighters,player.fighters);
	printf("\033[0;35m  You destryed \033[36m%d\033[35m fighters, \033[1;36m%d\033[35m remain.\n",b-defender.fighters,defender.fighters);
	player.turns_left -= 7;
	update_player();

	sprintf(ibuf,"%s/users/%s/player_state",BBSDIR,obuf);
	if((fp = fopen(ibuf,"wb")) == NULL) {
		printf("\033[1;37m\n \"Captain, there has been an amonmoly,");
		printf(" a ripple in time some how, the enemy");
		printf(" ship has returned to this normal form");
		printf(" just before the battle.\"\n\n");
		return 0;
	}
	if(fwrite((char *)&defender,sizeof(PLAYER),1,fp) != 1) {
		printf("\033[1;37m\n \"Captain, there has been an amonmoly,");
		printf(" a ripple in time some how, the enemy");
		printf(" ship has returned to this normal form");
		printf(" just before the battle.\"\n\n");
		fclose(fp);
		return 0;
	}
	fclose(fp);
	printf("\n");
	return 0;
}
