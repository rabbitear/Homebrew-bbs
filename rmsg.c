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
#include "rooms.h"

int GetRoomFile(void);
void JoinStats(void);
void ResetFiveMsgs(void);
 
/* rrd = read reverse direction */
void ReadMsgs(int rrd) {
	long msgNo = 0, tmpMsgNo = 0;
	int done = FALSE;
	char ch, ibuf[100],obuf[1024];
	long lohi[2],newmsgs;
	
	
	if(rrd) {
		newmsgs = NewMsgs(1,lohi);	 
		if(newmsgs) {
			PrintSlow("\n\n\x1b[0mNo Old Messages!\n\n");
			return;
		} else done = FALSE;
	
	} else {
		/*************************************************/
		/* in this case, I check the current conference  */
		/* first, for reading forward new messages, then */
		/* check from 0 to MAX which is 2 for any new.   */
		/*************************************************/ 
		newmsgs = NewMsgs(0,lohi);
		if(!newmsgs) { 				// nothing in current confernece to read
			rNum = 0;
			while(rNum < maxrooms) {	// search other conferences
				newmsgs = NewMsgs(0,lohi);
				if(rNum == (maxrooms-1) && (!newmsgs)) {
					PrintSlow("\n\n\x1b[0mNo New Messages. (;\n\n");
					rNum=0;
					return;
				} else if((newmsgs) && (rNum < maxrooms) && (room[rNum].avail != TRUE)) {
					printf("\n\n");
					JoinStats();
					done = TRUE;					
					break;
				} else {
					done = TRUE;
					rNum++;
				}
			}
		} else done = FALSE;
	}

	if(done) { 
		printf("\033[1;32m\nHit the \033[1;34m[\033[0;35mSPACE\033[1;34m] \033[1;32mbar to read them..\n\n");
		return;
	} else printf("%s..", GetRoomName());
	if(!rrd) userstate.lastmsg[rNum]++;
	msgNo = userstate.lastmsg[rNum];	
	CatMsg(msgNo,lohi[1],rNum);

	/*********************************************/
	/* this is the start of the msg prompt loop. */
	/* To support multi-rooms we may have to re- */
	/* write the updating of message pointer     */
	/* along with adding a room pointer (update) */
	/*********************************************/
	while(!done) {
		if(rNum != 0)
			sprintf(obuf,"\x1b[1;31m(\x1b[33ma\x1b[31m)\x1b[0;35mgain\x1b[34m,\x1b[1;31m(\x1b[33mp\x1b[31m)\x1b[0;35most\x1b[34m,\x1b[1;31m(\x1b[33mq\x1b[31m)\x1b[0;35muit\x1b[34m,\x1b[1;31m[\x1b[1;33m%d \x1b[0;33mMore\x1b[1;31m]\x1b[1;31m[\033[1;33m?\033[1;31m]\033[0;35m=:>\x1b[0m ", lohi[1]-msgNo);
		else
			sprintf(obuf,"\033[1;31mA\033[1;34m>\033[1;32mgain\033[1;34m, \033[1;31mQ\033[1;34m>\033[1;32muit\033[1;34m, <\033[1;31mSPACE\033[1;34m>=\033[1;32mnext \033[0;34m[\033[1;31m%d \033[1;32mMore\033[0;34m]\033[1;34m?\033[0m ", lohi[1]-msgNo);
 
		ch = GetKeyPrompt(obuf);
		switch(tolower(ch)) {
			case 'a':
				printf("Again");
				CatMsg(msgNo, lohi[1],rNum);
				continue;
			case 'l':
				printf("Logout");
				logout();
			case 's':
				printf("Stop\n\n");
				done = TRUE;
				continue;
			case 'q':
				printf("Quit\n\n");
				done = TRUE;
				continue;
			case 'p':
				logact('p');
				printf("Post\n");
				whois_on(5);
				editor2(NULL,NULL,0);
				whois_on(1);
				printf("\n");
				continue;
			case 'r':
				logact('p');
				printf("\x1b[1;35mBug! \033[1;36m[\x1b[0;35m%s\x1b[1;36m]\n",(char *)GetMsgAuthor(msgNo,rNum));
				whois_on(5);
				editor2((char *)GetMsgAuthor(msgNo,rNum),
					(char *)GetMsgSubject(msgNo,rNum),msgNo);
				whois_on(1);
				printf("\n");
				continue;
			case 'd':
				if( (!strcmp((char *)get_user_name(),
					(char *)GetMsgAuthor(msgNo,rNum)) ) 
					|| (!strcmp((char *)get_user_name(),
					SYSOPNAME)) 
					|| (!strcmp((char *)get_user_name(), 
					room[rNum].owner)) ) {

					printf("Delete! Confirm [Shift-Y]? ");
					for(;;) {
						ch = GetKey(0);
						if(ch == 'Y') {
							DeleteMsg(msgNo,rNum);
							printf("\n\nMessage no. %ld is Removed.\n\n", msgNo);
							break;
						} else {
							printf("\n\nMessage Not Deleted.\n\n");
							break;
						}
					}
				} else {
					printf("\n\n\x1b[0;33mYou can't delete this message.\n\n");
				}
				continue;
			case 'm':
				if(is_sysop(get_user_name())) {
					printf("Move\n");
					MoveMsg(msgNo, lohi[1], rNum);
				} else {
					printf("m\n\nHmmm..\n\n");
				}
				continue;
			case 'u':
				printf("\n\n");
				UserToProfile();
				continue;
			case 'h':
			case '?':
				printf("\n");				
				printf("\nOPTIONS FOR READING MESSAGES\n");
				printf("\n");
				printf("  <A>..Again, Read THIS Msg\n");
				printf("  <->..Back One Msg\n");
				printf("  <P>..Post A Msg -> here.\n");
				printf("  <!>..Jump to a Msg\n");
				printf("  <Q>..Quit Reading Msgs\n");
				printf("  <R>..Reply to Current Msg\n");
				printf("  <D>..Delete this Message (n/a)\n");
				printf("  <^>..Colors help-text\n");
				printf("\n");
				printf(" [SPACE]..Goto NEXT msg\n");
				printf("\n");
				continue;
			case ' ':
				/*********************************************/
				/* at this point we should have two choices  */
				/* goto the next Message or jump to the next */
				/* subboard forum room ......................*/
				/*********************************************/
				if((msgNo >= lohi[1]) || (msgNo < lohi[0])) {
					printf("[END]\n\n\x1b[0;33mNo More Msgs In The [%s\x1b[0;33m] Room.\n\n", GetRoomName());
					printf("\x1b[0;34m  Hit [\x1b[1;34mSPACE\x1b[0;34m] bar to check the \x1b[1;34mnext\x1b[0;34m room..\n\n");
					done=TRUE;
					continue;
				}
				printf("Next");
				msgNo++;

				CatMsg(msgNo, lohi[1], rNum);

				userstate.lastmsg[rNum] = msgNo;
				update_user();
				continue;
			case '!':
				sprintf(ibuf, "Jump to Msg [%ld-%ld=:> ", lohi[0]+1, lohi[1]);
				printf(ibuf);
				GetStr(ibuf,5,0);
				if(!strisdigit(ibuf)) {
					printf("\n That string is not digits.\n\n");
					continue;
				}
				tmpMsgNo=atol(ibuf);

				if((tmpMsgNo <= lohi[1]) && (tmpMsgNo >= lohi[0])) {
					msgNo=tmpMsgNo;
					CatMsg(msgNo,lohi[1],rNum);
					userstate.lastmsg[0] = msgNo;
					update_user();
					continue;
				} else {
					printf("\n Msg %ld out of Range.\n", tmpMsgNo);
					printf(" Jump range is %ld to %ld.\n\n", lohi[0]+1,lohi[1]);
					continue;
				}
			case '-':
			case 'b':
				if(msgNo < lohi[0]+2) {
					printf("\x1b[0m[END]\n\nYou have reached the beginning of Room [%s\x1b[0m].\n", GetRoomName());
					done=TRUE;
					continue;
				}
				printf("Back");
				msgNo--;
				CatMsg(msgNo,lohi[1],rNum);
				continue;
			case 'w':
				printf("Who's Online\n");
				syslog(LOG_INFO,"%s %s",username,"whos online - rmsg");
				whois_on_list();
				continue;
			case '^':
				printf("Colors Help\n\n");
				sprintf(obuf,"%s/text/colors-help",BBSDIR);
				CatFileDirect(obuf);
				continue;
			default:
				printf("\n\n================================\n");
				printf(" Press [[1;5mSPACEBAR[0m] for next msg.\n\n");
				printf("Hit '?' for more options help.\n");
				continue;
		}
		printf("\n\n");
	}

	syslog(LOG_INFO,"%s %s %d %s %ld",
		username,
		"read",
		rNum,
		"messages, stopped at num",
		userstate.lastmsg[rNum]);
	return;
}

