#include "shack.h"
#include "user.h"
#include "msg.h"
#include "rooms.h"

typedef struct line_of_text_def {
	struct line_of_text_def *next;
	struct line_of_text_def *prev;
	char linebuf[256];
	unsigned long linenumber;
} lot_data;

typedef struct lot_header_def {
	lot_data *head;
	lot_data *tail;
	lot_data *current;
	unsigned long length;
} lot_header;


/***************************/
/* proto's for this editor */
/* mainly list functions   */
/***************************/
void create_textlist(lot_header *);
void free_textlist(lot_header *);
int topappend_lot(lot_header *, const char *);
int insert_lot(lot_header *, const char *);
void goto_lot(lot_header *, int);
void print_lot(lot_header *);

void draw_editor_header(void);
char *get_who_to(char *);

void editor2(const char *whoto, const char *subj, long reNo) {
	int    msgNo,x=0,y=0,last_y=0,ch,done=FALSE,menu=TRUE;
	char   obuf[200], ibuf[200], zbuf[80], nbuf[32], ch2[2];
	struct tm *tp;
	time_t end_time=0,   // end time
	       start_time=0; // start time
	FILE            *sp;
	MSGBODY         *msg;
	lot_header	hdr;

	/** Initialize our character list **/
	nbuf[0] = '\0';
	create_textlist(&hdr);
	time(&start_time);
	strncpy(nbuf,(char *)get_who_to(whoto),30);
	if(!strcmp(nbuf,"New")) return; /* forgot what this does */
	if(rNum <= 0 && (is_sysop(get_user_name()) == FALSE)) {
		CatFileDirect("/home/bbs/text/no_post_in_1st_room");
		return;
	}
	if((rNum == 0) && (strcmp(nbuf,"Everybody"))) { 
		printf("\n\033[1;35mSysOp only message base!\n");
		printf("\n\033[1;35mHit '\033[1;31mSHIFT-P\033[1;35m' to post here.\n");
		return;
	}
	if( (rNum != 1) && (is_twit(get_user_name())) ) {
		return;
	}
	if(subj == NULL) {
		printf("\n\033[0;32mSu\033[1;33mbject: \033[1;32m");
		GetStr(zbuf,25,0);
	} else {
		if(subj[0] != 'R') {
			sprintf(ibuf,"R\033[0;32mE No.\033[1;36m%ld:\033[1;32m %s",reNo,subj);	
			strcpy(zbuf,ibuf);
		} else {
			strcpy(zbuf,subj);
		}
	}
	draw_editor_header();
	yeahagoto:
	ibuf[0] = '\0';
	printf("\x1b[0;32m\n");  //start the input prompt
	for(;;) {
		ch = GetKeyArrows();
		/************************************/
		/* [ESC] (ascii 27) for arrow keys  */
		/************************************/
		if(ch > 255) continue;
		/*******************************/
		/* If they press the ENTER key */
		/*******************************/
		else if(ch == '\n') {
			if(!x) {
				break;
			} else {
				printf("\n");
				strcat(ibuf,"\n");
				insert_lot(&hdr,ibuf);	
				x=0;
				ibuf[0] = '\0';
				/** here we should have a reformat **/
				continue;
			}
		} else if( ((ch=='\b')||(ch=='\x7f')) && (!x)) continue;
		else if((ch=='\b')||(ch=='\x7f')) {
			if(ibuf[strlen(ibuf)-1] == 'm'
			&& strlen(ibuf) >= 7
			&& ibuf[strlen(ibuf)-7] == '\033') {
				ibuf[strlen(ibuf)-7] = '\0';
			} else {
				printf("\b \b\033[P");
				x--;
				ibuf[strlen(ibuf)-1] = '\0';
			}
			continue;
		} 

		/** Start of Color Codes **/
		else if(ch=='^') {
			char chstr[8];
			int cha;
			chstr[0] = '\0';
			printf("^");
			cha = GetKeyArrows();
			if(cha == 'R') sprintf(chstr,"\033[1;31m");
			
			else if(cha == 'D') sprintf(chstr,"\033[0;33m");
			else if(cha == 'Z') sprintf(chstr,"\033[1;30m");

			else if(cha == 'G') sprintf(chstr,"\033[1;32m");
			else if(cha == 'Y') sprintf(chstr,"\033[1;33m");
			else if(cha == 'B') sprintf(chstr,"\033[1;34m");
			else if(cha == 'M') sprintf(chstr,"\033[1;35m");
			else if(cha == 'C') sprintf(chstr,"\033[1;36m");
			else if(cha == 'W') sprintf(chstr,"\033[1;37m");
			
			else if(cha == 'r') sprintf(chstr,"\033[0;31m");
			else if(cha == 'g') sprintf(chstr,"\033[0;32m");
			else if(cha == 'y') sprintf(chstr,"\033[0;33m");
			else if(cha == 'b') sprintf(chstr,"\033[0;34m");
			else if(cha == 'm') sprintf(chstr,"\033[0;35m");
			else if(cha == 'c') sprintf(chstr,"\033[0;36m");
			else if(cha == 'w') sprintf(chstr,"\033[0;37m");
			else if(cha == '^') sprintf(chstr,"^");
			else { printf("\b \b"); continue; }
			
			if((chstr[0] != '\0') && (chstr[0] != '^')) {
				strcat(ibuf,chstr);
				strcat(ibuf,"\0");
				printf("%s",chstr);
			}
			
			printf("\b \b");
			if(chstr[0] != '^') continue;		
		}
		/** End of Color Codes **/

		else if(isprint(ch)) {
			char cstr[2];
			cstr[0] = ch;
			cstr[1] = '\0';
			strcat(ibuf,cstr);
			if((x>=40)&&(strchr(ibuf,' '))) {
				char *p;
				p=&ibuf[strlen(ibuf)-1];
				while(!isspace(*p)) {
					printf("\b \b");
					p--;
				}
				*p='\0';
				p++;
				sprintf(obuf,"%s\n",ibuf);
				insert_lot(&hdr,obuf);
				strcpy(ibuf,p);
				x=strlen(ibuf);
				printf("\n%s",ibuf);
			} else {
				x++;
				printf("%c",ch);
			}
			continue;
		}
	}

	done = FALSE;
	menu = TRUE;

		/* close down msg text */
	if(hdr.length < 1) {
		printf("Nothing Written!\n");
		free_textlist(&hdr);
		return;
	}
	time(&end_time);
	while(!done) {
		if(menu) printf("\x1b[0;35m(\033[1;31mH\033[0;35m)elp, Save? \x1b[34m[\x1b[1;31mYES\x1b[0;34m=:> ");
		ch = GetKey(0);
		switch(ch) {
		case 'n':
		case 'N':
			printf("\x1b[0mNo! (Aborting)\n"); 
			free_textlist(&hdr);
			return; 
			break;
		case 'y':
		case 'j':
			printf("\033[0mYes..\n");
			done = TRUE;
			break;
		case 'c':
		case 'C':
			printf("\033[0mContinue..\n");
			printf("\033[1;35mContinue after line [\033[1;31mLAST\033[1;35m]? \033[0m");
			GetStr(ibuf,3,0);
			if(ibuf[0] == '\n' || ibuf[0] == '\0') {
				goto_lot(&hdr,hdr.length);
				ch = hdr.length;
				done = TRUE;
				menu = FALSE;
			} else {
				ch = atoi(ibuf);
				if(ch < 0 || ch > hdr.length) {
					printf("Illegal line number!\n");
					done = FALSE;
					menu = TRUE;
					break;
				}
			}
			if(ibuf[0] != '\0' || ibuf[0] != '\n') goto_lot(&hdr,ch);
			printf("\n\033[1;31mContinue on line #\033[1;35m%d.\033[0m",ch+1);
			done = TRUE;
			menu = FALSE;
			goto yeahagoto;
			break;
		case 'd':
		case 'D':
			printf("\033[1;33mDelete a Line!\n");
			printf("\033[1;35mLine Number To Delete? \033[1;31m");			
			GetStr(ibuf,3,0);
			if(ibuf[0] == '\n' || ibuf[0] == '\0') {
				printf("\033[0;33mNothing deleted!\n");
				done = FALSE;
				menu = TRUE;
				break;
			}
			ch = atoi(ibuf);
				if(ch < 1 || ch > hdr.length) {
				printf("\033[1;33mIllegal line number!\n");
				done = FALSE;
				menu = TRUE;
				break;
			}
			if(delete_lot(&hdr,ch))
				printf("\033[1;33mLine #\033[0;33m%d \033[1;33mdeleted!\n",ch);
			else
				printf("\033[1;33mOperation Failed!\n");
			done = FALSE;
			menu = TRUE;
			break;

		case 'h':
		case 'H':
		case '?':
			CatFileNoPause("ED-HELP");
			done = FALSE;
			menu = TRUE;
			break;
		case 'l':
		case 'L':
			printf("List..\n\n");
			print_lot(&hdr);
			done = FALSE;
			menu = TRUE;
			break;
		default:
			done = FALSE;
			menu = FALSE;
			break;
		}
	}
	
	printf("\n\033[0;36mThis is your ");
	printf("\033[1m%ld \033[0;36mpost.", userstate.no_posts);
	printf("\n\033[0;33mTime spend writting\033[0m: ");
	printf("%ld \033[36mseconds.", end_time - start_time);

	/***********************************/
	/* This is how to save the message */
	/* ------------------------------- */
	/* a better solution would be to   */
	/* remake the MSGBODY struct to fit*/
	/* with the link list, but I would */
	/* almost have to re-write the --- */
	/* entire message base ...         */
	/* ------------------------------- */
	/* so for now we'll just strcat it */
	/***********************************/
	
	msg = calloc(1,sizeof(MSGBODY));
	msg->date = start_time;
	msg->unumber = 0;	
	strcpy(msg->uhandle, get_user_name());
	if(zbuf[0] == '\0')
		strcpy(msg->subject,"None..");
	else
		strcpy(msg->subject,zbuf);
	strcpy(msg->uto,nbuf);

	hdr.current = hdr.head;
	if(hdr.current == NULL) return;
	while(hdr.current != hdr.tail) {
		strcat(msg->text,hdr.current->linebuf);
		hdr.current = hdr.current->next;
		if(hdr.current->next == NULL) break;
	}
	if(hdr.tail != NULL)
		strcat(msg->text,hdr.tail->linebuf);
	free_textlist(&hdr);
	
	/* Calculate the number of bytes in the text */

	printf("\n\033[33mBytes used for text\033[0m: ");
	printf("%ld \033[36mcharacters.", strlen(msg->text));
	printf("\n\033[1;32mUpdating msg index..");

	if(rNum != 0) {
		sprintf(obuf,"%s/users/%s/signature",BBSDIR,get_user_name());
		if((sp = fopen(obuf,"rt")) != NULL) {
	 		while(fgets(ibuf, 199, sp) != NULL) {
	               		strcat(msg->text,ibuf);
				strcat(msg->text,"\0");
			}
			fclose(sp);
			printf("\033[0;35mT\033[1magging \033[0;35mS\033[1mig");
		}
	}
	
	userstate.time_posting += (end_time - start_time);
	userstate.no_posts++;
	update_user_state();
	if(rNum != 1) { 
		if(!WriteNewMsg(*msg,rNum)) {
			printf(".\x1b[32m.done!");
			sprintf(obuf,"\033[1;35m[\033[1;31m%s\033[1;35m] Posted a Message.\033[0m\n",get_user_name());
		        broadcast(obuf);
		} else printf(".\x1b[1;33m.(\x1b[0;31mERROR\x1b[1;33m) NOT SAVED!");
	} else {
		if(!WriteNewMail(*msg,msg->uto)) {
			printf(".\x1b[32m.done!");
			sprintf(obuf,"\033[1;35m[\033[1;31m%s\033[1;35m] Wrote Mail.\033[0m\n",get_user_name());
			broadcast(obuf);
		} else printf("ERROR, NOT SAVED!");
	}
					
	printf("\n\x1b[0m");
	free(msg);
	
	return;	/* write_msg function all done */
}

