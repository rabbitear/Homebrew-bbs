#include "shack.h"
#include "user.h"
#include "msg.h"
#include "rooms.h"


void FakeMail() {
	MSGBODY *msg;
	msg = calloc(1,sizeof(MSGBODY));
	
	strcpy(msg->subject,"Hello..");
	msg->date = time(NULL);
	msg->unumber = 0;
	strcpy(msg->uhandle,SYSOPNAME);
	strcpy(msg->uto,(char *)get_user_name());
	strcpy(msg->text,"Welcome!\n");
	WriteNewMail(*msg,msg->uto);
	free(msg);
	return;
}


void file_editor(const char *filename, const int maxlines) {
	int	 x,y;
	char obuf[200], ibuf[200], ch, ch2[2];
	FILE *fp;

	struct tm *tp;
	time_t end_time=0,   // end time
	       start_time=0; // start time

	
	//zero the file, start a timer.
	time(&start_time);
	if((fp = fopen(filename,"wt")) == NULL) return;
	fclose(fp);

	//let user write message for the file
	printf("\x1b[0m");
	printf("\033[1;34mHit Enter On Blank Line To Finish!\n");
	printf("\033[0;31m~~~~\033[1mv\033[0;31m~~~~\033[1mx\033[0;31m~~~~\033[1mv\033[0;31m~~~~\033[1mx\033[0;31m~~~~\033[1mv\033[0;31m~~~~\033[1mx\033[0;31m~~~~\033[1mv\033[0;31m~~~~\033[1mx\033[0m");
	
	ibuf[0] = '\0'; //init the variable
	x=0;
	y=0;
	printf("\x1b[0;32m\n");  //start the input prompt
	for(;;) {
		ch = (char)GetKey(0);
		if(ch == '\n') {
			if(!x) {
				strcpy(ibuf,"\n");
				break;
			} //break to save
			printf("\n");
			x=0;
			if((fp = fopen(filename,"at")) == NULL) return;
			fprintf(fp,"%s\n",ibuf);
			fclose(fp);			
			if(y> (maxlines+2)) break; /* allows 6 lines of text */
			y++;
			ibuf[0] = '\0';
			continue;
		}
		if( ((ch=='\b')||(ch=='\x7f')) && (!x)) continue;		
		if((ch=='\b')||(ch=='\x7f')) {
			if(ibuf[strlen(ibuf)-1] == 'm' 
			&& strlen(ibuf) >= 7
			&& ibuf[strlen(ibuf)-7] == '\033') {
				ibuf[strlen(ibuf)-7] = '\0';	
			} else {
				x--;
				ibuf[strlen(ibuf)-1] = '\0';
				printf("\b \b");
			}
			continue;
		} 
		if(ch=='^') {
			char chstr[8];
			char cha;
			chstr[0] = '\0';
			printf("^");
			cha = (char)GetKey(0);
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
		
		if(isprint(ch)) {
			char chstr[2];
			chstr[0] = ch;
			chstr[1] = '\0';
			strcat(ibuf, chstr);

			if((x>=39)&&(strchr(ibuf,' '))) {
				char *p;
				p=&ibuf[strlen(ibuf)-1];
				while(!isspace(*p)) {
					printf("\b \b");
					p--;
				}
				*p='\0';
				p++;
				if((fp = fopen(filename,"at")) == NULL) return;
				fprintf(fp,"%s\n",ibuf);
				fclose(fp);				
				strcpy(ibuf, p);
				x=strlen(ibuf);
				printf("\n%s",ibuf);
				if(y>4) break; /* allows 6 lines of text */
				y++;
			} else {
				x++;
				printf("%c",ch);
			}
			continue;
		}
	}
	
		/* close down msg text */

	time(&end_time);
	printf("\x1b[0;35mSaving File..\n ");
	printf("\x1b[0;33mTime spend writting\x1b[0m: %ld \x1b[36mseconds.", end_time - start_time);
	printf("\n\n\x1b[0m");
	return;	/* write_msg function all done */
}

/** EOF **/