/*********************************************/
/* ResetFiveMsgs()                           */
/* This function goes throw all the messsage */
/* rooms and sets the users pointer to the   */
/* read the last five messages only..        */
/*********************************************/

void ResetFiveMsgs() {
	long lohi[2];
	int i;
	
	for(i=0;i<maxrooms;i++) {
		StatMsgs(lohi,i);
		if(i != 1) {
			if(lohi[1] > 5) userstate.lastmsg[i] = (long) lohi[1] - 5;
		} else {
			userstate.lastmsg[i] = 1;
		}
	}
	update_user_state();
}

int TotalNewMsgs() {
	long i, total=0;
	long lohi[2];
	for(i=0;i<maxrooms;i++) if(room[i].avail != TRUE) { 
		StatMsgs(lohi,i);
		if(lohi[1] != userstate.lastmsg[i])
			total += lohi[1] - userstate.lastmsg[i];
		printf(".");
		fflush(stdout);
	}	
	return total;
}

int SelectRoom() {
	char ibuf[20], obuf[200];
	int i,n;
	long lohi[2];
	
	GetStr(ibuf,4,NO_SPACE|NO_ATSIGN|NO_FORSLASH|NO_BAKSLASH|NO_DASH);
	if(!strcmp(ibuf,"?")) {
		CatFileNoPause("ROOMS");
		printf("\033[1;31mNew Rm# Description\n");
		printf("\033[0;31m~~~ ~~~ ~~~~~~~~~~~\n");
		for(i=0;i<maxrooms;i++) if(room[i].avail != TRUE) { 
			StatMsgs(lohi,i);
			if(lohi[1] != userstate.lastmsg[i])
				printf("\033[1;32m%3d ",lohi[1] - userstate.lastmsg[i]);
			else
				printf("    ");
			printf("\033[0;1;33m%2d. ",i+1);
			if(i != 1) printf("\033[0;35m%s\033[0;35m's\033[0m ",room[i].owner);
			printf("%s\n",room[i].name);
  		}
		return -2;
  	}

	if(strisdigit(ibuf)) {
		n = (atoi(ibuf)-1);
		if((n < 0) || (n > maxrooms-1) || (room[n].avail != FALSE)) {
			printf("\nThat room is not available.\n");
			return -1;
		} else {
			syslog(LOG_INFO,"%s %s %d",
				username,
				"selected room number",
				n+1);
			printf("\n");
			return n;
		}
	} else {
		printf("\nbad string..\n");
		syslog(LOG_INFO,"%s %s",username,"selectroom: not valid string");
		return -1;
	}
	return -1;
}


