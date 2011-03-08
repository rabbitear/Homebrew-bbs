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
#include <dirent.h>

int MakeDropFile(void);

/**************************/
/* logs user in and loads */
/* state and info files   */
/**************************/

int login(void) {
	char abuf[500];
	int i=0;
	
	/*******************************************/
	/* this login function still needs work.   */
	/* mainly the user 'should' have more then */
	/* one try to get in, and it should be as  */
	/* case insensitive as possible            */
	/* 0 = user found and loaded               */
	/* 1 = new user                            */
	/* 2 = no does not exist 		   */
	/* 3 = logoff				   */
	/*******************************************/

	loginagain:
	if(i >= 5) return 3;
	printf("\033[0;32mType '\033[1;31mNEW\033[0;32m' or Login \033[1;33m=:>\033[0;32m ");
	GetStrName(abuf,30);
	if(abuf[0] == '\0') { 
		i++;
		fflush(stdout);
		sleep(1); 
		printf("\nTry again.\n\n"); 
		fflush(stdout);
		sleep(1); 
		goto loginagain;}
	if(!strcmp(abuf,"New")) return 1;
	if(!strcmp(abuf,"NEW")) return 1;
	if(!strcmp(abuf,"Bye")) return 3;
	if(!strcmp(abuf,"Off")) return 3;
	if(!strcmp(abuf,"Quit")) return 3;
	if(!strcmp(abuf,"Exit")) return 3;
	if(existuser(abuf) == TRUE) { 
		strcpy(username,abuf);
		if(get_user_info() != 0) exit(0);
		if(get_user_state() != 0) exit(0); 
		AskPass();
		return 0;
	} else { 
		sleep(1);
		printf("\n");
		printf("\033[1;32mMsg To Users:\n");
		printf("\033[0;32m   --------------\n");
		printf("\033[1;32mFor any problems with logging in,\n");
		printf("\033[1;32meMail '\033[1;31msdelic@sdelic.org\033[1;32m' We'll Fix You!\n");
		printf("\n\033[0;32m -> \033[1;32m[\033[0;32m%s\033[1;32m] Unknown User! (sp?)\n",abuf);
		sleep(1);
		printf("\033[0;32m -> \033[1;32mType `\033[1;31mNew\033[1;32m` to create an account.\n\n");
		i++;
		goto loginagain;
	}
	return 3;
}

/*********************/
/* AskPass()         */
/* ask for password! */
/*********************/

void AskPass(void) {
	int i=0;
	char buf[30];
	
	askpass_top:
	
	i++;
	printf("\n\033[0;32mPassword \033[1;33m=:>\033[0;32m ");
	fflush(stdout);
	GetStr(buf,8,ECHO_DOTS);
	if(!strcmp(buf,userinfo.pass)) {
		printf("\n");
	} else {
		if(i > 2) {  
			printf("\nCLICK..\n\n");
			fflush(stdout);
			sleep(1);
			exit(0);
		} else {
			printf("\nWrong Password..\n\n");
			fflush(stdout);
			sleep(3);
			goto askpass_top;
		}
	}
	sleep(1);
	return;
}

/*****************/
/* logout()      */
/* save her info */
/*****************/

void logout(void) {
	time_t t;
	char obuf[384];
	
	time(&t);
	userstate.time_total += (t - userstate.time_laston);
	update_user_state();
	sprintf(obuf,"\x1b[0;34mlogging out \x1b[33m[\x1b[35m%s\x1b[33m] (\x1b[34m%lds\x1b[33m).\n\x1b[0m", username, t-userstate.time_laston);
	broadcast(obuf);
	listact(0);
	whois_off();
	syslog(LOG_INFO,"%s logged out.",username);

	printf("\n");
	CatFileNoPause("logout");
	printf("  \033[1;34m(= t\033[1;35mHe \033[1;34msH\033[1;35macK! \033[1;34mbB\033[1;35mS \033[1;34m=) ");
	printf("\033[0;1;34m[\033[0;35mct=%ds\033[1;34m]\033[0m\n", t-userstate.time_laston);
	printf("\033[1;34m%s, Thankyou for Logging in!\033[0m\n\n",username);

	fflush(stdout);
	sleep(3);
	exit(0);
}


