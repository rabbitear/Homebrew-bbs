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

/*****************************/
/* new circluar message base */
/* reading and writing msgs. */
/*****************************/

/*
 * I need a afew functions for maintainance of the base
 * like when the base doesn't exist, create it.
 */

#include "shack.h"
#include "msg.h"
#include "rooms.h"

/*************************************************************************
 * things I'm not sure about:
 * o finding the last NEXT position of the message bodytext
 * o where to put the code to wrap after 50 messages, or when file is
 *   too big
 *****************/
 

int WriteNewMsg(MSGBODY msg, const int rnum) {
	/********************************************************
	 * here we have to find the last message in the index
	 * then know if its the first
	 * then write the new message
	 * close
	 ***********************/
	FILE *fp;
	long lastMsgNo=0;
	MSGBODY tmp_msg;
	/**************************
	 * only the pos may reset to the beginning,
	 * the msg number never gets reset! is continueous
	 * (note: I'll have to do something after 2billion messages
	 */

	msg.noshow = FALSE;
	
	if((fp = fopen(room[rnum].path,"rb+")) == NULL) {
		if(errno == ENOENT) {
			syslog(LOG_NOTICE,"%s",
				"MESSAGEFILE does not exist yet");
			syslog(LOG_NOTICE,"%s",
				"creating a new one");
			if((fp = fopen(room[rnum].path,"wb")) == NULL) {
				syslog(LOG_NOTICE,"%s",
					"can not create MESSAGEFILE");
				return -1;
			}
			msg.number=1;
			fwrite((char *)&msg,sizeof(MSGBODY),1,fp);
			fclose(fp);
			return 0;
		} else {			
			syslog(LOG_NOTICE,"%s",
				"can not open MESSAGEFILE at all");
			return -1;
		}
	}

	/*********************************************/
	/* we've have an open message base file      */
	/* now we must find what the last record is. */
	/* then write to the next, or circle around. */
	/*********************************************/


	do {
		if(fread((char *)&tmp_msg,sizeof(MSGBODY),1,fp) != 1)
			if(feof(fp)) 
				if((tmp_msg.number % room[rnum].toplimit) == 0) {
					/*************************************/
					/* we are at the end of the circle   */
					/* end of file and we rewind to keep */
					/* looking for the highest number    */
					/*************************************/
					rewind(fp);
					continue;
				} else if(tmp_msg.number < room[rnum].toplimit) {
					/**********************************************/
					/* this means you have less then MAXMESSAGES  */
					/* so don't bother rewinding, or seeking back */
					/**********************************************/ 
					msg.number = tmp_msg.number + 1;
					fwrite((char *)&msg,sizeof(MSGBODY),1,fp);
					fclose(fp);
					return 0;
				} else return -1; /* some weirdness happened */
		
		/************/
		/* the test */
		/*          */
		/************/

		if(tmp_msg.number > lastMsgNo) { /* this is NOT the last message */		
			lastMsgNo = tmp_msg.number;
			continue;

		} else if(tmp_msg.number < lastMsgNo) {
			msg.number = lastMsgNo + 1;
			fseek(fp,((long)-sizeof(MSGBODY)),SEEK_CUR);
			fwrite((char *)&msg,sizeof(MSGBODY),1,fp);
			fclose(fp);
			return 0;
		} else {
			syslog(LOG_NOTICE,"%s",
				"something about message numbering couldn't write");
			break;
		}
		
		
	} while(!feof(fp));

	fclose(fp); /* should hopefully not reach this point */
	return 0;
}



/************************************/
/* this writes an email to any user */
/************************************/