/*******************************************/
/* JoinRoom()                              */
/* This function shows a menu to the user  */
/* and allows them to join different rooms */
/*******************************************/
void JoinRoom() {
	int n;
	printf("\n\n\033[0;32mType [\033[1;33m?\033[0;32m] to see room list.\n");	
	for(;;) {
	  	printf("\n");
		printf("\x1b[1;34mYou are in Room #%d\n",rNum+1);
		printf("\x1b[0m[\x1b[1;34mENTER\x1b[0m] to stay here.\n\n");
	
		printf("\x1b[1;34mR\x1b[1;35mo\x1b[1;36mom #\033[1;35m[\033[1;34m1-%d\033[1;35m]\x1b[36m: ",maxrooms);
		n = SelectRoom();
		if(n == -1) {
			break;
		} else if(n == -2) {
			continue;
		} else {	
			rNum = n;
			JoinStats();
			break;
		}
	}
}


void JoinStats() {
	long lohi[2];
	printf("\033[1;34mJoining\033[0;34m: \033[0m%s\n", GetRoomName());
	StatMsgs(lohi,rNum);
	printf("\033[1;34m  Stats\033[0;34m: \033[1;34m%d \033[1;32mMessages",lohi[1]-lohi[0]);
	if(lohi[1] != userstate.lastmsg[rNum])
		printf("\033[0;35m, \033[1;34m%d \033[1;32mNew",lohi[1] - userstate.lastmsg[rNum]);
	printf("\033[0m\n");		
}

char *GetRoomName() {
	static char buf[130];	
	strcpy(buf,room[rNum].name);
	return (char *)buf;
}

