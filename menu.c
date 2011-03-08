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
#include "rooms.h"
#include <netdb.h>
#include <sys/socket.h>


void MainMenu(const char *ip) {
	int done=FALSE;
	int menu=FALSE;
	char ch, obuf[256];
	struct tm *tp;
	time_t t;
	
	while(!done) {
		if(menu) CatFileNoPause("MAINMENU");
		//sprintf(obuf,"\x1b[1;34m[ \x1b[0;31m!%s\x1b[0;31m! [?]=:>\x1b[0;33m ",GetRoomName());
		sprintf(obuf,"\x1b[0;31m!%s\x1b[0;31m! \033[1;31m[\033[1;33m?\033[1;31m]\033[1;33m=:>\x1b[0;33m ",GetRoomName());
		fflush(stdout);
		ch = GetKeyPrompt(obuf);
		switch(ch) {
		case 'J':
		case 'j':
			printf("Join");
			fflush(stdout);
			JoinRoom();
			printf("\n");
			fflush(stdout);
			menu=FALSE;
			break;
		case 'E':
		case 'e':
			syslog(LOG_INFO,"%s %s",
				get_user_name(),
				"hit E instead of P for post");
			printf("\n\nHit `p` to post a message.\n\n");
			fflush(stdout);
			menu=FALSE;
			break;
		case 'K':
		case 'k':
			syslog(LOG_INFO,"%s %s",
				get_user_name(),
				"jit K instead of J");
			printf("\n\nHit `j` to see room list.\n\n");
			fflush(stdout);
			menu=FALSE;
			break;
		case ' ':
		case 'r':
			logact('r');
			whois_on(1);
			printf("\x1b[0;35mR\x1b[31me\x1b[32mA\x1b[36md\x1b[1;34mi\x1b[31mn\x1b[0;32mg\x1b[35m.\x1b[34m. ");
			syslog(LOG_INFO,"%s %s",
				get_user_name(),
				"is reading messages");
			fflush(stdout);
			ReadMsgs(0);
			whois_on(0);
			syslog(LOG_INFO,"%s %s",
				get_user_name(),
				"is done reading messages");
			menu=FALSE;
			done=FALSE;
			break;
		case 'B':
		case 'b':
		case '-':
			logact('r');
			whois_on(1);
			syslog(LOG_INFO,"%s %s",
				get_user_name(),
				"is reading backwards");
			printf("\x1b[1;31mbackwards read: ");
			fflush(stdout);
			ReadMsgs(1);
			whois_on(0);
			syslog(LOG_INFO,"%s %s",
				get_user_name(),
				"is done reading backwards");
			menu=FALSE;
			break;
		case 'P':
			logact('p');
			if(!is_sysop(get_user_name())) {
				printf("\n\n  Type '?' for help.\n");
				printf("  Turn off CAPS, hit 'p' to post.\n\n");
				done=FALSE;
				menu=FALSE;
				break;
			} else {
				printf("\033[1;35mSysOp Post!!\n");
				whois_on(5);
				editor2("Everybody",NULL,0);
				printf("\n");
				whois_on(0);
				done=FALSE;
				menu=FALSE;
				break;
			} 		
		case 'p': 
			logact('p');
			whois_on(5);
			syslog(LOG_INFO,"%s %s",
				get_user_name(),
				"is trying to post");
			printf("\033[1;31mpost \033[0;31m(\033[1;33mYay!\033[0;31m)\n");
			editor2(NULL,NULL,0);
			whois_on(0);
			printf("\n");
			syslog(LOG_INFO,"%s %s",
				get_user_name(),
				"finished posting");	
			menu=FALSE;
			break;
		case 'G':
		case 'g':
			printf("\033[1;34mG\033[0;34m.\033[1;34mfi\033[0;34mles!\033[0m\n");
			if(is_twit(get_user_name())) {
				printf("\n");
				menu=FALSE;
				break;
			}
			logact('t');
			syslog(LOG_INFO,"%s %s",
				get_user_name(),
				"is reading gfiles");
			whois_on(2);
			fflush(stdout);
			GFilesMenu();
			fflush(stdout);
			whois_on(0);
			syslog(LOG_INFO,"%s %s",
				get_user_name(),
				"is done with text files");
			menu=TRUE;
			break;
		case 'Y':
		case '.':
		case 'y':
			if(is_twit(get_user_name())) {
				printf("\n");
				menu=FALSE;
				break;
			}
			logact('y');
                        syslog(LOG_INFO, "%s %s",
                                get_user_name(),
                                "is entering chat");
			whois_on(6);
			ChatRoomPrompt();
			whois_on(0);
			menu=TRUE;
			break;
		case 'L':
		case 'l':
			printf("\x1b[0mlogout");
			menu = FALSE;
			done = TRUE;   /* done goto end */
			break;
		case 'X':
		case 'x':
			/* Note:
			 * check to see if there is a blank
			 * space after quit of games menu
			 */
			if(is_twit(get_user_name())) {
				menu=FALSE;
				break;
			}
			logact('x');
			syslog(LOG_INFO,"%s %s",
				get_user_name(),
				"enters the door game");
			whois_on(3);
			printf("bbs game!\n");
			StartGame();
			printf("\n");
			fflush(stdout);
			whois_on(0);
			syslog(LOG_INFO,"%s %s",
				get_user_name(),
				"leaves the door game");
			menu=TRUE;
			done=FALSE;
			break;
		case '?':
			printf("\x1b[1;34mMenu\n");
			syslog(LOG_INFO,"%s %s",
				get_user_name(),
				"displays the menu");
			menu=TRUE;
			break;
		case 't':
			t = time(NULL);
			tp = localtime(&t);
			printf("%s\n", asctime(tp));
			printf("Time Connected: %ld Seconds\n\n",TotalConnTime());
			syslog(LOG_INFO,"%s %s",
				get_user_name(),
				"checks the time");
			menu=FALSE;
			break;
		case 'U':
		case 'u':
			logact('u');
			syslog(LOG_INFO,"%s %s",
				get_user_name(),
				"in user profiles");
			printf("UserProfiles\n");
			printf("\n\033[1;35mType '\033[1;31mList\033[1;35m' to list all.\n\n");
			UserToProfile();
			syslog(LOG_INFO,"%s %s",
				get_user_name(),
				"is done reading the user list");
			menu=FALSE;
			break;
		case 'C':
		case 'c':
			printf("\033[0;36mCo\033[0;35mnfig");
			syslog(LOG_INFO,"%s %s",
				get_user_name(),
				"enters the configurator");
			whois_on(4);
			ConfigureMenu(ip);
			whois_on(0);
			syslog(LOG_INFO,"%s %s",
				get_user_name(),
				"leaves the configurator");
			menu=TRUE;
			break;
		case '/':
			printf("Who's Online\n\n");
			syslog(LOG_INFO,"%s %s",
				get_user_name(),
				"views the who's online list");
			whois_on_list();
			printf("\n");
			menu=FALSE;
			break;	
		case '!':
			if(!is_sysop(get_user_name())) {
				printf("\n\n  Type '?' for help.\n\n");
				done=FALSE;
				menu=FALSE;
				break;
			} else {
				printf("!\n");
				SysopMenu();
				printf("\n\n");
				done=FALSE;
				menu=FALSE;
				break;
			} 		
		default:
			printf("\n\n  Type '?' for help.\n\n");
			done=FALSE;
			menu=FALSE;
			break;
		} //end of switch
	} //end of while
	return;
}

