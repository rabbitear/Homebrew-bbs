#include <ctype.h>
#include "shack.h"
#include "user.h"
#include <dirent.h>

#define Up 1;
#define Down -1;

static int Sort_Direction = Up;

typedef unsigned long int uli;

typedef struct userdata {
	char name[64];
	unsigned long time_laston;
	unsigned int no_calls;
	unsigned int no_posts;
	unsigned int no_drops;
	unsigned long time_posting;
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



void PrintListHeader(void);


void CreateList(Header *hdr);
int  AddFirst(Header *hdr, Data *s);
int  AddLast(Header *hdr, Data *s);
int  AddItem(Header *hdr, Data *s);
int  GetItem(Header hdr, Data *s);
void ResetList(Header *hdr);
int  PrevItem(Header *hdr);
int  NextItem(Header *hdr);
int  FirstItem(Header *hdr);
int  LastItem(Header *hdr);
uli  ItemCount(Header hdr);
int  KillItem(Header *hdr);
void PrintList(Header hdr, FILE *fptr);
void ClearList(Header *hdr);
void Merge(Header *ha, Header *hb, Header *ret);
void Sort(Header *hdr);
void SortUp(void);
void SortDown(void);
void RemoveDups(Header *hdr);


/****************************/
/* this list is out of date */
/****************************/
/* for the new userlist I'll*/
/* have to use opendir() and*/
/* readdir() and closedir() */
/****************************/

void PrintListHeader(void) {
	printf("\x1b[0m");
	printf("                 \x1b[0;35m days \x1b[32mNo. \x1b[31mNo.   \x1b[34mSecs\n");
	printf("                 \x1b[0;35msince \x1b[32m of \x1b[31m of   \x1b[34mspent\n");
	printf("\x1b[0mUser Name:       \x1b[0;35mlogin \x1b[32mcal \x1b[31mpst \x1b[33mD \x1b[34mposts\n");
	printf("\x1b[0;1;30m~~~~ ~~~~~       ~~~~~ ~~~ ~~~ ~ ~~~~~\n");
	return;
}

void UserList2(int mode) {

	DIR *dp;	
	FILE *statefp;
	char fname[256];
	USERSTATE udata;
	int i,x=0;
	struct dirent *dirinfo;
	struct stat st;	
	Data userdat;
	Header h;
	
	CreateList(&h);

	printf("\x1b[0mRetrieving List..");	
	fflush(stdout);
	sprintf(fname,"%s/users/",BBSDIR);
	dp = opendir(fname);
	
	while((dirinfo = readdir(dp)) != NULL) {
		if(dirinfo->d_name[0] == '.') continue;		
		sprintf(fname,"%s/users/%s/state",BBSDIR,dirinfo->d_name);
		if((statefp = fopen(fname,"rb")) != NULL) {
			if(fread((char *)&udata,sizeof(USERSTATE),1,statefp) == 1) {
				strcpy(userdat.name,dirinfo->d_name);
				userdat.time_laston = udata.time_laston;
				userdat.no_calls = udata.no_calls;
				userdat.no_posts = udata.no_posts;
				userdat.no_drops = udata.no_drops;
				userdat.time_posting = udata.time_posting;
				if(mode == NON_MEMBERS) {
					sprintf(fname,"%s/users/%s/keycode",BBSDIR,dirinfo->d_name);
					if(!stat(fname,&st)) {/** if keycode **/
						AddItem(&h, &userdat);
						x++;
					}
				} else {
					sprintf(fname,"%s/users/%s/keycode",BBSDIR,dirinfo->d_name);
					if(stat(fname,&st)) {/** if no keycode **/
						AddItem(&h, &userdat);
						x++;
					}
				}
			}
			fclose(statefp);
		}
	}
	closedir(dp);
	printf("\n\033[0;35mCounted \033[1;35m%d \033[0;35mUser(s)\n\n",x);
	Pause2();
	printf("\n");
	Sort(&h);
	ResetList(&h);	
	PrintListHeader();
	i=0;
	while(NextItem(&h)) {
		if(i>(rows-6)) {
			fflush(stdout);
			if(!More2()) {
				printf("\n");
				return;
			}
			printf("\n");
			i=0;
			PrintListHeader();
		}
		GetItem(h,&userdat);
		sprintf(fname,"%s/users/%s/user_profile",BBSDIR,userdat.name);
		if(stat(fname,&st))	
			printf("\x1b[0m%-17.17s", userdat.name);
		else
			printf("\x1b[1;33m%-17.17s", userdat.name);
		printf("\x1b[0;35m%5.1f", ((time(NULL) - userdat.time_laston )/86400.0));
		printf("\x1b[32m%4d", userdat.no_calls);
		printf("\x1b[31m%4d", userdat.no_posts);
		printf("\x1b[33m%2d ", userdat.no_drops);
		printf("\x1b[34m%d\n", userdat.time_posting);
		i++;
		fflush(stdout);
	}
	printf("\n");
	ClearList(&h);
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

void KillElement(Header *hdr, List *item)
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


void DebugPrint(Header H)
{
   List *current;

   printf("Head:%p\n",H.head);
   printf("Tail:%p\n",H.tail);
   printf("Current:%p\n",H.current);
   printf("No.:%i\n\n",H.length);

   current = H.head;

   while (current != NULL)  {
      printf("Node :%s\n",current->data);
      printf("Addr :%p\n",current);
      printf("Prev :%p\n",current->prev);
      printf("Next :%p\n\n",current->next);
      current = current->next;
   }
}


int CreateNode(List *new, Data *s)
{
   if (!(new->data = (Data *) malloc (sizeof(struct userdata))))  {
      free(new);
      return(FALSE);
   }

   memcpy(new->data, s,sizeof(struct userdata));
   return(TRUE);
}


/* Public Functions */

void CreateList(Header *hdr)
{
   hdr->head = (List *) NULL;
   hdr->tail = (List *) NULL;
   hdr->current = (List *) NULL;
   hdr->length = 0;
}


int  AddFirst(Header *hdr, Data *s)
{
   List *new;

   if (!(new = (List *) malloc (sizeof(List))))
      return(FALSE);

   new->prev = (List *) NULL;

   if (CreateNode(new, s))  {
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


int AddLast(Header *hdr, Data *s)
{
   List *new;

   if (!(new = (List *) malloc (sizeof(List))))
      return(FALSE);

   new->next = (List *) NULL;

   if (CreateNode(new, s))  {
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


int  AddItem(Header *hdr, Data *s)
{
   List *new;

   if (hdr->current == NULL)
      return(AddLast(hdr,s));

   if (!(new = (List *) malloc (sizeof(List))))
      return(FALSE);

   if (CreateNode(new, s))  {
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


int  GetItem(Header hdr, Data *s)
{
   if (hdr.current == NULL)  {
      return (FALSE);
   } else {
      memcpy(s, hdr.current->data,sizeof(struct userdata));
      return (TRUE);
   }
}


void ResetList(Header *hdr)
{
   hdr->current = NULL;
}


int  PrevItem(Header *hdr)
{
   if ((hdr->current == NULL) || (hdr->current == hdr->head))  {
      return (FALSE);
   } else {
      hdr->current = hdr->current->prev;
      return (TRUE);
   }
}


int  NextItem(Header *hdr)
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


int FirstItem(Header *hdr)
{
   if (hdr->head == NULL)  {
      return (FALSE);
   } else {
      hdr->current = hdr->head;
      return (TRUE);
   }
}


int LastItem(Header *hdr)
{
   if (hdr->head == NULL)  {
      return (FALSE);
   } else {
      hdr->current = hdr->tail;
      return (TRUE);
   }
}


uli ItemCount(Header hdr)
{
   return (hdr.length);
}


int KillItem(Header *hdr)
{
   List *t;

   if (hdr->current == NULL) {
      return (FALSE);
   } else {
      t = hdr->current->prev;
      KillElement(hdr, hdr->current);
      hdr->current = t;
      return (TRUE);
   }
}


void PrintList(Header hdr, FILE *fptr)
{
   List *current;

   current = hdr.head;

   while (current != NULL)  {
      fprintf(fptr,"%s\n",current->data);
      current = current->next;
   }
}


void ClearList(Header *hdr)
{
   List *current, *temp;

   current = hdr->head;

   while (current != NULL)  {
      free(current->data);
      temp = current->next;
      free(current);
      current = temp;
   }
   CreateList(hdr);
}


void Merge(Header *ha, Header *hb, Header *ret)
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

      if (a->data->time_posting > b->data->time_posting) {
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
   CreateList(ha);
   CreateList(hb);
}


void Sort(Header *hdr)
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
         Merge(&th1, &th2, &th3);
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


void SortUp(void)
{
    Sort_Direction = Up;
}


void SortDown(void)
{
    Sort_Direction = Down;
}


void RemoveDups(Header *hdr)
{
   List *current, *temp;

   current = hdr->head;

   while (current->next != NULL)  {
      temp = current->next;
      if (strcmp(current->data, current->next->data) == 0)
         KillElement(hdr, current);
      current = temp;
   }
}