int NewMsgs(int rrd, long lohi[2]) {

	if(StatMsgs(lohi,rNum) == -1) {
		return 0;
	}


	/********************************************/
	/* if user last msg less then first message */
	/* set user last msg to lowest msg number   */
	/* and read!                                */
	/********************************************/
	if(userstate.lastmsg[rNum] < lohi[0]+1) {
		userstate.lastmsg[rNum] = lohi[0]+1;
		update_user();
		if(rrd == 1) return 0;					// no, don't read
		else return (lohi[1] - userstate.lastmsg[rNum]);	// yes, read
	} 


	/*************************************************/
	/* if user last msg is greater then the high msg */
	/* set user last msg to the high msg             */
	/* and don't read!                               */
	/*************************************************/
	if(userstate.lastmsg[rNum] > lohi[1]) {
		userstate.lastmsg[rNum] = lohi[1];
		update_user();
		if(rrd == 1) return (lohi[0]+1 - userstate.lastmsg[rNum]);
		else return 0;
	} 
	
	/*****************************************/
	/* if user last msg is equal to high msg */
	/* don't read!                           */
	/*****************************************/
	if(userstate.lastmsg[rNum] == lohi[1]) {
		return 0;	// no dont' read
	}
	
	/******************************************/
	/* if user last msg is less then high msg */
	/* read!                                  */
	/******************************************/	
	if(userstate.lastmsg[rNum] < lohi[1]) {
		return (lohi[1] - userstate.lastmsg[rNum]); 		// yes, read
	}
}


void MakeRooms() {
	//sprintf(room[0].name,"\033[1;34m(o: \033[1;34mH\033[1;35ma\033[0;34mL\033[1;35mL \033[1;34mo\033[1;35mF \033[1;34mJu\033[1;35msT\033[0;34mi\033[1;35mCe \033[1;34m:o)");
	sprintf(room[0].name,"\033[0;31mDa\033[1mZed \033[0;31ma\033[1mNd \033[0;31mCoNf\033[1muSed");
	sprintf(room[0].path,"%s/msgs/lobby",BBSDIR);
	sprintf(room[0].owner,"tHe sHack!");
	room[0].toplimit = 50;
	room[0].avail = FALSE;
		
	sprintf(room[1].name,"\033[1;32m%s's Mailbox\033[0;31m",get_user_name());
	sprintf(room[1].path,"%s/users/%s/Mail",BBSDIR,get_user_name());
	sprintf(room[1].owner,"%s", get_user_name());
	room[1].toplimit = 100;
	room[1].avail = FALSE;
	
	sprintf(room[2].name,"\x1b[0;31mGeN\x1b[1;31meRaL \x1b[0;31mFo\x1b[1;31mRuM");
	sprintf(room[2].path,"%s/msgs/generalforum",BBSDIR);
	sprintf(room[2].owner,"tHe sHack!");
	room[2].toplimit = 50;
	room[2].avail = FALSE;
	
	sprintf(room[3].name,"\x1b[0;31mB\x1b[32mug \x1b[31mNo\x1b[32mtes \x1b[31m+ N\x1b[32mews"); 
	sprintf(room[3].path,"%s/msgs/bugnotes",BBSDIR);
	sprintf(room[3].owner,SYSOPNAME);
	room[3].toplimit = 75;
	room[3].avail = FALSE;
	
	maxrooms = GetRoomFile();
	if(maxrooms <= 4) {
		maxrooms = 4; 
		syslog(LOG_INFO,"%s","GetRoomFile: nothing loaded"); 
	}

	return;	
}


/*********************************************************
 * TODO:
 * make the other 28 available rooms user definable.
 * I need to have a file of 28 records.
 *  - Available YES/NO?
 *  - Room name (name should be in color);
 *  - Owner of room.
 *
 * The owner of the room will be able to delete messages,
 * and/or the room itself, they will only have one room
 * at a time.
 * 
 * the PATH will be ROOM5.. ROOM6.. ROOM7.. etc
 * the TOPLIMIT will be 50 for all rooms.
 *
 * for the furture, I might want an access list of who
 * is alowed to Join and Read the room.
 *
 * the room number will not move... if you use the Join
 * command, you will see whatever room number that is,
 * say if rooms 5,6,7 don't exist, and somebody owns
 * room 8, then you will see room 1,2,3,4,8 in the join
 * list.
 *
 * Two new config options should be added.. one is a
 * list of who can own rooms, and what room they own.
 * the other, if they have access to create a room, and
 * there are less then 28 rooms in use, if there is an
 * open room slot, then they will see an option to
 * create a room, also to delete the room if they have
 * a room already.
 *
 * 3. List All Roomies
 * 4. Create a Room
 * 5. Delete your Room
 *
 ********************************************************/


int GetRoomFile(void) {
	FILE *fp;
	int i,lastrm=0;
	char obuf[200],ibuf[200];

	for(i=4;i<MAXROOMS;i++) {
		sprintf(obuf,"%s/data/rooms/%d",BBSDIR,i);
		fp = fopen(obuf,"rb");
		if(fp != NULL) {
			fscanf(fp,"%[^~]~%[^~]~%[^~]~\n",
				room[i].owner,room[i].name,ibuf);
			sprintf(room[i].path,"%s/msgs/ROOM%d",BBSDIR,i);
			room[i].toplimit = atoi(ibuf);
			room[i].avail = FALSE;
			lastrm = i+1;
			fclose(fp);
		} else {
			sprintf(room[i].name,"Vacent Room");
			sprintf(room[i].path,"%s/data/msgs/room0",BBSDIR);
			sprintf(room[i].owner,"nobody");
			room[i].toplimit = 3;
			room[i].avail = TRUE;
		}
	}
	return(lastrm);
}




