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

#include "../shack.h"
#include "../user.h"

#include <dirent.h>

int main(void) {
	
	DIR *dp;
	char buf[500],buf2[500];
	time_t t;
	int x=0;
	
	struct dirent *dirinfo;
	struct stat st;
	USERSTATE udata;
	USERINFO uinfo;
	FILE *fp;
	
	dp = opendir("/home/bbs/users/");
	time(&t);	
	while((dirinfo = readdir(dp)) != NULL) {
		if(dirinfo->d_name[0] == '.') continue;
		sprintf(buf,"/home/bbs/users/%s/state",dirinfo->d_name);
		if((fp = fopen(buf,"rb")) != NULL) {
			if(fread((char *)&udata,sizeof(USERSTATE),1,fp) == 1) {
				if( ((86400 * 90) <=  t - udata.time_laston) 
				&&(udata.time_posting <= 0)) {
					printf("%s last on (%.1f days ago)",dirinfo->d_name,(t - udata.time_laston) / 86400.0);
					fclose(fp);
					strcpy(buf,"/tmp/bbsmsg.XXXXXX");
					mktemp(buf);
					sleep(1);
					printf("..tmp");
					if((fp = fopen(buf,"w")) == NULL) {
						printf("..can't create tmp! *skipping*\n");
						continue;
					}
					sleep(1);

fprintf(fp,"                                ,\n");
fprintf(fp,"  tHe sHacK! bBS..            _/{\n");
fprintf(fp,"            ,_           o  .'   './`>\n");
fprintf(fp,"            _}\\_  O        / a ((  =<\n");
fprintf(fp,"       <`\\.'    '.  o      '.,__,.'\\_>\n");
fprintf(fp,"        >    )) e \\             \\)\n");
fprintf(fp,"       <_/'.,___,.'         toga.cx\n");    
fprintf(fp,"              (/      velvet.ath.cx\n");
fprintf(fp,"\n");
fprintf(fp,"Greetz %s,\n",dirinfo->d_name);
fprintf(fp,"\n");
fprintf(fp,"The sHacK bBS formally known as toga.cx\n");
fprintf(fp,"has moved to a new location, Texas!  We\n");
fprintf(fp,"also have a new address 'velvet.ath.cx'\n");
fprintf(fp,"\n");
fprintf(fp,"YoUR WeLCoMe To CoMe bY aNd eNjoY oUR\n");
fprintf(fp,"HoT & JuiCy' HoMe CooKeD TeXaS HideAWay\n");
fprintf(fp,"MeSSaGe FoRuMs oN a StEaK 24 HoURs a\n");
fprintf(fp,"daY.\n");
fprintf(fp,"\n");
fprintf(fp,"THaNks, aNd HaVe a GrEaT daY!\n");
fprintf(fp,"\n");
fprintf(fp,"telnet://velvet.ath.cx\n");
fprintf(fp,"ssh://bbs@velvet.ath.cx\n");

					fclose(fp);
					sprintf(buf2,"/home/bbs/users/%s/info",dirinfo->d_name);
                			if((fp = fopen(buf,"rb")) == NULL) {
						printf("..can't open userinfo! *skipping*\n");
						continue;
					}
                        		if(fread((char *)&uinfo,sizeof(USERINFO),1,fp) == 1) {
						printf("..emailing %s", uinfo.email);

						sprintf(buf2,"mail -s \"GreeTz FRoM tHe sHacK bBS!\" %s < %s", uinfo.email, buf);
						//sprintf(buf2,"mail -s \"GreeTz FRoM tHe sHacK bBS!\" %s < %s","kreator@velvet.ath.cx", buf);
						system(buf2)
						printf("..sent");
						x++;
					}
					fclose(fp);
					printf("\n");
				}
			}
		}
	}
	closedir(dp);
	printf("-- %d users emailed\n",x);
	return 0;
}