/******************************************/
/* misc. support functions for the editor */
/******************************************/

void draw_editor_header() {
	printf("\x1b[0m");

	printf("[0;31m            .-------------------------.\n");
	printf("[1;32m  /|||\\\\    [0;31m|[1mCoMpoSe yOuR MEsSagE HeRe[0;31m|\n");
	printf("[1;32m /[0;31mC^,^C[1;32m\\\\   [0;31m|  [0;31m=-=-=-=-=-=-=-=-=-=-=  [0;31m|\n");
	printf("[1;32m \\\\\\[0;31mo [1;32m///   [0;31m|[1;31m<[1;33m^[1;31m> the `[0;31mcaret[1;31m` key will [0;31m|\n");
	printf("[0;31m/P  ||      [0;31m| [1;31mallow you to change the [0;31m|\n");
	printf("[0;31m\\\\/[1;33m(  )[0;31m\\    [0;31m|[1;31mcolor of text then hit[0;31m,  |\n");
	printf("[0;31m \\/[1;33m \\_\\[0;31m\\\\   [0;31m| [0;34mb[0;31m,r,[0;35mm[0;31m,[0;32mg[0;31m,[0;33my[0;31m,[1;34mB[0;31m,[1;31mR[0;31m,[1;35mM[0;31m,[1;32mG[0;31m, [1;31mor [1;33mY[0;31m.|\n");
	printf("[1;33m   /   \\[0;31m\\P  [0;31m|[1m<[1;33menter[1;31m> key twice prompt [0;31m|\n");
	printf("[1;33m   \\___/    [0;31m| [1;31myou to save the message[0;31m.|\n");
	printf("[0;31m    \\\\\\\\    [0;31m`-------------------------'\n");
	printf("\033[0;31m~~~~\033[1m.\033[0;31m~~~~\033[1m.\033[0;31m~~~~\033[1m.\033[0;31m~~~~\033[1m.\033[0;31m~~~~\033[1m.\033[0;31m~~~~\033[1m.\033[0;31m~~~~\033[1m.\033[0;31m~~~~\033[1m.\033[0m");
}

