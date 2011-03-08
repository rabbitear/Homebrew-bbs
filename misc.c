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


/******************************************/
/* printago()                             */
/* print the time difference up until now */
/******************************************/

void printago(long cmpTime) {
	time_t currentTime, diffTime;
	float newTime;
	
	time(&currentTime);
	diffTime = currentTime - cmpTime;

	if(diffTime >= 86400) {
		newTime = diffTime / 86400.0;
		printf("%.1f days", newTime);
	} else if(diffTime >= 3600) {
		newTime = diffTime / 3600.0;
		printf("%.1f hours", newTime);
	} else if(diffTime >= 60) {
		newTime = diffTime / 60.0;
		printf("%.1f minutes", newTime);
	} else printf("%ld seconds", diffTime);
	return;
}


/*************************************************/
/* CatFileDirect()                               */
/* Print an entire file from exact filepath name */
/* Does page pausing and more prompts.           */
/*************************************************/

int CatFileDirect(const char *filename) {
	FILE *fp;
	char ibuf[128];
	int x=0;
	
	fflush(stdout);
	if((fp = fopen(filename,"rt")) == NULL) {
		return -1;
	}
	for(;;) {
		if(x <= (rows-2)) {
			if(fgets(ibuf, 127, fp) != NULL) {
				printf("%s",ibuf);
				x++;
			} else {
				fclose(fp);
				return 0;
			}
		} else {
			if(!More2()) {
				fclose(fp);
				return 0;
			} else {
				x=0;
			}
		}
	}
	fclose(fp);
}

int CatFileDirect2(const char *filename) {
	FILE *fp;
	char ibuf[128];
	int x=0;
	
	fflush(stdout);
	if((fp = fopen(filename,"rt")) == NULL) {
		return -1;
	}
	for(;;) {
		if(x <= (rows-2)) {
			if(fgets(ibuf, 127, fp) != NULL) {
				printf("%s",ibuf);
				x++;
			} else {
				fclose(fp);
				return 0;
			}
		} else {
			if(!More3()) {
				fclose(fp);
				return 0;
			} else {
				x=0;
			}
		}
	}
	fclose(fp);
}

/*******************************/
/* CatFileNoPause()            */
/* Print entire file path name */
/* without pausing for more.   */
/*******************************/

int CatFileNoPause(const char *filename) {
	FILE *fp;
	char obuf[200], ibuf[128];
	char ch;

	fflush(stdout);	
	sprintf(obuf,"%s/text/%s",BBSDIR,filename);
	if((fp = fopen(obuf, "rt")) == NULL) {
		return -1;
	}
	while(fgets(ibuf, 127, fp) != NULL) {
		printf("%s", ibuf);
	}
	
	fflush(stdout);
	fclose(fp);
	return 0;
}



/*******************************************/
/* tolowerString()                         */
/* toupperString()                         */
/* Convert a string to lower or upper case */
/* turn a character string to lower case   */
/*******************************************/
void tolowerString(char *s) {
	for( ; ((*s != 0) || isspace(*s)) ; s++ ) *s = tolower(*s);
	return;
}

void toupperString(char *s) {
	for( ; ((*s != 0) || isspace(*s)); s++ ) *s = toupper(*s);
	return;
}


/*******************************************/
/* strisdigit()                            */
/* Test to see if the STRING is all digits */
/*******************************************/

int strisdigit(char *s) {
	for( ; ((*s != 0) || isspace(*s)) ; s++ ) if(!isdigit(*s)) return FALSE;
	return TRUE;
}


/*******************************************************************/
/*                                                                 */
/*   D I C T I O N A R Y  -  O N L I N E  -  F O R  -  W O R D S   */
/*                                                                 */
/*******************************************************************/

void DictWord() {
	char ibuf[31];
	char tbuf[200];
	printf("\x1b[0mWord to define: ");
	GetStr(ibuf, 30,0);

	if(ibuf[0] == '\0') {
		printf("\nCancelled!\n\n");
		return;
	}
	printf("\n\033[0;32mPlease Wait...");
	fflush(stdout);
	sleep(1);
	syslog(LOG_INFO,"%s %s %s",
		get_user_name(),
		"is looking up the word",
		ibuf);
	sprintf(tbuf,"dict %s > \"%s/users/%s/tmp.dict.txt\"",
		ibuf,
		BBSDIR,
		get_user_name());
	system(tbuf);
	sprintf(tbuf,"%s/users/%s/tmp.dict.txt",
		BBSDIR,
		get_user_name());
	CatFileDirect(tbuf);
	printf("\n");
	fflush(stdout);
	return;	
}


/***************************/
/* Pauses and More Prompts */
/***************************/

void Pause(void) {	
	int ch;
	printf("\x1b[0;1;31m[\x1b[34mPaUSed\x1b[31m] \x1b[32mhit the SPACEBAR\x1b[1;31m: \x1b[0m");
	for(;;) {
		ch = toupper(GetKey(0));
		if(ch == ' ' || ch == 'C') break;
	}
	printf("\n");
	fflush(stdout);
	return;
}

