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

static struct termios stored;
long chatfilesize;
long cchatfsize;



void broadcast(const char *obuf) {
	FILE *fp;	
	if((fp = fopen(CHATFILE,"ab")) == NULL) {
		if(errno == ENOENT) { 
			syslog(LOG_INFO,"%s","CHATFILE does not exist");
			syslog(LOG_INFO,"%s","creating a new one");
			if((fp = fopen(CHATFILE,"wb")) == NULL) {
				syslog(LOG_NOTICE,"%s",
					"can not create CHATFILE");
				return;
			}
			fprintf(fp,"====================");
			fprintf(fp,"CHATFILE starts here");
			fprintf(fp,"====================");
			fprintf(fp,"%s", obuf);
			fflush(fp);
			fclose(fp);
			return;
		} else { 
			syslog(LOG_NOTICE,"%s",
				"can not open CHATFILE at all"); 
			return;
		}
	}
	
	fprintf(fp,"%s", obuf);
	fflush(fp);
	fclose(fp);
	return;
}


void resetcchatffp() {
	struct stat st;
	stat(CCHATF,&st);
	cchatfsize = st.st_size;
	return;
}

void resetchatfp() {
	struct stat st;
	stat(CHATFILE,&st);
	chatfilesize = st.st_size;
	return;	
}

int newcchatffp() {
	struct stat st;
	stat(CCHATF,&st);
	if(cchatfsize < st.st_size) {
		return TRUE;
	} else if(cchatfsize > st.st_size) {
		cchatfsize = st.st_size;
		return FALSE;
	} return FALSE;
}

int newchatfp() {
	struct stat st;
	stat(CHATFILE,&st);
	if(chatfilesize < st.st_size) {
		return TRUE;
	} else if(chatfilesize > st.st_size) {
		chatfilesize = st.st_size;
		return FALSE;
	} return FALSE;
}

void readcchatffp() {
	FILE *fp;
	int c;

	fp = fopen(CCHATF,"rb");
	if(fp == NULL) {
		syslog(LOG_NOTICE,"%s", "can't read CCHATF");
		return;
	}
	fseek(fp, cchatfsize, SEEK_SET);
	while((c = fgetc(fp)) != EOF) printf("%c",c);
	fclose(fp);
	resetcchatffp();
	return;
}

void readchatfp() {
	FILE *fp;
	int c;
	
	fp = fopen(CHATFILE,"rb");
	if(fp == NULL) {
		syslog(LOG_NOTICE,"%s", "can't read CHATFILE");
		return;
	}
	fseek(fp, chatfilesize, SEEK_SET);
	while((c = fgetc(fp)) != EOF) printf("%c",c); 
        fclose(fp);                                
	resetchatfp();
	return;
}

	
void Yell() {
	char *ibuf,*obuf;
	printf("\n\x1b[1;35mEnter Msg To Broadcast!\n\x1b[0;34m:\x1b[32m ");
	ibuf = calloc(79,sizeof(char));
	GetStr(ibuf, 77,0);
	if(ibuf[0] != '\0') {
		obuf = calloc(256,sizeof(char));
		sprintf (obuf, "\x1b[0;35m<\x1b[33m%s\x1b[35m> \x1b[36m%s\n\x1b[0m", get_user_name(), ibuf);
		broadcast(obuf);
		free(obuf);		
	} else printf("Broadcast cancelled.\n");
	free(ibuf);
	return;
}



int fd_has_data (const int timeout, const int fd)
{
	fd_set rfds;
	struct timeval tv;
      
	FD_ZERO (&rfds);
	FD_SET (fd, &rfds);
	tv.tv_sec = 0;
	tv.tv_usec = timeout;
	select (fd + 1, &rfds, NULL, NULL, &tv);
  
	if(FD_ISSET(fd, &rfds)) 
	   	return TRUE; 
	else 
		return FALSE;
  
}


                              
/************
 ** this func gets a keyboard press and prints the
 ** prompt, also handles incomming chat messages.
 ** also has a timeout.
 ***/