void NewUserResetFiveMsgs(void) {

	char obuf[256];

	sprintf(obuf,"%s/users/%s/keycode",BBSDIR,get_user_name());
	unlink(obuf);
	sleep(1);
	printf("\n");
	printf("\033[1;32mWelcome to the System, %s.\n",get_user_name());
	PrintSlow("\033[1;31mKeycode PASSED!\n");
	printf("\033[0;31m=-=-=-=-=-=-=-=-=-=\033[1;32m\n");
	printf("To save yourself some reading, you can\n");
	printf("Set to read just the last five messages\n");
	//printf("\n");

	//printf("\033[1;32mDo this? \033[1;33m(\033[1;31mRecommended\033[1;33m) [\033[0;31mYes\033[1;33m=:> ");

	ResetFiveMsgs();
	printf("\n\033[1;35mPointers reset..\n\n");
}

int ValidateMenu(int i) {
	
	char ibuf[5],obuf[256];
	char ch,done=FALSE,menu=TRUE,val=FALSE;
	rNum = 1;	

	//syslog(LOG_INFO,"%s %s",
	//	get_user_name(),
	//	"is at new user menu");
	logact('n');
	while(!done) {
		if(menu) CatFileNoPause("NewUserMenu");
		ch = GetKeyPrompt("\x1b[1;34m[ \x1b[0;31m!N\x1b[1;31mewbies\x1b[0;31m! \x1b[1;34m]=:>\x1b[0;33m ");
		switch(ch) {
		case 'e':
		case 'E':
			syslog(LOG_INFO,"%s %s",
				get_user_name(),
				"attempt to enter a key code");
			printf("\n\n\033[1;32mPlease Enter Email Keycode\033[1;34m: [    ]\b\b\b\b\b\033[0;33m");
			GetStr(ibuf,4,NO_SPACE|NO_ATSIGN|NO_FORSLASH|NO_BAKSLASH|NO_DASH);
			i = CheckKeyCode();
			if(i != atoi(ibuf)) {
				printf("\n\033[1;33mKeycode not valid!\n");
				printf("\n'?' for Help.\n\n");
				syslog(LOG_INFO,"%s%s",
					get_user_name(),
					"'s key code is not valid");
				done = FALSE;
				val = FALSE;
			} else {
				char cha;
				sprintf(obuf,"%s/users/%s/keycode",BBSDIR,get_user_name());
				unlink(obuf);
				sleep(1);
				printf("\n");
				printf("\033[1;32mWelcome to the System, %s.\n",get_user_name());
				PrintSlow("\033[1;31mKeycode PASSED!\n");
				printf("\033[0;31m=-=-=-=-=-=-=-=-=-=\033[1;32m\n");
				printf("To save yourself some reading, you can\n");
				printf("Set to read just the last five messages\n");
				printf("\n");
				printf("\033[1;32mDo this? \033[1;33m(\033[1;31mRecommended\033[1;33m) [\033[0;31mYes\033[1;33m=:> ");
				for(;;) {
					cha = tolower(GetKey(0));
					if(cha=='y'||cha=='j'||cha=='n') break;
				}
				if(cha=='y'||cha=='j') {
					printf("Yes");
					ResetFiveMsgs();
					printf("\n\n\033[1;35mPointers reset..");
				} else printf("No");
				printf("\n\n");
				syslog(LOG_INFO,"%s%s",
					get_user_name(),
					"'s key code PASSED!");
				val = TRUE;
				done = TRUE;
			}
			menu = FALSE;
			break;
		case 'c':
		case 'C':
			printf("Change Email Address");
			syslog(LOG_INFO,"%s %s",
				get_user_name(),
				"attemping to change their email address");
			GetEmailVerify();
			menu = FALSE;
			break;
		case 'n':
		case 'N':
			printf("Note To: SysOp!\n");
			syslog(LOG_INFO,"%s %s",
				get_user_name(),
				"tries writting a note to Sysop");
			editor2(NULL,NULL,0);
			printf("\n");
			menu = FALSE;
			break;
		case 'l':
		case 'L':
			printf("logout");
			menu = FALSE;
			val = FALSE;
			done = TRUE;
			break;
		case '?':
			printf("menu\n\n");
			syslog(LOG_INFO,"%s %s",
				get_user_name(),
				"displays the newbie menu");
			menu = TRUE;
			done = FALSE;
			break;
		default:
			printf("\n\nType '?' for menu.\n\n");
			menu = FALSE;
			done = FALSE;
			break;
		}
	}
	return val;		
}