void Pause2(void) {
	int ch;
	printf("\x1b[0;1;31m[\x1b[34mPaUSed\x1b[31m] \x1b[0mhit the \x1b[1;34mSPACEBAR\x1b[1;31m:\x1b[0m ");
	for(;;) {
		ch = toupper(GetKey(0));
		if(ch == ' ' || ch == 'C') break;
	}
	printf("\n");
	fflush(stdout);
	return;
}

void Pause3(void) {
	int ch;
	printf("\033[0;1;34;40m<\033[0;34m(\033[31mMORE\033[34m)\033[1;34m>\033[1;33m: \033[0m");
	for(;;) {
		ch = toupper(GetKey(0));
		if(ch == ' ' || ch == 'C'|| ch == '\n') break;
	}
	printf("\n");
	fflush(stdout);
	return;
}
	
void PauseNoColor(void) {
	int ch;
	printf("\x1b[0m[PaUSed] hit the SPACEBAR: ");
	for(;;) {
		ch = toupper(GetKey(0));
		if(ch == ' ' || ch == 'C') break;
	}
	printf("\n");
	fflush(stdout);
	return;
}

int More(void) {
	int ch;
	
	for(;;) {
		ch = GetKeyPrompt("\x1b[1;34mMore\x1b[0;36m [\x1b[35mYES\x1b[36m]? \x1b[32m");
		if(tolower(ch) == 'y') {printf("Yes..\n"); return TRUE;}
		else if(ch == '\n') {printf("More!\n"); return TRUE;}
		else if(ch == ' ') {printf("More..\n"); return TRUE;}
		else if(tolower(ch) == 'n') {printf("No..\n"); return FALSE;}
		else if(tolower(ch) == 'q') {printf("No, Quit!\n"); return FALSE;}
		else if(tolower(ch) == 's') {printf("No, Stop!\n"); return FALSE;}
		else printf("\n\x1b[0;33m '\x1b[1;33my\x1b[0;33m' = Yes, GO.\n '\x1b[1;33mn\x1b[0;33m' = No, STOP!\x1b[0m\n");
	}
	printf("\x1b[0;36m");
	return TRUE;
}

int More2(void) {
	int ch;
	
	for(;;) { 
		ch = GetKeyPrompt("\x1b[1;34mMore\x1b[0;36m [\x1b[35mYES\x1b[36m]? \x1b[32m");
		if(tolower(ch)=='y'||ch=='\n'||ch==' ') {
			printf("\033[12D\033[K\033[0;32m");		
			return TRUE;
		} else if(tolower(ch) == 'n'||tolower(ch) == 'q'||tolower(ch) == 's') {
			printf("\033[12D\033[K\033[0;32m");		
			return FALSE;
		} else {
			printf("\n\x1b[0;33m '\x1b[1;33my\x1b[0;33m' = Yes, GO.\n '\x1b[1;33mn\x1b[0;33m' = No, STOP!\x1b[0m\n");
		}
		printf("\033[12D\033[K\033[0;32m");		
	}
	printf("\x1b[0;36m");
	return TRUE;
}

int More3(void) {
	int ch;
	
	for(;;) { 
		ch = GetKeyPrompt("\x1b[1;31mMo\033[0;31mre \033[0;32m[\x1b[1;33mYES\x1b[0;32m]\033[1;32m? \x1b[0m");
		if(tolower(ch)=='y'||ch=='\n'||ch==' ') {
			printf("\033[12D\033[K\033[0m");		
			return TRUE;
		} else if(tolower(ch) == 'n'||tolower(ch) == 'q'||tolower(ch) == 's') {
			printf("\033[12D\033[K\033[0m");		
			return FALSE;
		} else {
			printf("\n\x1b[0;33m '\x1b[1;33my\x1b[0;33m' = Yes, GO.\n '\x1b[1;33mn\x1b[0;33m' = No, STOP!\x1b[0m\n");
		}
		printf("\033[12D\033[K\033[0m");		
	}
	printf("\x1b[0m");
	return TRUE;
}



int TailFile(const char *filename, int ln) {

	FILE *fp;
	int c;

	if(ln <= 0) return 0;

	fp = fopen(filename,"r");
	if(fp == NULL) {
		return 1;
	}
	if(fseek(fp,0,SEEK_END) != 0) { /* if the seek to end fails for some reason */
		fclose(fp);
		return 1;
	}
	if(ftell(fp) <= 0) { /* file must be bigger then 0 bytes */
		fclose(fp);
		return 1;
	}

	if(fgetc(fp) == '\n') ln--;

	while(fseek(fp,(long)-1,SEEK_CUR) == 0) {

		if(ftell(fp) <= 0) {
			ln = 0;
			break; 
		}
		
		if(fgetc(fp) == '\n') ln--;

		if(ln < 0) break;
		
		if(fseek(fp,(long)-1,SEEK_CUR) != 0) break;
	}
	while(!feof(fp)) {
		if((c = fgetc(fp)) == EOF) break;
		putc(c,stdout);
	}
	fclose(fp);
	return 0;

	
}




void PrintSlow(const char *s) {
	int i = 0;
	
	while(s[i] != '\0') {
		//usleep(27027);
		usleep(25027);
		putc(s[i],stdout);
		fflush(stdout);
		i++;
	}
}


/*** EOF ***/                                                                                      
