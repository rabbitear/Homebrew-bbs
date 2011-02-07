#include "shack.h"
#include "user.h"
#include <dirent.h>

int SysopMenu(void) {

	char ibuf[200];
	char nbuf[200];
	int i;
	char ch;
	USERINFO uinfo;
	FILE *fp;
	DIR *dp;
	int menu=TRUE;
	int done=FALSE;
	int twit=FALSE;
	struct dirent *dirinfo;
	struct stat *st;

	syslog(LOG_INFO,"%s USER EDITING.",username);
	printf("\n\033[1;35mUser to edit>\033[0m ");
	GetStrName(nbuf,30);
	if(nbuf[0] == '\n') return 0;
	if(is_sysop(nbuf) == TRUE) {
		printf("\n\033[1;35mUser [\033[31m%s\033[1;35m] ",nbuf);
		printf("is a sysop\033[31m!\033[35m can not edit\033[31m!");
		return -1;
	}
	if(existuser(nbuf) == FALSE) {
		printf("\n\033[1;35mUser [\033[31m%s\033[35m] Does Not Exist\033[31m!\033[0m",nbuf);
		return -1;
	} else {
		printf("\n\033[1;35mUser [\033[31m%s\033[35m] Found\033[31m!\n\033[0m",nbuf);
	}
	sprintf(ibuf,"%s/users/%s/info",BBSDIR,nbuf);
	if((fp = fopen(ibuf,"r")) == NULL) return -1;
	if(fread((char *)&uinfo,sizeof(USERINFO),1,fp) != 1) return -1;
	fclose(fp);

	while(!done) {
		if(menu) {
			printf("\n");
			printf("\033[1;32m");
			printf("%s \033[31m(\033[1;35m",nbuf);
			if(uinfo.sex == 'f'||uinfo.sex == 'F') printf("female");
			else if(uinfo.sex == 'm'||uinfo.sex == 'M') printf("male");
			else printf("unknown");
			printf("\033[1;31m)\n");
			
			printf("\033[1;31m1st login was \033[1;31m");
			printf("%ld, \033[1;35m[\033[31mD\033[35m]elete\n",uinfo.time_firston);

			sprintf(ibuf,"%s/users/%s/twit",BBSDIR,nbuf);
			if((fp = fopen(ibuf,"r")) != NULL) {
				printf("\033[1;35mThis User *\033[1;31mIS\033[1;35m* a [\033[1;31mT\033[1;35m]wit");
				twit = TRUE;
				fclose(fp);
			} else {
				printf("\033[1;35mThis User is *\033[1;31mNOT\033[1;35m* a [\033[1;31mT\033[1;35m]wit");	
				twit = FALSE;
			}
	
			printf("\n\n");
			printf("\033[1;35m1\033[0m> \033[1;32mEmail Address");
			printf("\033[0m: \033[1;35m%s\n",uinfo.email);

			printf("\033[1;35m2\033[0m> \033[1;32mPassword");
			printf("\033[0m: \033[1;35m");
			//for(i=0;i<strlen(uinfo.pass);i++) printf("x");
			printf("%s",uinfo.pass);
			printf("\n");

			printf("\033[1;35m3\033[0m> \033[1;32mStatus");
			printf("\033[0m: \033[1;35m");
			
			sprintf(ibuf,"%s/users/%s/keycode",BBSDIR,nbuf);
			if((fp = fopen(ibuf,"r")) == NULL) {
				printf("Regular User");
			} else {
				printf("New User");
				fclose(fp);
			}
			printf("\n\033[1;35m4\033[0m> \033[1;32mView \033[1;31m%s\033[1;32m'\033[1;31ms \033[1;32miP Log.\033[0m",nbuf);
			printf("\n\n");
		}
		
		printf("\033[1;35mEdit %s> ",nbuf);
		ch = GetKey(0);
		switch(ch) {
		case 'D':
		case 'd':
			printf("\n\n\033[1;35m -> Type delete the bastard to Delete [\033[1;31m%s\033[1;35m] <-\n\n",nbuf);
			printf("Just Hit [\033[1;31mENTER\033[1;35m] to Cancel\033[1;31m:\033[1;33m ");
			GetStr(ibuf,18,0);
			if(!strcmp(ibuf,"delete the bastard")) {
				// TRUE delete user!
				printf("\n");
				sprintf(ibuf,"%s/users/%s/",BBSDIR,nbuf);
				dp = opendir(ibuf);
				while((dirinfo = readdir(dp)) != NULL) {
					if(dirinfo->d_name[0] == '.') continue;
					sprintf(ibuf,"%s/users/%s/%s",
						BBSDIR,
						nbuf,
						dirinfo->d_name);
					if(unlink(ibuf)) {
						printf("\033[0;32m%s..not deleted!\n",dirinfo->d_name);
					} else {
						printf("\033[0;32m%s..deleted!\n",dirinfo->d_name);
					}
				}
				closedir(dp);
				sprintf(ibuf,"%s/users/%s",BBSDIR,nbuf);
				if(rmdir(ibuf)) {
					printf("\033[1;35m[\033[1;31m%s\033[1;35m] failed to delete!",nbuf);
				} else {
					printf("\033[1;35m[\033[1;31m%s\033[1;35m] deleted successfully",nbuf);
				}
			} else {
				printf("\n\033[1;32mDid *not* delete %s!",nbuf);
			}
			done = TRUE;
			break;
		case '3':
			sprintf(ibuf,"%s/users/%s/keycode",BBSDIR,nbuf);
			if(unlink(ibuf)) {
				printf("\nno keycode deleted!");
			} else {
				printf("\nkeycode deleted!");
			}
			done = TRUE;
			break;
		case '4':
			printf("iP Log..\n\n\033[0m");
			sprintf(ibuf,"%s/users/%s/ipRecord",BBSDIR,nbuf);
			CatFileDirect(ibuf);
			done = TRUE;
			break;
		case 'T':
		case 't':
			sprintf(ibuf,"%s/users/%s/twit",BBSDIR,nbuf);
			if(twit) {
				if(unlink(ibuf)) {
					printf("\n\n\033[1;33m%s *NOT* UnTwited!!\n",nbuf);
				} else {
					printf("\n\n\033[1;33m%s UnTwited!!\n",nbuf);
				}
			} else {
				if((fp = fopen(ibuf,"w")) == NULL) {
					printf("\n\n\033[1;33mCan't create TWIT for %s!!\n",nbuf);
					return;
				}
				fprintf(fp,"%s, you are a twit\n",nbuf);
				fclose(fp);
				printf("\n\n\033[1;33m%s is a Twit!!\n",nbuf);
			}
			done = FALSE;
			break;
			
		default:
			done = TRUE;
			break;
		}
	}
}

/*
User to edit> Mea

User Mea (female) 
1st Call was 83.2 days ago
1> Email Address: yo29@yahoo.com
2> Password: XXXXXXXXX
3> Status: Regular Member
4> View Mea's ipLog

User Edit>
*/


