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

/************************/
/* Last Callers Listing */
/************************/
#include "shack.h"
#include "user.h"

char actions[1024];



void logact(char act) {
	char buf[2];
	buf[0] = act;
	buf[1] = '\0';
	if(strchr(actions,act) == NULL && 
	   (strlen(actions) < 1023) ) strcat(actions,buf);
	return;	
}

void listact(int drop) {
	FILE *fp,*tmpfp;
	char buf[256],nbuf[256],abuf[128],sbuf[128];
	time_t t;
	int i;

	buf[0] = '\0';
	nbuf[0] = '\0';
	abuf[0] = '\0';
	sbuf[0] = '\0';

	/****************************************/
	/* if in LastListHidden, don't log last */
	/****************************************/
	if(is_lastlisthidden(get_user_name())) return;				
	

	/******************************************/
	/* copy the last 4 enteries into tmp file */
	/******************************************/
	if((tmpfp = tmpfile()) == NULL) return;
	sprintf(nbuf,"%s/data/LastCallers",BBSDIR);
	if((fp = fopen(nbuf,"r")) != NULL) {
		for(i=0;i<5;i++) {
			if(fgets(buf,128,fp) == NULL) break;
			if(i==0) continue;
			/*****************************************/
			/* the line above is a problem:          */
			/* if you delete the file and start over */
			/* it will not be able to go past one.   */
			/*   -- to tired to fix it now...        */
			/*****************************************/
			fputs(buf,tmpfp);
		}
		fclose(fp);
	}
	/**********************************/
	/* Check for what they did online */
	/* first one checks for read      */
	/************************/
	/* Caller read messages */
	/************************/

	if(strchr(actions,'r') != NULL) {
		sbuf[0] = 'R';
		sbuf[1] = '\0';
		strcat(abuf,sbuf);
	} else {
		sbuf[0] = '-';
		sbuf[1] = '\0';
		strcat(abuf,sbuf);
	}
	
	/******************/
	/* check for post */
	/******************/	
	if(strchr(actions,'p') != NULL) {
		sbuf[0] = 'P';
		sbuf[1] = '\0';
		strcat(abuf,sbuf);
	} else {
		sbuf[0] = '-';
		sbuf[1] = '\0';
		strcat(abuf,sbuf);
	}

	/******************/
	/* check for yell */
	/******************/	
	if(strchr(actions,'y') != NULL) {
		sbuf[0] = 'Y';
		sbuf[1] = '\0';
		strcat(abuf,sbuf);
	} else {
		sbuf[0] = '-';
		sbuf[1] = '\0';
		strcat(abuf,sbuf);
	}

	/************************/
	/* check for dictionary */
	/************************/	
	if(strchr(actions,'d') != NULL) {
		sbuf[0] = 'D';
		sbuf[1] = '\0';
		strcat(abuf,sbuf);
	} else {
		sbuf[0] = '-';
		sbuf[1] = '\0';
		strcat(abuf,sbuf);
	}

	/**********************/
	/* check for userlist */
	/**********************/	
	if(strchr(actions,'u') != NULL) {
		sbuf[0] = 'U';
		sbuf[1] = '\0';
		strcat(abuf,sbuf);
	} else {
		sbuf[0] = '-';
		sbuf[1] = '\0';
		strcat(abuf,sbuf);
	}

	/********************/
	/* check for Wumpus */
	/********************/	
	if(strchr(actions,'x') != NULL) {
		sbuf[0] = 'X';
		sbuf[1] = '\0';
		strcat(abuf,sbuf);
	} else {
		sbuf[0] = '-';
		sbuf[1] = '\0';
		strcat(abuf,sbuf);
	}

	sbuf[0] = '-';
	sbuf[1] = '\0';
	strcat(abuf,sbuf);
	
	/**************************/
	/* check for drop carrier */
	/**************************/	
	if(drop == 0) {
		sbuf[0] = 'N';
		sbuf[1] = '\0';
		strcat(abuf,sbuf);
	} else {
		sbuf[0] = 'C';
		sbuf[1] = '\0';
		sbuf[2] = '\0';
		strcat(abuf,sbuf);
	}

	/**********************************/
	/* or skip check if user is new   */
	/**********************************/
	if(strchr(actions,'n') != NULL) {
		abuf[0] = '\0';
		strcpy(abuf, "new");
	}

	/**************************/
	/* Write to the data file */
	/**************************/
	time(&t);
	fprintf(tmpfp,"%-14.14s~%ld~%ld~%s~\n",username,t-userstate.time_laston,userstate.time_laston,abuf);
	if((fp = fopen(nbuf,"w")) == NULL) return;
	rewind(tmpfp);
	while(fgets(buf,128,tmpfp) != NULL) fputs(buf,fp);
	fclose(fp);
	fclose(tmpfp);
	return;
} 


void lastlist(void) {
	FILE *fp;
	char buf[128],nbuf[128],abuf[16],agobuf[100],minbuf[100];
	long ago,min;
     
	// sdelic: this screen clear doesn't work on alot of the
	//  terminals... it can make the screen look screwie,
	//  I decided not to do any screen clearing because there
	//  isn't any nice way to do it, like a scrolling printer
	//  ...   by the way, \033[H\033[2J would work better, if
	//       you think its really necessary to clear the screen.
	//    .. so I'll put it back with that and see -if there is
	//     any feedback yadda yadda, try afew different terminals..        

	//  by the way, trying to keep things (screens) at 'less than
	//  23 characters... 

        printf("\033[H\033[2J");
        printf("\x1b[0;1;34m");
	printf("\nL A S T   C A L L E R S   L I S T\n");
	printf("\033[0;34m~ ~ ~ ~   ~ ~ ~ ~ ~ ~ ~   ~ ~ ~ ~\033[0m\n");
	sprintf(buf,"%s/data/LastCallers",BBSDIR);
	if((fp = fopen(buf,"r")) != NULL) {
		while(!feof(fp)) {
			fscanf(fp,"%[^~]~%[^~]~%[^~]~%[^~]~\n",nbuf,minbuf,agobuf,abuf);
			min = atoi(minbuf);
			ago = atoi(agobuf);
			printf("\033[0;32m%-14s ",nbuf);
			if(min >= 60) {
				sprintf(buf,"%3dm",(min/60));
				printf("\033[1;32m%-3s ",buf);
			} else {
				sprintf(buf,"%3ds",min);
				printf("\033[1;32m%-3s ",buf);
			}
			if(!strcmp(abuf,"new")) 
				strcpy(abuf,"\033[0;32mN\033[1;32mew\033[0;32mU\033[1;32mser!");
			printf("\033[0;31m%1.1f  \033[1;31m%s\n",(time(NULL)-ago)/86400.0,abuf);
		}
	} else return;
	fclose(fp);
	return;
}