/************************************/
/* whois_on() writes a file to tell */
/* whois online list who's on       */
/************************************/
void whois_on(int doing) {
	FILE *fp;
	char obuf[256];
	
	/* doing:
	 * 0> Main Menu
	 * 1> Reading Msg
	 * 2> Textfiles
	 * 3> Hunt Cabal
	 * 4> Configure
	 * 5> Posting
	 * 6> Yelling
	 * 7> Voting	 
	 */
	sprintf(obuf,"%s/data/who/%s",BBSDIR,username);
	if((fp = fopen(obuf,"wb")) == NULL) return;
	fprintf(fp,"%d", doing);
	fclose(fp);
	return;
}

/*************************************/
/* whois_off() deletes th file so    */
/* user will not be shown in whoison */
/*************************************/
void whois_off(void) {	
	char obuf[256];
	sprintf(obuf,"%s/data/who/%s",BBSDIR,username);
	unlink(obuf);
	return;
}

/*************************************/
/* get_user_state()                  */
/*                                   */
/* gets user state file into memory. */
/* returns 0 on success, -1,-2 fail  */
/*************************************/

int get_user_state() {
	FILE *fp;
	char obuf[256];
	
	sprintf(obuf,"%s/users/%s/state",BBSDIR,username);
	if((fp = fopen(obuf,"r")) == NULL) return -1;
	if(fread((char *)&userstate,sizeof(USERSTATE),1,fp) != 1) return -2;
	fclose(fp);
	return 0;
}

/*************************************/
/* get_user_info()                   */
/*                                   */
/* gets user information into memory */
/*************************************/

int get_user_info() {
	FILE *fp;
	char obuf[256];
	
	sprintf(obuf,"%s/users/%s/info",BBSDIR,username);
	if((fp = fopen(obuf,"r")) == NULL) return -1;
	if(fread((char *)&userinfo,sizeof(USERINFO),1,fp) != 1) return -2;
	fclose(fp);
	return 0;
}

/*************************/
/* get_user_name         */
/* just returns username */
/*************************/

char *get_user_name() {
	static char buf[31];
	strcpy(buf,username);
	return (char *)buf;
}

/******************************************************/
/* MakeDropFile()                                     */
/* creates a dorinfo1.def file in the users directory */
/******************************************************/

int MakeDropFile(void) {
	FILE *fp;
	char obuf[256];
	
	syslog(LOG_INFO,"Creating a DORINFO1 file.");	
	sprintf(obuf,"%s/users/%s/dorinfo1.def",BBSDIR,username);
	if((fp = fopen(obuf,"w")) == NULL) return -1;
	fprintf(fp,"TOGA.CX\n");
	fprintf(fp,"KREATOR\n");
	fprintf(fp,"\n");
	fprintf(fp,"COM1\n");
	fprintf(fp,"38400 BAUD-R,N,8,1\n");
	fprintf(fp,"0\n");
	strcpy(obuf,username);
	toupperString(obuf);
	fprintf(fp,"%s\n",obuf);
	fprintf(fp,"\n");
	fprintf(fp,"THE INTERNET\n");
	fprintf(fp,"1\n");
	fprintf(fp,"10\n");
	fprintf(fp,"90\n");
	fclose(fp);
	return 0;
}

/******************************************/
/* CheckKeyCode()                         */
/* checks to see if user is not validated */
/******************************************/

int CheckKeyCode() {
	FILE *fp;
	char obuf[256];
	int i=0;
	sprintf(obuf,"%s/users/%s/keycode",BBSDIR,username);
	if((fp = fopen(obuf,"r")) == NULL) {		return 0;
	} else {
		if(fread((char *)&i,sizeof(int),1,fp) != 1) return 0;
	}
		
	fclose(fp);
	return i;
}



/******************************/
/* updates the user info file */
/******************************/

int update_user_info(void) {
	FILE *fp;
	char obuf[256];
	
	sprintf(obuf,"%s/users/%s/info",BBSDIR,username);
	if((fp = fopen(obuf,"wb")) == NULL) return -1;
	
	if(fwrite((char *)&userinfo,sizeof(USERINFO),1,fp) != 1) return -2;
	fclose(fp);	
	return 0;
}