char *get_who_to(char *whoto) {
	char ch, obuf[200];
	static char ibuf[32];
	int x;

	if(whoto == NULL && rNum != 1) {
		strcpy(ibuf, "All");
		return (char *)ibuf;
	} else if(whoto != NULL) {
		strcpy(ibuf,whoto);
		return (char *)ibuf;
	} else if(rNum == 1 && whoto == NULL) {
		sprintf(obuf,"%s/text/PostMsgPriv",BBSDIR);
		CatFileDirect(obuf);
		printf("\033[1;31mAd\033[33mdress \033[31mT\033[33mo:\033[33m ");
		GetStrName(ibuf,30);
		if(ibuf[0] == '\0') {
			printf("\nAborting Message.\n");
			strcpy(ibuf,"New");
			return (char *) ibuf;
		} else if(!strcmp(ibuf,"List")) {
			printf("\033[0m");
			UserList2(MEMBERS);
			strcpy(ibuf,"New");
			return (char *) ibuf;
		} else if(existuser(ibuf) != TRUE) {
			printf("\nUser [%s] does not exist.\n",ibuf);
			strcpy(ibuf,"New");
			return (char *)ibuf;
		} else {
			return (char *)ibuf;
		}
	}
	strcpy(ibuf,"New");
	return (char *)ibuf;
}