int MakeNewRoom(void) {
	FILE *fp;
	int i,openslot;
	char buf[200],buf1[200],buf2[12],ch;
	struct stat st;

	if(is_room_maker((char *)get_user_name()) == FALSE) {
		printf("\nYou aren't on the Room Makers List\n\n");
		return -1;		
	}
	
	MakeRooms();

	/* Check to see if there are available rooms */
	
	openslot = 0;
	
	for(i=4;i<MAXROOMS;i++) {
		sprintf(buf,"%s/data/rooms/%d",BBSDIR,i);
		if(stat(buf,&st) != 0) {
			openslot = i;
			break;
		}
	}	

	if(openslot == 0) {
		printf("\n\033[1;33mNo more rooms available, sorry!\n\n");
		return -1;
	}
				
	/* Now we know we have an open slot */

	printf("\n\033[0;33mEn\033[1mter \033[0;33mNa\033[1mme \033[0;33mo\033[1mf \033[0;33mYo\033[1mur \033[0;33mRo\033[1mom [\033[0;32m3-35 characters\033[0;33m=:> ");
	GetStr(buf,35,NO_ATSIGN);	
	if(strlen(buf) < 5) {
		printf("Name too short!\n");
		return -1;
	}
	
	printf("\n\033[1;34mC\033[1;36mo\033[1;36mL\033[1;37mo\033[1;31mR\033[1;32ms\033[1;33m!\n");
	printf("\033[1;34m~\033[1;36m~\033[1;36m~\033[1;37m~\033[1;31m~\033[1;32m~\033[1;33m~\n");

	printf("  \033[0m1. \033[1;36mMegenta   \033[0m4. \033[1;31mRed\n");
	printf("  \033[0m2. \033[1;34mBlue      \033[0m5. \033[1;32mGreen\n");
	printf("  \033[0m3. \033[1;37mWhite     \033[0m6. \033[1;33mYellow\n");
	
	printf("\n\033[0;36mPi\033[1mck a \033[0;36mCo\033[1mlor\033[0;36m? ");
	for(;;) {
		ch = GetKey(0);
		if(ch=='1'||ch=='2'||ch=='3'||ch=='4'||ch=='5'||ch=='6'||ch=='7') break;
	}
	
	switch(ch) {
		case '1':
			printf("\033[1;36mMegenta!\n");
			sprintf(buf2,"\033[1;36m");
			break;
		case '2':
			printf("\033[1;34mBlue!\n");
			sprintf(buf2,"\033[1;34m");
			break;
		case '3':
			printf("\033[1;37mWhite!\n");
			sprintf(buf2,"\033[1;37m");
			break;
		case '4':
			printf("\033[1;31mRed!\n");
			sprintf(buf2,"\033[1;31m");
			break;
		case '5':
			printf("\033[1;32mGreen!\n");
			sprintf(buf2,"\033[1;32m");
			break;
		case '6':
			printf("\033[1;33mYellow!\n");
			sprintf(buf2,"\033[1;33m");
			break;
		default:
			printf("\033[0mCyan!\n");
			sprintf(buf2,"\033[0m");
			break;
	}
	
	printf("\033[0;34m\nRoom Name: %s%s \033[1;34mNo.%d\n",buf2,buf,openslot);
	printf("\033[1;32mOwned By: \033[1;31m%s\n\n",get_user_name());
	printf("\033[0;35mSa\033[1mving \033[0;35mRo\033[1mom \033[0;35mIn\033[1mfo..");

	sprintf(buf1,"%s/data/rooms/%d",BBSDIR,openslot);
	if(fp = fopen(buf1,"wb"));
	if(fp != NULL) {
		fprintf(fp,"%s~%s%s~%d~\n",get_user_name(),buf2,buf,50);
		syslog(LOG_INFO,"%s %s %s!",
			username,
			"made a room called",
			buf);
		fclose(fp);
	} else {
		syslog(LOG_INFO,"%s %s %s %s",
			username,
			"could not make new room",
			buf,
			": fopen failed!");
		fclose(fp);
		return -1;
	}	
	MakeRooms();
	printf("\033[0;35mdo\033[1;35mne!\033[0m\n");
	return 0;
}

