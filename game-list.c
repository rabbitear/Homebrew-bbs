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
#include "game.h"
#include <dirent.h>
#include <ctype.h>

#define Up 1;
#define Down -1;

static int Sort_Direction = Up;

typedef unsigned long int uli;

typedef struct playerdata {
	char name[31];
	char shipbrand[64];
	unsigned long credit_chips;
	unsigned long score;
	unsigned long time_lastin;
	int alingment;
} Data;

typedef struct listitem {
	struct listitem *next;
	struct listitem *prev;
	Data *data;
} List;

typedef struct listheader {
	List *head;
	List *tail;
	List *current;
	uli length;
} Header;


void PrintPLHeader(void);


void GCreateList(Header *hdr);
int  GAddFirst(Header *hdr, Data *s);
int  GAddLast(Header *hdr, Data *s);
int  GAddItem(Header *hdr, Data *s);
int  GGetItem(Header hdr, Data *s);
void GResetList(Header *hdr);
int  GPrevItem(Header *hdr);
int  GNextItem(Header *hdr);
int  GFirstItem(Header *hdr);
int  GLastItem(Header *hdr);
uli  GItemCount(Header hdr);
int  KillItem(Header *hdr);
void GPrintList(Header hdr, FILE *fptr);
void GClearList(Header *hdr);
void GMerge(Header *ha, Header *hb, Header *ret);
void GSort(Header *hdr);
void GSortUp(void);
void GSortDown(void);
void GRemoveDups(Header *hdr);


/*********************************************************/
/*********************************************************/
/********* Ranking and Score Board Player List ***********/
/*********************************************************/
/*********************************************************/


/****************************/
/* this list is out of date */
/****************************/
/* for the new userlist I'll*/
/* have to use opendir() and*/
/* readdir() and closedir() */
/****************************/

void PrintPLHeader(void) {
	printf("\x1b[0m");
	printf("\033[1;35;44mPlayer Name:\033[0;40m     \033[1;37;44m DSLP\033[0;40m \033[1;35;44mShip Brand And Class\033[0;40m \033[1;37;44mCreditChip\033[0;40m \033[1;33;41m  Score!  \033[0;40m \033[1;37;44mAl\033[0;40m\n");
	fflush(stdout);
	return;
}

void PlayerList(void) {

	char fname[256];
	int x=0;
	struct dirent *dirinfo;
	struct stat st;
		
	DIR *dp;	
	FILE *statefp;
	PLAYER playerfiledat;
	Data playerdat;
	Header h;
	
	GCreateList(&h);
	fflush(stdout);
	sprintf(fname,"%s/users/",BBSDIR);
	dp = opendir(fname);
	
	while((dirinfo = readdir(dp)) != NULL) {
		if(dirinfo->d_name[0] == '.') continue;		
		sprintf(fname,"%s/users/%s/player_state",BBSDIR,dirinfo->d_name);
		if((statefp = fopen(fname,"rb")) != NULL) {
			if(fread((char *)&playerfiledat,sizeof(PLAYER),1,statefp) == 1) {
				if(exists_player_state()) {
					strcpy(playerdat.name,dirinfo->d_name);
					strcpy(playerdat.shipbrand,playerfiledat.shipbrand);
					playerdat.time_lastin  = playerfiledat.time_lastin;
					playerdat.credit_chips = playerfiledat.credit_chips;
					playerdat.score        = playerfiledat.score;
					playerdat.alingment    = playerfiledat.alingment;
					GAddItem(&h, &playerdat);
					x++;
				}
			}
			fclose(statefp);
		}
	}
	closedir(dp);
	GSort(&h);
	GResetList(&h);	
	PrintPLHeader();
	while(GNextItem(&h)) {
		if(x>(rows-3)) {
			if(!More2()) return;
			printf("\n");
			fflush(stdout);
			x=0;
			PrintPLHeader();
		}
		GGetItem(h,&playerdat);
		printf("\x1b[0;35m%-17.17s", playerdat.name);
		printf("\x1b[0;32m%5.1f ", ((time(NULL) - playerdat.time_lastin )/86400.0));
		printf("\033[0;33m%-20.20s ", playerdat.shipbrand);
		printf("\x1b[0;32m%10d ", playerdat.credit_chips);
		printf("\x1b[1;31m%10d ", playerdat.score);
		printf("\x1b[0;32m%d\n", playerdat.alingment);
		x++;
		fflush(stdout);
	}
	printf("\n");
	Pause3();
	GClearList(&h);
	return;
}