int WriteNewMail(MSGBODY msg, const char *whoto) {
	/********************************************************
	 * here we have to find the last message in the index
	 * then know if its the first
	 * then write the new message
	 * close
	 ***********************/
	FILE *fp;
	long lastMsgNo=0;
	MSGBODY tmp_msg;
	char topath[256];
	int rnum;
		
	/**************************
	 * only the pos may reset to the beginning,
	 * the msg number never gets reset! is continueous
	 * (note: I'll have to do something after 2billion messages
	 */

	msg.noshow = FALSE;
	rnum = 1;
	sprintf(topath,"%s/users/%s/Mail",BBSDIR,whoto);
	
		
	if((fp = fopen(topath,"rb+")) == NULL) {
		if(errno == ENOENT) {
			syslog(LOG_NOTICE,"%s",
				"MESSAGEFILE does not exist");
			syslog(LOG_NOTICE,"%s",
				"creating a new one");
			if((fp = fopen(topath,"wb")) == NULL) {
				syslog(LOG_NOTICE,"%s",
					"can not create MESSAGEFILE");
				return -1;
			}
			msg.number=1;
			fwrite((char *)&msg,sizeof(MSGBODY),1,fp);
			fclose(fp);
			return 0;
		} else {			
			syslog(LOG_NOTICE,"%s",
				"can not open MESSAGEFILE at all");
			return -1;
		}
	}

	/*********************************************/
	/* we've have an open message base file      */
	/* now we must find what the last record is. */
	/* then write to the next, or circle around. */
	/*********************************************/


	do {
		if(fread((char *)&tmp_msg,sizeof(MSGBODY),1,fp) != 1)
			if(feof(fp)) 
				if((tmp_msg.number % room[rnum].toplimit) == 0) {
					/*************************************/
					/* we are at the end of the circle   */
					/* end of file and we rewind to keep */
					/* looking for the highest number    */
					/*************************************/
					rewind(fp);
					continue;
				} else if(tmp_msg.number < room[rnum].toplimit) {
					/**********************************************/
					/* this means you have less then MAXMESSAGES  */
					/* so don't bother rewinding, or seeking back */
					/**********************************************/ 
					msg.number = tmp_msg.number + 1;
					fwrite((char *)&msg,sizeof(MSGBODY),1,fp);
					fclose(fp);
					return 0;
				} else return -1; /* some weirdness happened */
		
		/************/
		/* the test */
		/*          */
		/************/

		if(tmp_msg.number > lastMsgNo) { /* this is NOT the last message */		
			lastMsgNo = tmp_msg.number;
			continue;

		} else if(tmp_msg.number < lastMsgNo) {
			msg.number = lastMsgNo + 1;
			fseek(fp,((long)-sizeof(MSGBODY)),SEEK_CUR);
			fwrite((char *)&msg,sizeof(MSGBODY),1,fp);
			fclose(fp);
			return 0;
		} else {
			syslog(LOG_NOTICE,"%s",
				"something about message numbering couldn't write");
			break;
		}
		
		
	} while(!feof(fp));

	fclose(fp); /* should hopefully not reach this point */
	return 0;
}



/**************************/
/*   prints any message   */
/*                        */
/**************************/
void PrintMsg(const MSGBODY msg, const long hiMsg, const int rnum) {

	int i=0,lc=0,x=0;
	char buf[8000],short_buf[2];
	printf("\n");	
	if(rnum == 0) {

		printf("\033[1;31m _         _ _     _   _\n");
		printf("\033[1;31m| |__ _  _| | |___| |_(_)_ _  ___\n");
		printf("\033[1;31m| '_ \\ || | | / -_)  _| | ' \\(_-<\n");
		printf("\033[1;31m|_.__/\\_,_|_|_\\___|\\__|_|_||_/__/\n");
		printf("\n");

		printf("\033[0;32mPosted by \033[1;33m%s \033[1;31m(\033[0;32m",msg.uhandle);
		printago(msg.date); printf(" ago\033[1;31m)\n");
	
		printf("\033[0;31m_.-_.-_.-_.-_.-_.-_.-_.-_.-_.-_.-_.-_.-\n");

		printf("\033[0;1;32m\n");
		lc = 9;
	} else {
		printf("\x1b[1;30m .-==-==-==-==-==-==-==-=--\n");
		printf("\x1b[1;30m | \x1b[0;37;44m Post:\x1b[0m \x1b[36mno. %ld \033[0;36m(\033[1;34m", msg.number);
		printago(msg.date); printf(" ago\033[0;36m)\033[0m\n");
	        printf("\x1b[1;30m | \x1b[0;37;44m From:\x1b[0m \x1b[1;32m%s \x1b[0;31mto ",msg.uhandle);
	        if(!strcmp(msg.uto,(char *)get_user_name())) printf("\x1b[0;35m[\x1b[1;5;32m%s\x1b[0;35m]\n", msg.uto);
	        else printf("\x1b[0;35m[\x1b[1;32m%s\x1b[0;35m]\n",msg.uto);
	        printf("\x1b[1;30m | \x1b[0;37;44m Rm%2d:\x1b[0m \x1b[32m%s\n",rnum+1,room[rnum].name);
	        printf("\x1b[1;30m | \x1b[0;37;44m Subj:\x1b[0m \x1b[35m%s\n",msg.subject);
	        printf("\x1b[1;30m `-==-==-=--\x1b[0;33m\n");
		lc = 7;
	}
	while(msg.text[i] != '\0') {
		if(msg.text[i] == '\n') {
			if(lc >= (rows-1)) {
				printf("%s\n",buf);
				if(!More2()) break;
				lc = 0;
				printf("\033[0;33m");
				buf[0] = '\0';
			} else {
				printf("%s\n",buf);
				lc++;
				buf[0] = '\0';
			}	
		} else {
			short_buf[0] = msg.text[i];
			short_buf[1] = '\0';
			strcat(buf,short_buf);	
		}
		/* increment the reading 'pointer' */
		i++;
	}

	strcpy(last_user_buffer,msg.uhandle);
	printf("\n");
	return;
}


