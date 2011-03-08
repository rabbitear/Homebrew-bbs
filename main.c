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
#include "user.h"
#include <signal.h>
#include <sys/ioctl.h>

void handle_broken_telnet(int signal);
void chgwinsz();

int main(int argc, char **argv) {

	time_t t,tc,laston_t;
	unsigned long lohi[2];
	int i=0,x=0;
	char obuf[384];
	struct winsize win;
	struct stat st;

	if(argc>2) syslog(LOG_INFO,"Connect (%s)",argv[2]);
	rows = 23;
	cols = 80;
	CatFileNoPause("LOGIN");
	printf("\033[0m\033[1;30mtHe sHacK! HomeBrew BBS v.96\n\033[0m");
	x = login();
	if(x>=3) { /* user desided to leave */
		syslog(LOG_INFO,"User desided to leave with (Exit/Bye/Quit).");
		printf("\nlogout.\n\n"); 
		exit(0); 
	}
	last_user_buffer[0] = '\0';

	/*****************************/
	/* record users ip addresses */
	/*****************************/
	if(argc>2)
		if(!search_users_ip(argv[2]))
			record_users_ip(argv[2]);
				
	/*****************************/
	/* initialize the rooms list */
	/*****************************/
	
	MakeRooms();

	/******************************/
	/* if a new user login as new */
	/******************************/

	if(x == 1) {
		syslog(LOG_INFO,"Running New User SignUp.");
		new_user_signup(); 
		MakeRooms();
	}

	/*******************************/
	/* handle the CRTL-C and drops */
	/*******************************/	
	signal(SIGINT,SIG_IGN); //ignore ctrl-c signal
	signal(SIGPIPE, handle_broken_telnet);
	signal(SIGHUP, handle_broken_telnet);
	signal(SIGTERM, handle_broken_telnet);
	signal(SIGWINCH, chgwinsz);
		
	/**************************/
	/* update user time stats */
	/**************************/
	time(&t);
	laston_t = userstate.time_laston;
	userstate.time_laston = t;
	userstate.no_calls++;
	update_user_state();

	/***********************************/
	/* announce login, update chatfile */
	/***********************************/	
	whois_on(0);
	sprintf(obuf,"\x1b[0;35mloggin in \x1b[1;34m[\x1b[32m%s\x1b[34m]\n\x1b[0m", username);
	broadcast(obuf);
	resetchatfp();
	syslog(LOG_INFO,"%s logged on to the bbs.",username);	
	fflush(stdout);

	/**********************************/
	/* check if user is not validated */
	/**********************************/
	x = TRUE;

	i = CheckKeyCode();

	// ------------------
	//  We are taking out the Validation Menu and letting new users
	//  directly into the bbs.  This is because the smtp ports have
	//  been blocked by the ISP.  
	//  We still check for the keycode to figure out if this is the
	//  callers first time logging in, and run the ... 
	// ------------------
	//if(i != 0) x = ValidateMenu(i);

	if(i != 0) NewUserResetFiveMsgs();

	if(x == TRUE) {
		/********************************/
		/* print some stats to the user */
		/********************************/



        /* static login message added by sdelic */ 
		
		/* Ktr: sdelic, you can have a whole screen 
		   for MOTD, we need to add function less
	           style, but I know I haven't been doing my
		   part lately on the function part either
		   I'll write up some plans for the message
		   queue'ing tonight. */

		/* This is: if the file MOTD exsists
		   it gets printed if it doesn't,
		   skips the screen all together,
		   =ktr= */
		sprintf(obuf,"%s/text/MOTD",BBSDIR);
		if(!stat(obuf,&st)) {	
			CatFileNoPause("MOTD");
			Pause();
			CatFileNoPause("MOTD2");
			Pause();
		}	

		lastlist();
		
		fflush(stdout);
		//sleep(1);
        sprintf(obuf,"\n\033[0;32mGreetings \033[1;31m%s!!\033[0;32m\n",get_user_name());
		PrintSlow(obuf);
		fflush(stdout);
		sleep(1);
                printf("Your last call is \033[1;31m"); printago(laston_t);
		printf("\033[0;32m ago.\n");
		printf("Checking msgs");
		fflush(stdout);
		i = TotalNewMsgs();
		printf("\033[255D\033[K\033[0;32m");
		fflush(stdout);
		sprintf(obuf,"There are \033[1;31m%d new msgs \033[0;32mon the bbs.\n\n",i); 
		printf(obuf);
		if(ioctl(fileno(stdout),TIOCGWINSZ,&win) != -1) {
			rows = win.ws_row;
			cols = win.ws_col;
		} else {
			rows = 23;
			cols = 80;
		}
		if((rows > 300) || (rows < 6)) rows = 23;
		if(cols > 300) cols = 80;
		if(cols < 40) cols = 40;
		syslog(LOG_INFO,"%s scrn is %d x %d",username,win.ws_col,win.ws_row);
		syslog(LOG_INFO,"%s has (%d) unread Msgs.",username,i);
		CheckProfile();
		printf("\033[1;32m        ////\n");
		printf("\033[1;32m       (o o)     \033[1;31m[\033[1;32mtHe sHack bBS\033[1;31m]\n");
		printf("\033[1;32m    ooO (_) Ooo\n");
		printf("\033[0;31m_._._._._._._._._ ._._.\033[0;31m _._._._._\n");
		printf(" \033[0;31m\"\033[1;31mSit back\033[0;31m, \033[1;31mrelax\033[0;31m, \033[1;31mtake off your\n");
		printf(" shoes\033[0;31m, \033[1;31mand press dat space bar\033[0;31m\"\n\n");
		JoinStats();
		printf("\n");
		fflush(stdout);

		/*************************/
		/* run the bbs as normal */
		/*************************/
		syslog(LOG_INFO,"Starting the MainMenu for %s.",username);
		if(argc>2)
			MainMenu(argv[2]);
		else
			MainMenu(NULL);

	}
	logout();
	return 0;
}



void handle_broken_telnet(int signal) {

	char obuf[384];
	time_t t;
		
	time(&t);
	userstate.time_total += (t - userstate.time_laston);

	userstate.no_drops++;
	update_user_state();
	listact(1);
	whois_off();
		
	sprintf(obuf,"\x1b[0;34mlogging out \x1b[33m[\x1b[35m%s\x1b[33m] (\x1b[34m%lds\x1b[33m) [\x1b[36mbroken telnet\x1b[33m]\n\x1b[0m", username, t-userstate.time_laston);
	broadcast(obuf);
	syslog(LOG_INFO,"%s %s","-broken- telnet!:",username);
	sleep(2);
	exit(0);
}


void chgwinsz() {
	struct winsize win;
	
	(void) signal(SIGWINCH, SIG_IGN);
	if(ioctl(fileno(stdout), TIOCGWINSZ, &win) != -1) {
		if(win.ws_row != 0) rows = win.ws_row;
		if(win.ws_col != 0) cols = win.ws_col;
	}
	(void) signal(SIGWINCH, chgwinsz);
}