int GetKeyPrompt(const char *prompttext) {

	int cnt=0,ch=0;
	char buf[2];
	struct termios new;
	time_t start_time=0,now=0;

	tcgetattr(0,&stored);	
	memcpy(&new,&stored,sizeof(struct termios));
	new.c_lflag &= (~ICANON);
	new.c_cc[VTIME] = 0;
	new.c_cc[VMIN] = 1;
	new.c_lflag &= (~ECHO);
	tcsetattr(0,TCSANOW,&new);

	if(newchatfp()) readchatfp();
	printf (prompttext);
	fflush(stdout);
 	time(&start_time);
 	 
	for(;;) {
		fflush(stdout);
		
		/***********************************/
		/* check to see if there sleeping. */
		/***********************************/
		time(&now);
		if((now-start_time) > IDLETIMEOUT) {
			printf("\n\n\07\x1b[0;1;33m*** %s is sleeping *** \x1b[34m(\x1b[35m%d sec idle timeout\x1b[34m)", get_user_name(), IDLETIMEOUT);
			syslog(LOG_INFO,"%s %s",
				get_user_name(),
				"is sleeping, logging off");
			logout();
		}
		
		if(fd_has_data(500000, 0)) {
			read(0, buf, 1);
			ch = buf[0];
			if(ch == '\n') break;
			if(!isprint(ch)) continue;

			/***********************************/
			/* nothing left, return the ch key */
			/***********************************/			
			break;
		}
		if(newchatfp()) {
			printf("\r\x1b[K");
			readchatfp();
			printf(prompttext);
			continue;
		}
	}  
	
	/************************************/
	/* now we have a ch so do something */
	/************************************/

  	tcsetattr(0,TCSANOW,&stored);
  	return ch;


}




/**************************************************************************/
/**                                                                      **/
/**                         new chat functions                           **/
/**                                                                      **/
/**************************************************************************/

void GetStr(char *ibuf, int max, const int mode) {
	
	int cnt, ch;	
	cnt = 0;
	ch  = 1;
	
	max++;
	for(;;) {	
		ch = GetKey(mode);
		if((mode & NO_DASHFIRST) && (cnt == 0) && (ch == '-')) continue;
		if(((ch == '\b') || (ch == '\x7f')) && cnt == 0) continue;
		if((ch == '\b') || (ch == '\x7f')) {
			cnt--;
			printf("\b \b");
			continue;
		}
		if(ch == '\n') {
			ibuf[cnt] = '\0';
			cnt = 0;
			printf("\n");
			break;
		}
		if(max > 1 && cnt == max-1) continue;
		if(ch < 32) continue;
		
		cnt++;
		if(mode & ECHO_DOTS) printf("."); else printf("%c",ch);
		ibuf[cnt-1] = (char) ch;
		ibuf[cnt] = '\0';		
		if(max == 1) {
			ibuf[cnt] = '\0';
			break;
		}
	}
	return;
}

/***************************************************/
/* GetKey()                                        */
/* - this is the master GetKey function, this used */
/* to be a bunch of functions, but now it is all   */
/* rolled into one.  The mode tells it waht to do. */
/* - available modes:                              */
/* ECHO_DOTS - not used here, but used in GetStr   */
/* NO_SPACE - do not allow space                   */
/* NO_ATSIGN - do not allow the @ sign             */
/* NO_FORSLASH - do not allow the forward slash /  */
/* NO_BAKSLASH - do not allow the back slash \     */
/* YES_CTRLX - allow the CTRL-X key                */
/* YES_ESCKEY - allow the ESC key                  */
/* NO_DASH - do not allow the '-' at all           */
/* NO_DASHFIRST - used with GetStr only            */
/* - returns:                                      */
/* the character key that was pressed on stdin     */
/***************************************************/
int GetKey(int mode) {
	int cnt=0,ch=0;
	char buf[2];
	struct termios new;
	time_t start_time=0,now=0;

	tcgetattr(0,&stored);	
	memcpy(&new,&stored,sizeof(struct termios));
	new.c_lflag &= (~ICANON);
	new.c_cc[VTIME] = 0;
	new.c_cc[VMIN] = 1;
	new.c_lflag &= (~ECHO);
	tcsetattr(0,TCSANOW,&new);

 	time(&start_time);
 	 
	for(;;) {
		fflush(stdout);
		
		/***********************************/
		/* check to see if there sleeping. */
		/***********************************/
		time(&now);
		if((now-start_time) > IDLETIMEOUT) {
			printf("\n\n\07\x1b[0;1;33m*** %s is sleeping *** \x1b[34m(\x1b[35m%d second idle timeout\x1b[34m)", get_user_name(),IDLETIMEOUT);
			syslog(LOG_INFO,"%s %s",
				get_user_name(),
				"is sleeping on the editor");
			logout();
		}
		
		if(fd_has_data(500000, 0)) {
			read(0, buf, 1);
			ch = buf[0];
			if(ch == '\n') break;
			if(ch == '\b') break;
			if(ch == '\x7f') break;
			if(mode & NO_SPACE) if(isspace(ch)) continue;
			if(mode & NO_ATSIGN) if(ch == '@') continue;
			if(mode & NO_FORSLASH) if(ch == '/') continue;
			if(mode & NO_BAKSLASH) if(ch == '\\') continue;
			if(mode & YES_CTRLX) if(ch == 24) break;
			if(mode & YES_ESCKEY) if(ch == 27) break;
			if(mode & NO_DASH) if(ch == '-') continue;
			if(!isprint(ch)) continue;
			/***********************************/
			/* nothing left, return the ch key */
			/***********************************/			
			break;
		}
	}  
	
	/************************************/
	/* now we have a ch so do something */
	/************************************/

  	//tcsetattr(0,TCSANOW,&stored);
	fflush(stdout);
  	return ch;
}