void create_textlist(lot_header *lp) {
        lp->head = (lot_data *) NULL;
        lp->tail = (lot_data *) NULL;
        lp->current = (lot_data *) NULL;
        lp->length = 0;
}

void free_textlist(lot_header *lp) {
        //lot_header *current, *temp;
	lot_data *temp;
        lp->current = lp->head;
        while(lp->current != NULL) {
                temp = lp->current->next;
                free(lp->current);
                lp->current = temp;
        }
        create_textlist(lp);
}

void goto_lot(lot_header *lp, int j) {
        if(j == 0) {lp->current = NULL; return; }
        j -= 1;
        lp->current = lp->head;
        while((lp->current != lp->tail)) {
                j--;
                if(j<0) break;
                lp->current = lp->current->next;
        }
}

int delete_lot(lot_header *lp, int j) {
	lot_data *tmp;
	goto_lot(lp, j);
	if(lp->current == NULL)
		return(FALSE);
	if(lp->current != lp->head) {
		tmp = lp->current->prev;
		lp->current->prev->next = lp->current->next;
		lp->current->next->prev = lp->current->prev;
	} else {
		tmp = lp->current->next;
		lp->head = lp->current->next;
		if(lp->head != NULL)
			lp->head->prev = (lot_data *) NULL;
	}
	if(lp->current == lp->tail) {
		lp->tail = lp->current->prev; 
		if(lp->tail != NULL)
			lp->tail->next = (lot_data *) NULL;
	}
	free(lp->current);
	lp->length--;

	if(lp->head != NULL) {
		j = 1;
		lp->current = lp->head;	

		while(lp->current != lp->tail) {
			lp->current->linenumber = j++;
			lp->current = lp->current->next;
			if(lp->current->next == NULL) break;
		}
		lp->tail->linenumber = j;
	}	

	return(TRUE);
}