/********************************/
/* updates the users state file */
/********************************/

int update_user_state() {
	FILE *fp;
	char obuf[256];
	
	sprintf(obuf,"%s/users/%s/state",BBSDIR,username);
	if((fp = fopen(obuf,"wb")) == NULL) return -1;
	if(fwrite((char *)&userstate,sizeof(USERSTATE),1,fp) != 1) return -2;
	fclose(fp);
	return 0;	
}

/*******************************/
/* update_user()               */
/*                             */
/* this function makes it comp-*/
/* atable with the old code    */
/*******************************/

int update_user(void) {
	int i;
	i = update_user_state();
	return i;
}

/*****************************/
/* simple function to return */
/* total connection time     */
/*****************************/

long TotalConnTime() {
	return (time(NULL)-userstate.time_laston);
}

/********************************/
/* UserToProfile()			    */
/*  allows the user to profile  */
/*  another user..              */
/********************************/

void UserToProfile(void) {
	char buf[200];
	FILE *fp;
	USERSTATE udata;
	USERINFO uinfo;

	printf("\033[1;32mUseR To pRofiLe");

	if(last_user_buffer[0] == '\0') {
		printf("\033[1;34m: ");
	} else {
		printf(" \033[1;34m(\033[1;31m%s\033[1;34m): ",last_user_buffer);
	}

	GetStrName(buf,30);
	if(buf[0] == '\0' && last_user_buffer[0] == '\0') {
		printf("\n");
		return;
	} 

	if(!strcmp(buf,"List")) {
		syslog(LOG_INFO,"%s is reading the userlist!",username);
		printf("\033[0m\n");
		UserList2(MEMBERS);
		return;
	}

	if(buf[0] != '\0') {
		strcpy(last_user_buffer,buf);
	}
	
	if(existuser(last_user_buffer) != TRUE) {
		printf("\n\033[1;35mNo such user as, \033[31m%s \033[35mon this bbs!\n\n",last_user_buffer);
		last_user_buffer[0] = '\0';
		return;
	}

	sprintf(buf,"%s/users/%s/info",BBSDIR,last_user_buffer);
	if((fp = fopen(buf,"rb")) == NULL) {	
		printf("\nCan not open users info file\n\n");
		return;
	}

	if(fread((char *)&uinfo,sizeof(USERINFO),1,fp) != 1) {
		printf("\nCan not read users info file\n\n");
		fclose(fp);
		return;
	}
	
	fclose(fp);

	sprintf(buf,"%s/users/%s/state",BBSDIR,last_user_buffer);
	if((fp = fopen(buf,"rb")) == NULL) {	
		printf("\nCan not open users state file\n\n");
		return;
	}

	if(fread((char *)&udata,sizeof(USERSTATE),1,fp) != 1) {
		printf("\nCan not read users state file\n\n");
		fclose(fp);
		return;
	}

	fclose(fp);

	syslog(LOG_INFO,"%s is Profiling (%s)",username,buf);
	printf("\n\033[1;35m[\033[31m%s\033[35m] (\033[34m",last_user_buffer);
	switch(uinfo.sex) {
		case 'M': case 'm': printf("male"); break;
		case 'F': case 'f': printf("female"); break;
		default:  printf("unknown"); break;
	}
	printf("\033[35m)\nLast online \033[31m");
	printago(udata.time_laston);
	printf(" ago.\n");
	printf("\033[1;35mPosted \033[31m%ld \033[35mmsgs, made \033[31m%ld \033[35mcalls.\n",udata.no_posts,udata.no_calls);
	printf(" \033[0;34m=._.-+-._.-=-._.-+-._.-=-._.-+-._.-=-._.-+-._.=\n");

	sprintf(buf,"%s/users/%s/user_profile",BBSDIR,last_user_buffer);
	printf("\033[0;33m");
	if(CatFileDirect(buf) == -1)
		printf("%s hasn't written a profile yet!\n\n",last_user_buffer);
	else
		printf("\n");
	return;
}