void GetStrName(char *ibuf, int max) {
	
	int cnt, ch, upflag;	
	cnt    = 0;
	ch     = 1;
	upflag = 1;

	max++;
	for(;;) {	
		ch = GetKey(NO_ATSIGN | NO_FORSLASH | NO_BAKSLASH | NO_DASH);
		if(ch == ' ' && upflag == 1) continue;
		
		if(((ch == '\b') || (ch == '\x7f')) && cnt == 0) continue;
		if((ch == '\b') || (ch == '\x7f')) {
			/**************************************************/
			/* here I check if the last character was Capital */
			/* if so, I make upflag = 1;                      */
			/**************************************************/
			cnt--;
			if(isupper(ibuf[cnt])) upflag = 1;
			else upflag = 0;
			printf("\b \b");
			continue;
		}
		if(ch == '\n') {
			ibuf[cnt] = '\0';
			cnt = 0;
			printf("\n");
			break;
		}
		if(max > 1 && cnt == max-1) continue;
		if(ch < 32) continue;
		
		if(upflag == 1) {
			cnt++;
			ibuf[cnt-1] = (char) toupper(ch);
			ibuf[cnt] = '\0';
			printf("%c", toupper(ch));
			upflag = 0;
		} else {
			cnt++;
			ibuf[cnt-1] = (char) ch;
			ibuf[cnt] = '\0';
			printf("%c", ch);
		}

		if(ch == ' ' && upflag == 0) 
			upflag = 1;
		else
			upflag = 0;
		
		if(max == 1) {
			ibuf[cnt] = '\0';
			break;
		}
	}
	return;
}


int GetKeyArrows(void) {
	int ch=0;
	ch = GetKey(YES_ESCKEY | YES_CTRLX);
	if(ch == 27) {
		ch = GetKey(0);
		if(ch == '[') {
			ch = GetKey(0);
			if(ch == 'A') return 1001;
			else if(ch == 'B') return 1002;
			else if(ch == 'C') return 1003;
			else if(ch == 'D') return 1004;
			else return ch;
		} else 
			return ch;
	}
  	return ch;
}


