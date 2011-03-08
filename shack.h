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

/************/
/* includes */
/************/
#include <stdio.h>
#include <errno.h>
#include <time.h>
#include <stdlib.h>
#include <termios.h>
#include <string.h>
#include <syslog.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#define TRUE   1
#define FALSE  0

/***********************************************************/
/*   You should change these items to match your system    */
/***********************************************************/

#define BBSDIR 		"/home/bbs"
#define CHATFILE	"/home/bbs/data/CHATFILE"
#define MESSAGEFILE	"/home/bbs/msgs/generalforum"
#define SYSOPNAME	"Kreator"
#define VERSION		"0.1a"
#define CCHATF		"/home/bbs/data/CCHATF"
#define IDLETIMEOUT     9000
/***********************************************************/
/* thats all the sysop configuration you should have to do */
/***********************************************************/
#define ECHO_DOTS	1
#define NO_SPACE	2
#define NO_ATSIGN	4
#define NO_FORSLASH	8
#define NO_BAKSLASH	16
#define YES_CTRLX	32
#define YES_ESCKEY	64
#define NO_DASH		128
#define NO_DASHFIRST	256

/**********/
/* protos */
/**********/

//main.c
void logout();

//menu.c
void MainMenu(const char *);
int ValidateMenu(int);
void ConfigureMenu(const char *);
void PhrackMenu();

//gfiles.c
void GFilesMenu();
int GFilesPrompt(int,FILE *);

//user.c
int login(void);
void AskPass(void);
void logout(void);
int get_user_state();
int get_user_info();
int update_user();
int update_user_state();
char *get_user_name();
int CheckKeyCode();
void new_user_signup();
int existuser(char *);
int mkuser_state(void);
int mkuser_info(void);
void ChangePassword();
int update_user_info(void);
void GetEmailVerify(void);
void whois_on(int);
void whois_off(void);
void whois_on_list(void);
int is_room_maker(const char *);
void UserToProfile(void);
void CheckProfile(void);
void UserList2(int);
int is_lastlisthidden(const char *);
int search_users_ip(char *ipaddress);
int record_users_ip(char *ipaddress);


//msgs.c
char *GetRoomName();
int SelectRoom();

//misc.c
int CatFileNoPause(const char *);
void printago(long);
void DistWord();
void tolowerString(char *);
void toupperString(char *); 
void Pause();
int More();
int More2(void);
int More3(void);
void PrintSlow(const char *);
int TailFile(const char *, int);

//chat.c
void resetchatfp();
int newchatfp();
void readchatfp();
void broadcast(const char *);
void Yell();
int GetKeyPrompt(const char *);
int GetKey(int);
void GetStr(char *,int,int);
void GetStrName(char *, int);
int GetKeyArrows(void);
int ChatRoomPrompt(void);

//logs.c
void putlog(const char *);
void PutLogNoName(const char *);

//files.c
void FileView(void);

//last.c
void logact(char);
void listact(int);

//wumpus.c
int Wumpus(void);

//game.c
void StartGame();
int Poker(void);
void display_sector();

//editor
void file_editor(const char *,const int);

//sysop.c
int SysopMenu(void);

//editor2.c
void editor2(const char *, const char *, long);
void test_the_list(void);

int cols;
int rows;

char last_user_buffer[32];

/* EOF */