void CheckProfile(void) {
	FILE *fp;
	char buf[200];
	char ch = FALSE;
	struct stat st;

	if(userstate.no_calls > 3) {
		sprintf(buf,"%s/users/%s/user_profile",BBSDIR,get_user_name());
		if(stat(buf,&st) == -1) ch = TRUE;
		if(st.st_size <= 3) ch = TRUE;
	}

	if(ch == TRUE) {
		printf("\033[1;35mYou do not have a profile.\n");
		printf("\033[0;35mWould you like to create one \033[1;34m[\033[1;31mYes\033[1;34m]\033[0;35m? ");	
		for(;;) {
			ch = tolower(GetKey(0));
			if(ch=='y'||ch=='\n'||ch=='n') break;
		}
		switch(ch) {
		case 'y':		
		case '\n':
			syslog(LOG_INFO,"%s elects Yes for AutoAskForProfile",username);
			printf("\033[1;31mYes!\n\n");
			printf("\033[1;31mEnter your user profile here.\n");
			printf("(\033[0;31m7 lines max\033[1;31m).\n");
			printf("\033[1;33mColor codes are ok!\n");
			printf("\033[1;34mStart typing here:\033[0;34m\n");
			file_editor(buf,8);
			break;
		case 'n':
		default:
			syslog(LOG_INFO,"%s elects No for AutoAskForProfile",username);
			printf("\033[1;31mNo..\n\n");
			break;
		}
	}
	return;
}

/************************************************************/
/*                                                          */
/*              N E W   U S E R   S I G N U P               */
/*                                                          */
/************************************************************/

void new_user_signup() {

	char ibuf[100];
	char *obuf;
	int i;

	
	/******************************/
	/* print new user info...     */
	/* get the name of the caller */
	/******************************/

	sleep(1);
	CatFileNoPause("NEWUSER");
	
	for(;;) {
		printf("\033[0m\n\033[1;33;40mChoose your handle \033[0;31m[\033[1;32mlogin name\x1b[0;31m].\n=:>\x1b[0;33m ");
		GetStrName(username,30);
		if(strlen(username) <= 2) {
			printf("\nName too short! (allows 3-8 characters)\n");
			printf("type 'off' or 'bye' to leave the system.\n");
			printf("Please try again.\n");
			continue;		
		} else if(!strcmp(username,"Bye")) {
			printf("\nLogging off... bye!\n\n");
			exit(0);
		} else if(!strcmp(username,"New")) {
			printf("\n\033[1;35mThe name '\033[1;31mNew\033[1;35m' is reserved.\n");
			continue;
		} else if(!strcmp(username,"Everybody")) {
			printf("\n\033[1;35mThe name '\033[1;31mEverybody\033[1;35m' is reserved.\n");
			continue;
		} else if(!strcmp(username,"List")) {
			printf("\n\033[1;35mThe name `\033[1;31mList\033[1;35m' is reserved.\n");
			continue;
		} else if(!strcmp(username,"All")) {
			printf("\n\033[1;35mThe name `\033[1;31mAll\033[1;35m' is reserved.\n");
			continue;
		} else if(!strcmp(username,"Off")) {
			printf("\nLoffing off... bye!\n\n");
			exit(0);
		} else if(existuser(username) == TRUE) {
			printf("\n\x1b[0;35m[\x1b[1;33m%s\x1b[0;35m] is already taken! \x1b[32mtry again.\n", username);
			continue;
		} else if(bad_name(username) == TRUE) {
			printf("\n\x1b[1;33m Hahaha, try again %s!!!\n", username);
			continue;
		}
		printf("\n\x1b[0;35mYour handle is \x1b[34m\"\x1b[36m%s\x1b[34m\" \x1b[35mis this right \x1b[34m[\x1b[31mYES\x1b[34m]? ", username);
		for(;;) {
			i = GetKey(0);
			if((toupper(i) == 'Y') || (toupper(i) == 'N')) break;
		}
		if(toupper(i) == 'Y') { printf("Yes..\n"); break; }
		if(toupper(i) == 'N') { printf("No..\n"); continue; }

	}
	sprintf(ibuf,"%s/users/%s",BBSDIR,username);
	umask(0);
	i = mkdir(ibuf,0755);
	
	/************************/
	/* Ask for users gender */
	/************************/
		
	printf("\n\x1b[0;33mYou are: [\x1b[31mM\x1b[33m]ale or [\x1b[31mF\x1b[33m]emale? ");
	for(;;) {
		i = GetKey(0);
		if((toupper(i) == 'M') || (toupper(i) == 'F')) break;
	}
	userinfo.sex = toupper(i);
	if(toupper(i) == 'M') printf("Male");
	if(toupper(i) == 'F') printf("Female");
		
	/*************************/
	/* Ask for email address */
	/* used for validation.  */
	/*************************/

	GetEmailVerify();

	/**********************/
	/* no more real names */
	/**********************/

	strcpy(userinfo.realname,"norealname");
	
	/*******************************/
	/* User must choose a password */
	/*******************************/
	
	printf("\x1b[0m");
	printf("\x1b[36mPassword\x1b[35m\n");
	printf("~~~~~~~~\n");
	printf(" Please choose a password for your account.\n");
	printf(" Dots will echo to the screen\x1b[0;35m.\n\n");

	for(;;) {
		printf("Choose a password =:> ");
		GetStr(ibuf,8,ECHO_DOTS);
		printf("\nRe-type to verify =:> ");
		GetStr(userinfo.pass,8,ECHO_DOTS);
		if(!strcmp(ibuf,userinfo.pass)) {
			break;
		} else {
			printf("\nPassword doesn't match, try again!\n");
			continue;
		}
	}
	
	/*
	 * what to do next:
	 * -----------------
	 * get a password, 
	 * make homedir, 
	 * make state and info files,
	 * make a keycode.
	 */		

	printf("\n\nDamnIt Jim!\n");
	fflush(stdout);
	sleep(1);
	

	i = mkuser_info();
	if(i != 0) exit(0);
	printf("\n\x1b[0mInfo file saved...\n");
	fflush(stdout);
	sleep(1);

	i = mkuser_state();
	if(i != 0) exit(0);
	printf("state file saved.\n");


	printf("\033[1;35mYAY!!  You got Mail!");
	sleep(2);
	fflush(stdout);
	printf("  YAAAY!\n");
	fflush(stdout);
	sleep(2);

	logact('n');
	
	PrintSlow("\n\033[1;36mAn Email's been sent to your address.\n");
	PrintSlow("Your validation key is in that.\n");
	PrintSlow("Grab it and enter it at the new user menu.\n\n");
	PrintSlow("\033[1;31mSignin Completed! (goodluck!)\n\n");
	syslog(LOG_INFO,"NewUser SignUp for % Completed.",username);
	Pause2();
	printf("\n");
	return;
}

