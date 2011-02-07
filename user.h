/*****************************************************/
/* 2 main user structs: 5 types of files. (at home)  */
/*****************************************************/
/* USERSTATE: describes the ever changing user data  */
/*            this info is always updated by bbs.    */
/* USERINFO:  describes info the user has inputted   */
/*            and the user has the option to update. */
/*                                                   */
/* There are 3 other file type(s).                   */     
/* a> keycode - which gives a new user a validation  */
/*              method via email.                    */
/* b> plan    - this describes the users profile.    */
/* c> *.msg   - any email the user has.              */
/*****************************************************/

#define MEMBERS		1
#define NON_MEMBERS	2


typedef struct {

	/*************/
	/* time data */
	/*************/
	
	unsigned long time_laston;
	unsigned long time_total;
	unsigned long time_posting;
	
	/***************/
	/* number data */
	/***************/
	
	unsigned int no_posts;
	unsigned int no_calls;
	unsigned int no_drops;
		
	/*****************/
	/* last msg data */
	/*****************/
	
	unsigned long lastmsg[32];
	
} USERSTATE;

typedef struct {

	/*******************************/
	/* the userdirectory itself is */
	/* the name of the user...     */
	/*******************************/

	char pass[11];		/* password 10 chars, one for NULL */
	
	char realname[31];	/* realname 30 chars, one for NULL */
	char email[256];	/* email address 255 chars, and NULL */
	char sex;		/* 1 char for gender type (male/female) */
	
	/*********************************/
	/* ans - multiple choice answers */
	/*********************************/
	
	char ans[128];

	/***********************************/
	/* first time/date the user called */
	/* this should never been modified */
	/***********************************/
	
	unsigned long time_firston;
	
} USERINFO;

/****************************************/
/* this file continues the structure of */
/* the character file for each user in  */
/* the game.                            */
/****************************************/

typedef struct {
	char shipname[31];
	char killedby[31];

	long exp;
	long trading_credits;
	long bank_credits;

	int shipclass;
	int fighters;
	int mines;
	int clocking;
	int becons;
	
	int holds;
	int holds_of_iron;
	int holds_of_alcohol;
	int holds_of_hardware;
	
	int currently_clocked;
	unsigned long time_lastin;
	
} CHARACTER;

/*************************************************/
/* this is the global once the username is found */
/*************************************************/

char username[31];

/*****************/
/* other globals */
/*****************/
USERSTATE userstate;
USERINFO userinfo;
CHARACTER character;

/* EOF */