void ConfigureMenu(const char *ip) {
	int done=FALSE,ch;
	char remotehost[2048];
	
	while(!done) {
		printf("\x1b[0m\n\n");
		if(is_sysop(get_user_name())) {
			printf("  \033[1;34m1. \033[0;35mS\033[36mh\033[0mell \033[36mt\033[0mo \033[35mB\033[36ma\033[0msh \033[35mP\033[36mr\033[0mompt.\n");
			printf("  \033[1;34m2. \033[0;35mV\033[36me\033[0miw \033[36mo\033[0mr \033[35mE\033[36md\033[0mit a \033[35mU\033[36ms\033[0mer.\n");
			printf("  \033[1;34m3. \033[0;35mS\033[36my\033[0mstem \033[0;35mW\033[36mi\033[0mde \033[35mB\033[36mr\033[0moadcast\n\n");
		}
		printf("  \x1b[1;34mx. \x1b[0;35mC\x1b[0;36mh\x1b[0mange \x1b[0;35my\x1b[0;36mo\x1b[0mur \x1b[0;35mP\x1b[0;36ma\x1b[0mssword\n");
		printf("  \033[1;34ma. \033[0;35mC\033[0;36mh\033[0mange \033[0;35my\033[0;36mo\033[0mur \033[0;35mA\033[0;36mu\033[0mtoSignature\n");
		printf("  \033[1;34mi. \033[0;35mC\033[0;36mh\033[0mange \033[0;35my\033[0;36mo\033[0mur \033[0;35mU\033[0;36ms\033[0mser \033[0;35mP\033[0;36mo\033[0mrfile\n");
		printf("\n");
		printf("  \x1b[1;34mv. \x1b[0;35mV\x1b[0;36mi\x1b[0mew \x1b[0;35mL\x1b[0;36ma\x1b[0mst \x1b[0;35mC\x1b[0;36ma\x1b[0mllers\n");
		printf("  \033[1;34ms. \033[0;35mS\033[0;36mh\033[0mow \033[0;35mC\033[0;36mo\033[0mnnection \033[0;35mS\033[0;36mp\033[0meed\n");
		printf("\n");
		printf("  \033[1;34ml. \033[0;35mL\033[36mi\033[0mst \033[35mR\033[36mo\033[0mom\033[35mM\033[36ma\033[0mkers\n");
		printf("  \033[1;34mc. \033[0;35mC\033[36mr\033[0meate \033[35ma \033[35mR\033[36mo\033[0mom\n");
		printf("  \033[1;34md. \033[0;35mD\033[36me\033[0mlete \033[35my\033[36mo\033[0mur \033[35mR\033[36mo\033[0mom\n\n");		
		printf("\x1b[0;35mCo\x1b[0;36mnfi\x1b[0mgure \x1b[0;35mM\x1b[0;36me\x1b[0mnu, \x1b[1;30m[\x1b[34mSPACE\x1b[1;30m] \x1b[0mto quit: [ ]\b\b"); 
		for(;;) {
			ch = tolower(GetKey(0));
			if(ch=='x'||ch=='v'||ch=='l'||ch=='c'||ch=='d'||
			   ch==' '||ch=='a'||ch=='q'||ch=='i'||
			   ch=='\n') break;
			if(is_sysop(get_user_name())) {
				if(ch=='1'||ch=='2'||ch=='3'||ch=='4') break;
			}
		} 
		switch(ch) {
		case '4':
			printf("4\n");
			gethostbyaddr(remotehost,2047,AF_INET);
			printf("\n\nRemoteHost = %s\n\n",remotehost);			
			done = TRUE;
			break;
		case '1':
			printf("1\n");
			printf("\n\033[1;32mShell is Inactive\n");
			//system("/usr/local/bin/tcsh");
			done = TRUE;
			break;
		case '2':
			printf("2\n");
			SysopMenu();
			printf("\n");
			done = TRUE;
			break;
		case '3':
			printf("3\n");
			Yell();
			done = TRUE;
			break;
		case 'x':
			printf("x\n");
			ChangePassword();
			printf("\n\x1b[0mPassword Changed Successfully.\n");
			syslog(LOG_INFO,"%s %s",
				get_user_name(),
				"changed their password");
			done = TRUE;
			break;
		case 'v':
			printf("v\n\n");
			lastlist();
			syslog(LOG_INFO,"%s %s",
				get_user_name(),
				"viewed the last callers list");
			printf("\n");
			system("uptime");
			printf("\n");
			Pause2();
			done = TRUE;
			break;
		case 'l':
			{
			char tbuf[200];
			printf("l\n\n");
			sprintf(tbuf,"%s/data/RoomMakers",BBSDIR);
			CatFileDirect(tbuf);
			printf("\n");
			syslog(LOG_INFO,"%s %s",
				get_user_name(),
				"viewed list of room makers");
			Pause2();
			done = TRUE;
			break;
			}
		case 'c':
			printf("c\n");
			if(MakeNewRoom() == 0) {
				printf("Room Saved Succesfully...\n");
				printf("Check the Join List..\n\n");
				Pause2();
				printf("\n");
				printf("\033[0;36mNote\033[1;37m1: \033[1;35mThe first message (msg# Zero) will not\n");
				printf("       be viewable by any, you must post it\n");
				printf("       to start the forum.\n");
				printf("\n");
				printf("\033[0;36mNote\033[1;37m2: \033[1;35mIt is recommended that you post an\n");
				printf("       introductionary message, but remember\n");
				printf("       no one will see the \033[1;31mvery first\033[1;35m message!\n");
				printf("\n");
				printf("\033[1;33mGoodluck!!\n\n");
				Pause2();
			} else {
				printf("Room NOT Saved for some reason.\n\n");
				Pause2();
			}
			done = TRUE;
			break;
		case 'd':
			printf("d\n\n");
			printf("Option not available yet.\n\n");
			Pause2();
			done = TRUE;
			break;
		case 's':
			printf("s\n\n");
			{
				char tbuf[500];
				sprintf(tbuf,"ping -c 5 %s",ip);
				system(tbuf);
			}
			printf("\n");
			Pause2();
			done = TRUE;
			break;
		case 'a':
			printf("a\n\n");
			syslog(LOG_INFO,"%s %s",
				get_user_name(),
				"viewing their signature");
			printf("\033[1;31mYour Currnet Signature\n");
			printf("\033[0;31m~~~~ ~~~~~~~ ~~~~~~~~~\033[0;33m\n");
			{
				char tbuf[500],ch2;
				sprintf(tbuf,"%s/users/%s/signature",BBSDIR,get_user_name());
				CatFileDirect(tbuf);
				printf("\033[0mC\033[36mh\033[35mange \033[0mA\033[36mu\033[35mtoSig \033[0;35m[\033[31mNo\033[35m]? ");
				for(;;) {
					ch2 = tolower(GetKey(0));
					if(ch2=='y'||ch2=='\n'||ch2=='n') break;
				}
				switch(ch2) {
				case 'y':
					printf("\033[0;31mYes!\n\n");
					printf("\033[1;31mType your signature here (\033[0;31m5 lines\033[1;31m).\n");
					printf("\033[1;33mColor codes are ok!\n");
					syslog(LOG_INFO,"%s %s",
						get_user_name(),
						"is changing their signature");
					file_editor(tbuf,6);
					break;			
				case 'n':
				case '\n':
				default:
					syslog(LOG_INFO,"%s %s",
						get_user_name(),
						"didn't change their signature");
					printf("\033[0;31mNo!\n\n");
					break; 
				} 

			}
			Pause2();
			done = TRUE;
			break;

		case 'i':
			printf("i\n\n");
			syslog(LOG_INFO,"%s %s",
				get_user_name(),
				"might change their profile.");
			printf("\033[1;31mYour Currnet User Profile\n");
			printf("\033[0;31m~~~~ ~~~~~~~ ~~~~ ~~~~~~~\033[0;33m\n");
			{
				char tbuf[500],ch2;
				sprintf(tbuf,"%s/users/%s/user_profile",BBSDIR,get_user_name());
				CatFileDirect(tbuf);
				printf("\033[1;32mDo you want to change this \033[35m[\033[31mNo\033[35m]\033[31m? ");
				for(;;) {
					ch2 = tolower(GetKey(0));
					if(ch2=='y'||ch2=='\n'||ch2=='n') break;
				}
				switch(ch2) {
				case 'y':
					printf("\033[0;31mYes!\n\n");
					printf("\033[1;31mEnter new profile here (\033[0;31m7 lines max\033[1;31m).\n");
					printf("\033[1;33mColor codes are ok!\n");
					syslog(LOG_INFO,"%s %s",
						get_user_name(),
						"changing profile");
					file_editor(tbuf,8);
					break;			
				case 'n':
				case '\n':
				default:
					syslog(LOG_INFO,"%s %s",
						get_user_name(),
						"did not change profile");
					printf("\033[0;31mNo!\n\n");
					break; 
				} 

			}
			Pause2();
			done = TRUE;
			break;
	


		case '\n':
		case 'q':
		case ' ':
			printf("q\n");
			done = TRUE;
			break;
		default:
			break;
		}
	}
	return;
}