int bad_name(const char *name) {
	char ibuf[32],tbuf[300];
	FILE *fp;
	int x=FALSE;
	
	sprintf(tbuf,"%s/data/BadNames",BBSDIR);
	fp = fopen(tbuf,"rt");
	if(fp == NULL) return FALSE;
	
	sprintf(tbuf,"%s\n",name);
	tolowerString(tbuf);
	while(fgets(ibuf,31,fp) != NULL) {
		tolowerString(ibuf);
		if(!strcmp(ibuf,tbuf)) {
			syslog(LOG_INFO,"user used the 'BadName' (%s).",username);
			x=TRUE;
		}
	}
	fclose(fp);
	return x;
}

int is_room_maker(const char *name) {
	char ibuf[32],tbuf[300];
	FILE *fp;
	int x=FALSE;
	
	sprintf(tbuf,"%s/data/RoomMakers",BBSDIR);
	fp = fopen(tbuf,"rt");
	if(fp == NULL) return FALSE;
	
	sprintf(tbuf,"%s\n",name);
	tolowerString(tbuf);
	while(fgets(ibuf,31,fp) != NULL) {
		tolowerString(ibuf);
		if(!strcmp(ibuf,tbuf)) x=TRUE;
	}
	fclose(fp);
	return x;
}

int is_lastlisthidden(const char *name) {
	char ibuf[32],tbuf[300];
	FILE *fp;
	int x=FALSE;
	
	sprintf(tbuf,"%s/data/HideFromLastList",BBSDIR);
	fp = fopen(tbuf,"rt");
	if(fp == NULL) return FALSE;
	
	sprintf(tbuf,"%s\n",name);
	tolowerString(tbuf);
	while(fgets(ibuf,31,fp) != NULL) {
		tolowerString(ibuf);
		if(!strcmp(ibuf,tbuf)) {
			syslog(LOG_INFO,"%s is HideFromLastList.",username);
			x=TRUE;
		}
	}
	fclose(fp);
	return x;
}