/*****************************************/
/*****************************************/
/*****************************************/
/*************  List Stuff  **************/
/*****************************************/
/*****************************************/
/*****************************************/
/*****************************************/

/* Private Functions */

void GKillElement(Header *hdr, List *item)
{
   List *temp;

   if (hdr->current == item)
      hdr->current = NULL;

   free(item->data);

   if (item != hdr->head)  {
      item->prev->next = item->next;
   } else {
      hdr->head = item->next;
      if (hdr->head != NULL)
         hdr->head->prev = (List *) NULL;
   }

   if (item != hdr->tail)  {
      item->next->prev = item->prev;
   } else {
      hdr->tail = item->prev;
      if (hdr->tail != NULL)
         hdr->tail->next = (List *) NULL;
   }

   free(item);
   hdr->length--;
}



int GCreateNode(List *new, Data *s)
{
   if (!(new->data = (Data *) malloc (sizeof(struct playerdata))))  {
      free(new);
      return(FALSE);
   }

   memcpy(new->data, s,sizeof(struct playerdata));
   return(TRUE);
}


/* Public Functions */

void GCreateList(Header *hdr)
{
   hdr->head = (List *) NULL;
   hdr->tail = (List *) NULL;
   hdr->current = (List *) NULL;
   hdr->length = 0;
}


int  GAddFirst(Header *hdr, Data *s)
{
   List *new;

   if (!(new = (List *) malloc (sizeof(List))))
      return(FALSE);

   new->prev = (List *) NULL;

   if (GCreateNode(new, s))  {
      if (hdr->head == NULL)  {
         hdr->tail = new;
         new->next = (List *) NULL;
      } else {
         new->next = hdr->head;
         hdr->head->prev = new;
      }
      hdr->head = new;
      hdr->length++;
      hdr->current = new;
      return(TRUE);
   } else {
      return(FALSE);
   }
}


int GAddLast(Header *hdr, Data *s)
{
   List *new;

   if (!(new = (List *) malloc (sizeof(List))))
      return(FALSE);

   new->next = (List *) NULL;

   if (GCreateNode(new, s))  {
      if (hdr->head == NULL)  {
         hdr->head = new;
         new->prev = (List *) NULL;
      } else {
         new->prev = hdr->tail;
         hdr->tail->next = new;
      }
      hdr->tail = new;
      hdr->length++;
      hdr->current = new;
      return(TRUE);
   } else {
      return(FALSE);
   }
}


int  GAddItem(Header *hdr, Data *s)
{
   List *new;

   if (hdr->current == NULL)
      return(GAddLast(hdr,s));

   if (!(new = (List *) malloc (sizeof(List))))
      return(FALSE);

   if (GCreateNode(new, s))  {
      new->prev = hdr->current;
      new->next = hdr->current->next;
      hdr->current->next = new;

      if (new->next == NULL) {
         hdr->tail = new;
      } else {
         new->next->prev = new;
      }

      hdr->length++;
      hdr->current = new;
      return(TRUE);
   } else {
      return(FALSE);
   }
}


int  GGetItem(Header hdr, Data *s)
{
   if (hdr.current == NULL)  {
      return (FALSE);
   } else {
      memcpy(s, hdr.current->data,sizeof(struct playerdata));
      return (TRUE);
   }
}


void GResetList(Header *hdr)
{
   hdr->current = NULL;
}


int  GPrevItem(Header *hdr)
{
   if ((hdr->current == NULL) || (hdr->current == hdr->head))  {
      return (FALSE);
   } else {
      hdr->current = hdr->current->prev;
      return (TRUE);
   }
}


int  GNextItem(Header *hdr)
{
   if (hdr->current == hdr->tail)  {
      return (FALSE);
   } else {
      if (hdr->current == NULL) {
         hdr->current = hdr->head;
      } else {
         hdr->current = hdr->current->next;
      }
      return (TRUE);
   }
}


