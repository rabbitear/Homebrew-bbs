/**************************************/
/* this header contains the structure */
/* of the circluar message file.      */
/**************************************/

#define MAXMESSAGES 50

/*******************************/
/* struct for the main msgfile */
/*******************************/
typedef struct {
	long number;
	int  noshow;
	char uhandle[31];
	char uto[31];
	long unumber;
	char subject[80];
	long date;
	char text[8000];
} MSGBODY;

/**************************************/
/* proto's for message base functions */
/* everything but the editor          */
/**************************************/

int WriteNewMsg(MSGBODY,const int);
void PrintMsg(const MSGBODY, const long,const int);
int CatMsg(const long, const long,const int);
int StatMsgs(long lohi[2],const int);
char *GetMsgAuthor(const long,const int);
int MoveMsg(const long, const long,const int);
void DeleteMsg(int, const int);
/************************/
/* -=- End of msg.h -=- */
/************************/