int is_sysop(const char *name) {
	char ibuf[32],tbuf[300];
	FILE *fp;
	int x=FALSE;
	
	sprintf(tbuf,"%s/data/Sysops",BBSDIR);
	fp = fopen(tbuf,"rt");
	if(fp == NULL) return FALSE;
	
	sprintf(tbuf,"%s\n",name);
	tolowerString(tbuf);
	while(fgets(ibuf,31,fp) != NULL) {
		tolowerString(ibuf);
		if(!strcmp(ibuf,tbuf)) {
			syslog(LOG_INFO,"%s is a SysOp!",username);
			x=TRUE;
		}
	}
	fclose(fp);
	return x;
}


int is_twit(const char *name) {
	char ibuf[200];
	FILE *fp;
	
	sprintf(ibuf,"%s/users/%s/twit",BBSDIR,name);
	fp = fopen(ibuf,"r");
	if(fp != NULL) {
		syslog(LOG_INFO,"%s %s",
			get_user_name(),
			"is a Twit.!!");
		printf("\n\033[1;35mYou are a twit!\n");
		fclose(fp);
		return TRUE;
	} else {
		return FALSE;
	}
	return FALSE;
}


/******************************/
/* existuser()                */
/* returns TRUE if does exist */
/* returns FALSE if doesn't   */
/******************************/

int existuser(char *name) {
	int x;
	char buf[256];
	DIR *dp;
	struct stat *st;
	
	sprintf(buf,"%s/users/%s",BBSDIR,name);
        //if(stat(buf,st) != 0) x = FALSE; else x = TRUE;

	dp = opendir(buf);
	if(dp != NULL) {
		x = TRUE;
		closedir(dp);
	} else {
		x = FALSE;
	}
 
	//if(errno != ENOENT) x = TRUE; else x = FALSE;
	//closedir(dp);
	return x;
}

int mkuser_keycode(void) {
	FILE *fp;
	int i;
	char obuf[128];
	srand(time(NULL));
	
	sprintf(obuf,"%s/users/%s/newuser.msg",BBSDIR,username);
	sleep(1);
	unlink(obuf);
	if((fp = fopen(obuf,"wt")) == NULL) return -1;

	i = (rand() % 8999) + 1000;

	fprintf(fp, "Dear %s,\n\n",username);
	fprintf(fp, "Your key code for Velvet.Ath.cx is %d\n", i);
	fprintf(fp, "Thanks for logging in.\n\n");
	fprintf(fp, "The Sysops!\n\n");
	fclose(fp);
	
	sprintf(obuf,"%s/users/%s/keycode",BBSDIR,username);
	unlink(obuf);
	sleep(1);
	if((fp = fopen(obuf,"wb")) == NULL) return -1;
	if(fwrite((char *)&i,sizeof(int),1,fp) != 1) return -2;
	fclose(fp);	
	
	return 0;
}


int mkuser_state(void) {
	int i;
	char obuf[128];
	FILE *fp;
	
	syslog(LOG_NOTICE,"mkuser_state() started");		
	userstate.time_laston = time(NULL);
	userstate.time_total = 0;
	userstate.time_posting = 0;
	
	userstate.no_posts = 0;
	userstate.no_calls = 0;
	userstate.no_drops = 0;
	
	/*****************************************************/
	/* when expanding number of rooms, this also needs   */
	/* to be changed from 32 to the max number of rooms  */
	/* in user file					     */
	/*****************************************************/
	
	for(i=0;i<32;i++) userstate.lastmsg[i]=0;
	
	sprintf(obuf,"%s/users/%s/state",BBSDIR,username);
	if((fp = fopen(obuf,"wb")) == NULL) {
		syslog(LOG_INFO,"failed mkuser_state: fopen");
		printf("\nfailed mkuser_state\n");
		fflush(stdout);
		sleep(3);
		return -1;
	}
	if(fwrite((char *)&userstate,sizeof(USERSTATE),1,fp) != 1) {
		syslog(LOG_INFO,"failed mkuser_state: fwrite");
		printf("\nfailed mkuser_state\n");
		fflush(stdout);
		sleep(3);
		return -1;
	}
	fclose(fp);	

	return 0;
}