void print_lot(lot_header *lp) {
        lp->current = lp->head;
        if(lp->current == NULL) return;
        while(lp->current != lp->tail) {
                printf("\033[1;35m%d\033[0;34m: \033[0;33m%s",lp->current->linenumber,lp->current->linebuf);
                lp->current = lp->current->next;
                if(lp->current->next == NULL) break;
        }
        if(lp->tail == NULL) return;
        printf("\033[1;35m%d\033[0;34m: \033[0;33m%s",lp->tail->linenumber,lp->tail->linebuf);
        printf("\n");
}

int topappend_lot(lot_header *lp, const char *s) {
        lot_data *new;
        if(!(new = (lot_data *) malloc(sizeof(lot_data)))) return FALSE;
        new->prev = (lot_data *) NULL;
        strncpy(new->linebuf,s,250);
        new->linenumber = 1;
        if(lp->head == NULL) {
                lp->tail = new;
                new->next = (lot_data *) NULL;
        } else {
                new->next = lp->head;
                lp->head->prev = new;
                // new sure what to do about line number from here
                lp->current = new->next;
				push_numbers(lp);
        }
        lp->head = new;
        lp->length++;
        lp->current = new;
        return(TRUE);
}

int insert_lot(lot_header *lp, const char *s) {
        lot_data *new;
        if(lp->current == NULL) {
                return(topappend_lot(lp,s));
        }
        if(!(new = (lot_data *) malloc(sizeof(lot_data))))
                return(FALSE);
        strncpy(new->linebuf,s,250);
        new->prev = lp->current;
        new->next = lp->current->next;
        lp->current->next = new;
        new->linenumber = (lp->current->linenumber + 1);
        if(new->next == NULL) {
                lp->tail = new;
        } else {
                new->next->prev = new;
                lp->current = new->next;
				push_numbers(lp);
       	}
        lp->length++;
        lp->current = new;
        return(TRUE);
}

int push_numbers(lot_header *lp) {
	if(lp->tail == lp->head)
		return(FALSE);
	while(lp->current != lp->tail) {
		lp->current->linenumber++;
		lp->current = lp->current->next;
		if(lp->current->next == NULL) break;
	}
	lp->tail->linenumber++;
	return(TRUE);
} 

/** EOF **/