int ChatRoomPrompt(void) {
	int cnt=0,ch=0,max=70;
	char buf[2],ibuf[256];
	struct termios new;
	time_t start_time=0,now=0;
	char prompttext[30];
	FILE *fp;
	
	tcgetattr(0,&stored);
	memcpy(&new,&stored,sizeof(struct termios));
	new.c_lflag &= (~ICANON);
	new.c_cc[VTIME] = 0;
	new.c_cc[VMIN] = 1;
	new.c_lflag &= (~ECHO);
	tcsetattr(0,TCSANOW,&new);

	
	printf("\n\nChat Room History\n");
	printf("-----------------\n");
	TailFile(CCHATF,15);
	resetcchatffp();
	printf("\n\n\033[1;31mWeLcoMe to The ShaCk's Chat Station!\n",get_user_name());
        printf("\033[1;33m------------------------------------\n");

        printf("\033[1;31m -=> \033[1;36mEacH LiNe iS 7o cHaRacTeRs \033[1;31m<=-\n");
	printf("\033[1;31m  --+- \033[0;35mType '\033[1;31mhelp\033[0;35m' for Chat Menu.\n");
	printf("\033[1;31m    -+- \033[0;35mType '\033[1;31mquit\033[0;35m' for Exit Chat..\n");
        printf("\033[1;33m------------------------------------\n\n");
	sprintf(prompttext,"\033[0;32mChat\033[1;32m>> \033[1;34m");
	ibuf[0]='\0';
	if(newchatfp()) readchatfp();
	if(newcchatffp()) readcchatffp();
	printf(prompttext);
	fflush(stdout);
        if((fp = fopen(CCHATF,"ab")) != NULL) {
		fprintf(fp,"\033[0;32m[\033[1;32m%s\033[0;32m] enters the chat room!\n",get_user_name());
		fclose(fp);
	}
	for(;;) {
		time(&start_time);
		fflush(stdout);
		for(;;) {
			fflush(stdout);
			time(&now);
			if((now-start_time) > IDLETIMEOUT) {
				printf("\n\n\07\033[0;1;33m*** %s is sleeping *** \033[34m(\033[35m%d second idle timeout\033[34m)", get_user_name(),IDLETIMEOUT);	
				syslog(LOG_INFO,"%s %s",
					get_user_name(),
					"is sleeping, logging off");
				logout();
			}

			if(fd_has_data(500000, 0)) {
				read(0, buf, 1);
				ch = buf[0];
				if(ch == '\n') break;
				if(ch == '\b') break;
				if(ch == '\x7f') break;
				if(!isprint(ch)) continue;
				/***************************/
				/* here we can put filters */
				/***************************/

				/**********************/
				/* we have a key, hmm */
				/**********************/
				break;
			}
			if(newchatfp()) {
				printf("\r");
				fflush(stdout);
				printf("\033[K");
				fflush(stdout);
				readchatfp();
				printf("%s", prompttext);
				fflush(stdout);
				if(ibuf[0] != '\0') printf("%s",ibuf);
				fflush(stdout);
				continue;
			}
			if(newcchatffp()) {
				printf("\r");
				fflush(stdout);
				printf("\033[K");
				fflush(stdout);
				readcchatffp();
				printf("%s", prompttext);
				fflush(stdout);
				if(ibuf[0] != '\0') printf("%s",ibuf);
				fflush(stdout);
				continue;
			}
		}
		
		/*****************************/
		/* a ch exists at this point */
		/*****************************/

		if(((ch == '\b') || (ch == '\x7f')) && cnt == 0) continue;
		if((ch == '\b') || (ch == '\x7f')) {
			cnt--;
			ibuf[cnt]='\0';
			printf("\b \b");
			fflush(stdout);
			continue;
		}
		if((ch == '\n')) {
			if(cnt < 2) continue;
			if(!strcmp(ibuf,"quit")) break;
			if(!strcmp(ibuf,"exit")) break;
			if(!strcmp(ibuf,"help")) {
				printf("\n \033[1;33m-> Type '\033[1;31mquit\033[1;33m' or '\033[1;31mexit\033[1;33m' to leave chat.");
				printf("\n \033[1;33m-> Type '\033[1;31mwho\033[1;33m' to display all users online.");
				printf("\n \033[1;33m-> Type '\033[1;31mhelp\033[1;33m' to display this menu.\n");
				printf("%s", prompttext);
				ibuf[cnt]='\n';
				ibuf[cnt+1]='\0';
				cnt = 0;
				continue;
			}
			if(!strcmp(ibuf,"who")) {
				printf("\n");
				whois_on_list();
				printf("%s", prompttext);
				ibuf[cnt]='\n';
				ibuf[cnt+1]='\0';
				cnt = 0;
				continue;
			}
	
			ibuf[cnt]='\n';
			ibuf[cnt+1]='\0';
			cnt = 0;
 			//printf("\n");
			if((fp = fopen(CCHATF,"ab")) == NULL) {
				if(errno == ENOENT) {
					syslog(LOG_INFO,"%s",
					"CCHATF does not exist");
					syslog(LOG_INFO,"%s",
					"creating a new one");
					if((fp = fopen(CCHATF,"wb")) == NULL) {
						syslog(LOG_NOTICE,"%s",
						"can not create CCHATF");
						break;
					}
					fprintf(fp,"==================");
					fprintf(fp,"CCHATF starts here");
					fprintf(fp,"==================");
					fprintf(fp,"%s> %s",get_user_name(),ibuf);
					fflush(fp);
					fclose(fp);
					printf(prompttext);
					continue;
				} else {
					syslog(LOG_NOTICE,"%s",
					"can not open CCHATF at all");
					break;
				}
			} else if((!strncmp(ibuf,"me ",3)) && (strlen(ibuf) > 3)) {
				fprintf(fp," \033[1;36m* %s\033[1;35m%s",get_user_name(),&ibuf[2]);
				ibuf[0]='\0';
				fflush(fp);
				fclose(fp);
				ibuf[0]='\0';
				continue;
			} else {
				fprintf(fp,"\033[1;34m<\033[1;31m%s\033[1;34m> \033[0;32m%s",get_user_name(),ibuf);
				ibuf[0]='\0';
				fflush(fp);
				fclose(fp);
				ibuf[0]='\0';
				continue;
			}
		}
		if(max > 1 && cnt == max) continue;
		if(ch < 32) continue;
		if(ch > 127) continue;
		cnt++;
		printf("%c",ch);
		ibuf[cnt-1] = (char) ch;
		ibuf[cnt] = '\0';
		fflush(stdout);

	}
	tcsetattr(0,TCSANOW,&stored);
	printf("\n\nquitting chat..\n");
	fflush(stdout);
	if((fp = fopen(CCHATF,"ab")) != NULL) {
		fprintf(fp,"\033[0;35m[\033[1;32m%s\033[0;35m] leaves the chat room!\n",get_user_name());
		fclose(fp);
	}
	sleep(1);	
	return;
}