int mkuser_info(void) {
	FILE *fp;
	char obuf[512];
	int i;
	
	for(i=0;i<128;i++) userinfo.ans[i]=0;	
	sprintf(obuf,"%s/users/%s/info",BBSDIR,username);
	printf("\n\nWhatup? this is mkuser_info()\n\n");
	fflush(stdout);
	sleep(3);
	if((fp = fopen(obuf,"wb")) == NULL) {
		syslog(LOG_NOTICE,"failed mkuser_info: fopen");
		printf("\nfailed mkuser_info");
		fflush(stdout);
		sleep(3);
		return -1;
	}

	printf("\n\nWe obviously got to OPEN the file! (;\n\n");
	fflush(stdout);
	sleep(3);
	if(fwrite((char *)&userinfo,sizeof(USERINFO),1,fp) != 1) {
		syslog(LOG_NOTICE,"failed mkuser_info: fwrite");
		printf("\nfailed mkuser_info\n");
		fflush(stdout);
		sleep(3);
		return -1;
	}
	printf("\n\nThis is the end of mkuser_state.. before closeing...\n\n");
	fflush(stdout);
	sleep(3);
	fclose(fp);	
	printf("\n\nbyebye from mkuser_state :)\n\n");
	fflush(stdout);
	sleep(3);
	return 0;

}


/******************************************/
/* this function gets users email address */
/* creates a new keycode                  */
/* then sends the email address a message */
/******************************************/

void GetEmailVerify(void) {

	char *obuf;
	int i;

	syslog(LOG_INFO,"%s %s",username,"editing email");	
	printf("\n\n");
	printf(" \x1b[0;32m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\n");
	printf(" \x1b[1;35mPlease type your \x1b[33minternet eMail\x1b[35m address\n");
	printf(" tHe sHacK will eMail you, for validation.\n");
	printf(" This address is kept \x1b[0;35mstrictly private\x1b[1;35m,\n");
	printf(" and will not be sold or given to anyone.\x1b[1;33m\n");
	printf("           -----)-- \x1b[0;31mtHe sHacK \x1b[1;33m(velvet.ath.cx)\n");
	printf(" \x1b[0;32m=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=\x1b[0m\n");	

	for(;;) {
		printf("\n\x1b[0;35mEnter a valid \x1b[36me-Mail\x1b[35m address?\n\x1b[1;33m=:>\x1b[0;36m ");

		/**********************************************/
		/* here we should add a better email checking */
		/**********************************************/
		GetStr(userinfo.email,255,NO_SPACE|NO_FORSLASH|NO_BAKSLASH|
			NO_DASHFIRST);
		tolowerString(userinfo.email);
		if(strlen(userinfo.email) <= 5) {
			printf("\nEmail not valid try again.\n");
			continue;
		} 
		
		printf("\n\x1b[0;34m\"\x1b[36m%s\x1b[34m\", \x1b[35mis this correct \x1b[34m[\x1b[31mYES\x1b[34m]? ", userinfo.email);
		for(;;) {
			i = GetKey(0);
			if((toupper(i) == 'Y') || (toupper(i) == 'N')) break;
		}

		if(toupper(i) == 'Y') { printf("Yes..\n"); break; }
		if(toupper(i) == 'N') { printf("No..\n"); continue; }
	}

	printf("\nPlease wait..");
	fflush(stdout);
	i = mkuser_keycode();
	if(i != 0) {
		printf("\n\x1b[0mOperation Failed.\n\n");
		syslog(LOG_NOTICE,"%s","could not write keycode file.");
		return;
	} else {
		syslog(LOG_INFO,"%s %s",username,"wrote keycode file.");
	}
	printf("\n\x1b[0mkeycode saved.\n");

	/* Note:
	 * here we might want the mail
	 * to also have a subject line.
	 */		
	obuf = calloc(500,sizeof(char));
	sprintf(obuf, "/usr/bin/mail -s \"tHe sHacK!\" %s < \"%s/users/%s/newuser.msg\"", userinfo.email,BBSDIR,username);
	i = system(obuf);
	syslog(LOG_INFO,"%s %s %s",username,"mailed keycode to:",userinfo.email);
	free(obuf);
	if(i != 0) exit(0);
	FakeMail();
	printf("\x1b[0meMail Sent.\n\n");
	return;
}