int GFirstItem(Header *hdr)
{
   if (hdr->head == NULL)  {
      return (FALSE);
   } else {
      hdr->current = hdr->head;
      return (TRUE);
   }
}


int GLastItem(Header *hdr)
{
   if (hdr->head == NULL)  {
      return (FALSE);
   } else {
      hdr->current = hdr->tail;
      return (TRUE);
   }
}


uli GItemCount(Header hdr)
{
   return (hdr.length);
}


int GKillItem(Header *hdr)
{
   List *t;

   if (hdr->current == NULL) {
      return (FALSE);
   } else {
      t = hdr->current->prev;
      GKillElement(hdr, hdr->current);
      hdr->current = t;
      return (TRUE);
   }
}


void GPrintList(Header hdr, FILE *fptr)
{
   List *current;

   current = hdr.head;

   while (current != NULL)  {
      fprintf(fptr,"%s\n",current->data);
      current = current->next;
   }
}


void GClearList(Header *hdr)
{
   List *current, *temp;

   current = hdr->head;

   while (current != NULL)  {
      free(current->data);
      temp = current->next;
      free(current);
      current = temp;
   }
   GCreateList(hdr);
}


void GMerge(Header *ha, Header *hb, Header *ret)
{
   List *a, *b, *c;
   List z;

   ret->length = ha->length + hb->length;

   if ((ha->head == NULL) && (hb->head == NULL)) {
      ret->head = NULL;
      ret->tail = NULL;
      return;
   }

   if (ha->head == NULL) {
      ret->head = hb->head;
      ret->tail = hb->tail;
      return;
   }

   if (hb->head == NULL) {
      ret->head = ha->head;
      ret->tail = ha->tail;
      return;
   }

   a = ha->head;
   b = hb->head;
   z.next = NULL;
   c = &z;

   do {

      //old line below..
      //if ((Sort_Direction * strcmp(a->data, b->data))<=0)  {

      if (a->data->score > b->data->score) {
         c->next = a;
         c->next->prev = c;
         c = a;
         a = a->next;
      } else {
         c->next = b;
         c->next->prev = c;
         c = b;
         b = b->next;
      }
   } while (a!= NULL && b!=NULL);

   if (a == NULL) {
      c->next = b;
      ret->tail = hb->tail;
   } else {
      c->next = a;
      ret->tail = ha->tail;
   }
   c->next->prev = c;
   ret->head = z.next;
   ret->head->prev = NULL;
   GCreateList(ha);
   GCreateList(hb);
}


void GSort(Header *hdr)
{
   List *a, *b, *c, head, *todo, *t;
   Header th1, th2, th3;
   unsigned long int i, N;

   if (hdr->head == NULL)
      return;

   c = hdr->head;
   N = 1;
   head.next = c;

   th1.length=0;
   th2.length=0;

   do {
      todo = head.next;
      c = &head;
      do {
         t = todo;
         a = t;
         for (i=1; i<=N-1; i++)  {
            if (t != NULL)
               t = t->next;
         }
         if (t == NULL)  {
            b = NULL;
         } else {
            b = t->next;
            t->next = NULL;
         }
         th1.tail = t;
         t = b;
         for (i=1; i<=N-1; i++)  {
            if (t != NULL)
               t = t->next;
         }
         if (t != NULL)  {
            todo = t->next;
            t->next = NULL;
         } else {
            todo = NULL;
         }
         th2.tail = t;
         th1.head = a;
         th2.head = b;
         GMerge(&th1, &th2, &th3);
         if (c != NULL)  {
            c->next = th3.head;
            if (c->next != NULL)
               c->next->prev = c;
            c = th3.tail;
         }
      } while (todo != NULL);
      N = N + N;
   } while (a != head.next);
   hdr->head = head.next;
   hdr->head->prev = NULL;
   t = hdr->head;
   while (t->next)
      t = t->next;
   hdr->tail = t;
}


void GSortUp(void)
{
    Sort_Direction = Up;
}


void GSortDown(void)
{
    Sort_Direction = Down;
}


void GRemoveDups(Header *hdr)
{
   List *current, *temp;

   current = hdr->head;

   while (current->next != NULL)  {
      temp = current->next;
      if (strcmp(current->data, current->next->data) == 0)
         GKillElement(hdr, current);
      current = temp;
   }
}



