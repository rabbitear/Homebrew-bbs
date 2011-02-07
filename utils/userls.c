#include "../shack.h"
#include "../user.h"

#include <dirent.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char *arg[]);
int ul_print_email(char *user_dir);
long ul_get_days_old(char *user_dir);
void print_usage(void);

int main(int argc, char *argv[]) {
	
	DIR *dp;
	long days_old; 
	struct dirent *dirinfo;
	struct stat st;
	
	if(argc != 3) { print_usage(); return 1; }

	days_old = atol(argv[2]);
	if(days_old < 0) { print_usage(); return 1; }
	if(!((!strcmp(argv[1],"email")) || 
	   (!strcmp(argv[1],"names")) ||
	   (!strcmp(argv[1],"paths")))) { print_usage(); return 1; }

	if((dp = opendir("/home/bbs/users/")) == NULL) return 1;
	while((dirinfo = readdir(dp)) != NULL) {
		if(dirinfo->d_name[0] == '.') continue;
		if(days_old < (ul_get_days_old(dirinfo->d_name))) {
			if(!strcmp(argv[1],"email"))
				ul_print_email(dirinfo->d_name);
			if(!strcmp(argv[1],"names"))
				printf("%s\n",dirinfo->d_name);
			if(!strcmp(argv[1],"paths"))
				printf("/home/bbs/users/%s\n",dirinfo->d_name);
		}
	}
	closedir(dp);
	return 0;
}

void print_usage(void) {
	printf("\nUsage: userls <email/names/paths> <no days old>\n\n");
	return;
}	

int ul_print_email(char *user_dir) {
	
	USERINFO user_info;
	FILE *fp;
	char buf[1024];
	sprintf(buf,"/home/bbs/users/%s/info",user_dir);
	if((fp = fopen(buf,"rb")) != NULL) {
		if(fread((char *)&user_info,sizeof(USERINFO),1,fp) == 1) 
			printf("%s\n", user_info.email);
		fclose(fp);
	} else return 1;
	return 0;
}

long ul_get_days_old(char *user_dir) {

	USERSTATE udata;
	FILE *fp;
	char buf[1024];
	time_t cur_time;
	time(&cur_time);
	sprintf(buf,"/home/bbs/users/%s/state",user_dir);
	if((fp = fopen(buf,"rb")) == NULL) { 
		return -1;
	} else {
		if(fread((char *)&udata,sizeof(USERSTATE),1,fp) == 1) {
			fclose(fp);
			return ((cur_time - udata.time_laston)/ 86400.0);
		}
	}
	return -2;
}