void ChangePassword() {
	/************************/
	/* change user password */
	/************************/
	char ibuf[10];
		
	printf("\x1b[0m\n");
	printf("\x1b[36mPassword\x1b[35m\n");
	printf("~~~~~~~~\n");
	printf(" Please choose a new password for your account.\n");
	printf(" Dots will echo to the screen\x1b[0;35m.\n\n");

	for(;;) {
		printf("Choose a password: ");
		GetStr(ibuf,8,ECHO_DOTS);
		printf("Re-type to verify: ");
		GetStr(userinfo.pass,8,ECHO_DOTS);
		if(!strcmp(ibuf,userinfo.pass)) {
			syslog(LOG_INFO,"%s %s",username,"changed password");
			break;
		} else {
			printf("\nPassword doesn't match, try again!\n");
			continue;
		}
	}
	update_user_info();
	return;
}


/* 12-17-04 ktr
   search_users_ip() just does searching
   of an ipaddress in the list at username/ipRecord
   returns 0 if nothing is found, 
   returns 1 if something is found. */

int search_users_ip(char *ipaddress) {
	FILE *fp;
	char buf[2048];
	int yi;
	sprintf(buf,"%s/users/%s/ipRecord",BBSDIR,username);
	if((fp = fopen(buf,"r")) != NULL) {
		if((fgets(buf,2048,fp)) != NULL) {
			buf[strlen(buf)-1] = '\0';
			if(!strncmp(buf,ipaddress,2048)) { fclose(fp); return 1; }
		}
		fclose(fp);
	}
	return 0;			
}

int record_users_ip(char *ipaddress) {
	FILE *fp;
	char buf[2048];
	sprintf(buf,"%s/users/%s/ipRecord",BBSDIR,username);
	if((fp = fopen(buf,"a")) != NULL) {
		fprintf(fp,"%s\n",ipaddress);
		fclose(fp);
		syslog(LOG_INFO,"New ip Record added for (%s).",username);
	} else return 1;
	return 0;
}

/****************************************/
/****************************************/
/* The Who's Online List                */
/* (Simple)                             */
/****************************************/
/****************************************/


void whois_on_list(void) {
        /* doing:
         * 0> Main Menu  
         * 1> Reading Msg
         * 2> Textfiles 
         * 3> Hunt Cabal
         * 4> Configure
         * 5> Posting
         * 6> Yelling
         * 7> Voting
         */
	DIR *dp;
	FILE *fp;
	char obuf[256];
	struct dirent *dirinfo;
	int doing;

	syslog(LOG_INFO,"%s views WhosOnline.",username);	
	printf("\033[0;30;44mUser's Online       \033[0m \033[0;30;41mDoing     \033[0m\n");
	sprintf(obuf,"%s/data/who/",BBSDIR);
	dp = opendir(obuf);	
	while((dirinfo = readdir(dp)) != NULL) {
		if(dirinfo->d_name[0] == '.') continue;
		sprintf(obuf,"%s/data/who/%s",BBSDIR,dirinfo->d_name);
		if((fp = fopen(obuf,"r")) == NULL) continue;
		fscanf(fp,"%d", &doing);
		printf("\033[1;33m%-20.20s ", dirinfo->d_name);
		switch(doing) {
		case 0:
			printf("Main Menu");
			break;
		case 1:
			printf("Reading Msgs");
			break;
		case 2:
			printf("G-files");
			break;
		case 3:
			printf("Games");
			break;
		case 4:
			printf("Config Acct");
			break;
		case 5:
			printf("Posting");
			break;
		case 6:
			printf("Chat Room");
			break;
		case 7:
			printf("Voting");
			break;
		default:
			printf("Other");
			break;
		}
		printf("\n");
		fclose(fp);
	}
	closedir(dp);
	return;
}