int CatMsg(const long msgNo, const long hiMsg, const int rnum) {
	FILE *fp;
	MSGBODY msg;
	
	if((fp = fopen(room[rnum].path,"rb")) == NULL) return -1;

	do {
		if(fread((char *)&msg,sizeof(MSGBODY),1,fp) != 1) return -1;
			if(msgNo == msg.number) 
				if(msg.noshow == TRUE) {
					printf("\n\n\033[1;35mMessage \033[1;31m%ld \033[1;35mis missing!\n\n\x1b[0m",msg.number);
					break;					
				} else {
					PrintMsg(msg,hiMsg, rnum);
					break;
				}
	} while(!feof(fp));
	fclose(fp);
	return 0;
}


int MoveMsg(const long msgNo, const long hiMsg, const int rnum) {
	FILE *fp;
	MSGBODY msg;
	int n;
	
	if((fp = fopen(room[rnum].path,"rb")) == NULL) return -1;

	do {
		if(fread((char *)&msg,sizeof(MSGBODY),1,fp) != 1) return -1;
			if(msgNo == msg.number) 
				if(msg.noshow == TRUE) {
					printf("\n\n\x1b[0;32mMessage [%ld] deleted, can not move.\n\n\x1b[0m",msg.number);
					break;					
				} else {
					/* Ask user where the message should go */
					for(;;) {
						printf("\n\033[0mMove Msg#\033[1m%d\033[0m to Room? ",msgNo);
						n = SelectRoom();	
						if(n == -1 || n == 1) {
							printf("\033[0;1;35mNo..\033[0m\n\n");
							fclose(fp);
							return 0;
						} else if (n == -2) {
							continue;
						} else {
							break;
						}
					}
					/* Here we have to write the new message */
					/* to the new base, first, if sucess, we */
					/* can delete the old message            */
					if(!WriteNewMsg(msg, n)) {
						printf("\033[1;32mMove successful!\n\n\033[0m");
						/* delete the old message */
						DeleteMsg(msgNo, rnum);
					} else {
						printf("\033[1;33mMove failed!\n\n\033[0m");
					}
					break;
				}
	} while(!feof(fp));
	fclose(fp);
	return 0;
}


char *GetMsgAuthor(const long msgNo, const int rnum) {
	FILE *fp;
	MSGBODY msg;
	static char buf[32];

	strcpy(buf,"nobody");
	if((fp = fopen(room[rnum].path,"rb")) == NULL) return -1;

	do {
		if(fread((char *)&msg,sizeof(MSGBODY),1,fp) != 1) return -1;
		if(msgNo == msg.number) {
			strcpy(buf,msg.uhandle);
			break;
		}
	} while(!feof(fp));
	fclose(fp);
	return (char *)buf;
}


char *GetMsgSubject(const long msgNo, const int rnum) {
	FILE *fp;
	MSGBODY msg;
	static char buf[32];

	strcpy(buf,"none..");
	if((fp = fopen(room[rnum].path,"rb")) == NULL) return -1;

	do {
		if(fread((char *)&msg,sizeof(MSGBODY),1,fp) != 1) return -1;
		if(msgNo == msg.number) {
			strcpy(buf,msg.subject);
			break;
		}
	} while(!feof(fp));
	fclose(fp);
	return (char *)buf;
}


void DeleteMsg(int msgNo, const int rnum) {
	FILE *fp;
	MSGBODY msg;
	if((fp = fopen(room[rnum].path,"rb+")) == NULL) return;
	do {
		if(fread((char *)&msg,sizeof(MSGBODY),1,fp) != 1) return;
			if(msgNo == msg.number) {
				msg.noshow = TRUE;
				fseek(fp,((long)-sizeof(MSGBODY)),SEEK_CUR);
				fwrite((char *)&msg,sizeof(MSGBODY),1,fp);
				break;
			}
	} while(!feof(fp));
	fclose(fp);
	return;
}


int StatMsgs(long lohi[2], const int rnum) {
	FILE *fp;
	MSGBODY msg;
	long lastMsg=0;
	
	lohi[0]=0;
	lohi[1]=0;
	
	if((fp = fopen(room[rnum].path,"rb")) == NULL) return -1;
	
	do {
		if(fread((char *)&msg,sizeof(MSGBODY),1,fp) != 1) if(feof(fp)) break;
			
		     if(msg.number > lastMsg) 	lohi[1] = msg.number;
		else if(msg.number < lastMsg) { lohi[0] = msg.number; break; }
				
		lastMsg = msg.number;
		
	} while(!feof(fp));

	fclose(fp);
	return 0;
}


