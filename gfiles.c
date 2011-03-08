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

/********************************************************/
/* Quick TODO:                                          */
/*  - If the directory is empty, say, its empty.        */
/********************************************************/

#include "shack.h"
#include <unistd.h>
#include <dirent.h>
#include <string.h>

void GFilesMenu() {
	int i=0;
	char ibuf[128], fbuf[500], *nextbuf, path[1000], done = FALSE, ch;
	char descbuf[42];
	FILE *fp;
	DIR *dp;
	struct dirent *dirinfo;
	struct stat st;
	
	fflush(stdout);
	path[0] = '\0';
	CatFileDirect("/home/bbs/text/gfiles-logo");
	while(!done) {
		ibuf[0] = '\0';
		if(path[0] == '\0')
			printf("\033[0;31msH\033[32macK \033[31mGf\033[32miLeS \033[1;31m[\033[1;32m/\033[1;31m]\033[32m[?]=:> \033[0m");
		else
			printf("\033[0;31msH\033[32macK \033[31mGf\033[32miLeS \033[1;31m[\033[1;32m%s\033[1;31m]\033[32m> \033[0m\033[0;33mType '\033[1mhelp\033[0;33m' for commands.\n\n",path);

                fflush(stdout);
		GetStr(ibuf,99,0);
		if(ibuf[0] == '\0') {
			printf("\n \033[0;33mType '\033[1mhelp\033[0;33m' for commands.\n\n");
			continue;
		}
		nextbuf = strtok(ibuf," ");
		if((!strcmp(nextbuf,"list")) || (!strcmp(nextbuf,"ls"))) {
			i=0;
			sprintf(fbuf,"%s/text/tf%s/header.desc",BBSDIR,path);
			if((fp = fopen(fbuf,"rt")) != NULL) {
				for(;;) {
					if(fgets(fbuf,127,fp) != NULL) {
						printf("%s",fbuf);
						i++;
					} else {
						break;
					}
				}	
				fclose(fp);
			}
			printf("\033[1;31m::[\033[32mFiLeNaMe\033[31m]:::::::[\033[32mSiZe\033[31m]::[\033[32mDeScRiPtIoN\033[31m]:::::::::::::::::::::::::::::\n");
			i++;
			sprintf(fbuf,"%s/text/tf%s/",BBSDIR,path);
			dp = opendir(fbuf);
			while((dirinfo = readdir(dp)) != NULL) {
				if(dirinfo->d_name[0] == '.') continue;
				if(i>rows-2) {
					if(!More3()) {
						done = FALSE;
						break;
					}
					i=0;
				}
				sprintf(fbuf,"%s/text/tf%s/%s",BBSDIR,path,dirinfo->d_name);
				stat(fbuf,&st);
				if(S_ISDIR(st.st_mode)) {
					printf("\033[1;33m%-17.17s \033[0;32m<\033[1;31mdir\033[0;32m>\n",dirinfo->d_name);		
					/* should fix the directory not being*/
					/* increamented for More prompts */
					i++;
					continue;
				}
				if(strstr(dirinfo->d_name,".desc") == NULL) {
					/* is not a .desc file */
					sprintf(fbuf,"%s/text/tf%s/%s.desc",BBSDIR,path,dirinfo->d_name);
					if((fp = fopen(fbuf,"rt")) == NULL) continue;
					if(fgets(fbuf,127,fp) == NULL) continue;
					printf("\033[0;33m%-17.17s \033[0;32m%6ldb \033[1;32m",dirinfo->d_name,st.st_size);
					if(fbuf[0] == '\0')
						printf("its null!\n");
					else
						printf("%s",fbuf);
					fclose(fp);
					i++;
				} else continue;
			}
			closedir(dp);
			done = FALSE;
			continue;
		} else if((!strcmp(nextbuf,"help")) || (!strcmp(nextbuf,"?"))) {
			printf("\n");
			printf("  \033[1;32mlist           \033[0;31m- \033[32mlist files in directory\n");
			printf("  \033[1;32mread <\033[0;32mfile\033[1;32m>    \033[0;31m- \033[32mread a file\n");
			printf("  \033[1;32mgoto <\033[0;32mdir\033[1;32m>     \033[0;31m- \033[32mgoto a directory\n");
			printf("  \033[1;32mback           \033[0;31m- \033[32mgo back one directory\n");
			printf("  \033[1;32mupload <\033[0;32mfile\033[1;32m>  \033[0;31m- \033[32mupload a file\n");
			printf("  \033[1;32mexit           \033[0;31m- \033[32mexit back to bbs\n");
			printf("\n");
			done = FALSE;
		} else if(!strcmp(nextbuf,"goto")) {
			nextbuf = strtok(NULL," ");
			if(nextbuf == NULL) {
				printf("\n\033[1;32mGoto What?\n\n");
				continue;
			}
			if(nextbuf[0] == '.') {
				printf("\nNo!\n\n");
				continue;
			}
			sprintf(fbuf,"%s/text/tf%s/%s",BBSDIR,path,nextbuf);
			if(!stat(fbuf,&st)) {
				if(S_ISDIR(st.st_mode)) {
					sprintf(fbuf,"/%s",nextbuf);
					strcat(path,fbuf);
					printf("\n");
				} else {
					printf("\n\033[1;32m[%s] is not a directory.\n\n",nextbuf);
				}
			} else
				printf("\n\033[0;33m[%s] is not a directory.\n\n",nextbuf);			
			done = FALSE;
		} else if(!strcmp(nextbuf,"back")) {
			if(path[0] == '\0') {
				printf("\nAlready in first top directory.\n\n");
				continue;
			} else {
				for(i = (strlen(path)-1) ; path[i] != '/' ; --i) { }
				path[i] = '\0';
			}
			printf("\n");
			done = FALSE;
		} else if((!strcmp(nextbuf,"read")) || (!strcmp(nextbuf,"cat"))) {
			nextbuf = strtok(NULL," ");
			if(nextbuf == NULL) {
				printf("\n\033[1;32mRead What?\n\n");
				continue;
			}
			printf("\033[0m");
			sprintf(fbuf,"%s/text/tf%s/%s",BBSDIR,path,nextbuf);
			if(CatFileDirect2(fbuf) == -1) 
				printf("\n\033[1;33mNo Textfiles called [\033[0;32m%s\033[1;33m]!\n\n",ibuf);
			done = FALSE;
			continue;
		} else if(!strcmp(nextbuf,"upload")) {
			nextbuf = strtok(NULL," ");
			if(nextbuf != NULL) {
				if(strlen(nextbuf) <= 5) {
					printf("\n\033[1;35mThe file name must be \033[1;31m5 characters or longer!\033[1;35m.\n");
					printf("Please Try again..\n\n");
					done = FALSE;
					continue;
				}

				sprintf(fbuf,"%s/text/tf/%s",BBSDIR,nextbuf);
				if((fp = fopen(fbuf,"r")) != NULL) {
					printf("\n\033[1;35m[\033[31m%s\033[35m] already exists\n",nextbuf);
					printf("Please Try again..\n\n");
					fclose(fp);
					done = FALSE;
					continue;
				}
				printf("\n\033[1;36m  EnTeR a DeScRiPtiOn:\n");
				printf("\033[1;32m[\033[1;31m------------\033[0;32m----------------------------\033[1;32m]\n:");
				GetStr(descbuf,40,0);
					if(strlen(descbuf) <= 12) {
					printf("\033[1;35mDescription is too short.\n");
					printf("Please try again..\n\n");
					done = FALSE;
					continue;
				}
				sprintf(fbuf,"%s/text/tf/%s.desc",BBSDIR,nextbuf);
				if((fp = fopen(fbuf,"wt")) == NULL) {
					printf("\nCan not open description\n");
					done = FALSE;
					continue;
				}
				fprintf(fp, "%s\n",descbuf);		
				fclose(fp);
				sprintf(fbuf,"%s/text/tf/%s",BBSDIR,nextbuf);
				if((fp = fopen(fbuf,"wt")) == NULL) {
					printf("\nCan not open write file\n");
					done = FALSE;
					continue;
				}
				printf("\n\033[1;32mUpload The File Here!\n");
				printf("\033[0;32mPress [\033[1;31mCTRL-X\033[0;32m] when done.\n");
				printf("\033[0m");	
				for(;;) {
					ch = GetKey(YES_CTRLX|YES_ESCKEY);
					i++;
					switch(ch) {
						case 0 ... 7: continue;
						case 8 ... 9: break;
						case 10: ch = '\n'; break;
						case 11 ... 12: continue;
						case 13: ch = '\n'; break;
						case 14 ... 23: continue;
						case 24: i = 32001; break;
						case 25 ... 26: continue;
						case 27: ch = 27; break;
						case 28 ... 31: continue;
						case 32 ... 126: break;
						default: continue;
					}
					if(i > 32000) {
						break;
					} else {
						fputc(ch,fp);
						printf("%c",ch);
					}
				}
				fclose(fp);
				fflush(stdout);
				printf("\n\033[0;1;33mThankyou for uploading :)\n");
				done = FALSE;
				continue;

			} else {
				printf("\n\033[1;35mUpload what?\n");
				printf("   *\033[1;31mhint\033[1;35m* type \033[1;36mupload <myfilename>\033[0m\n\n"); 
			}
			done = FALSE;
		} else if(!strcmp(nextbuf,"logout")) {
			logout();
		} else if((!strcmp(nextbuf,"exit")) || (!strcmp(nextbuf,"quit"))) {
			done = TRUE;
		} else {
			printf("\n\033[0;32mI don't know how to %s.\n",nextbuf);
			printf("\n \033[0;33mType '\033[1mhelp\033[0;33m' for commands.\n\n");
			done = FALSE;
		}
	}
}

