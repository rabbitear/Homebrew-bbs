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
