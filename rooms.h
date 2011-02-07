#define MAXROOMS 32

typedef struct {
	int avail;
	char name[128];
	char path[256];
	char owner[128];
	int toplimit;
} ROOMTYPE;

ROOMTYPE room[MAXROOMS];


int rNum;
int maxrooms;
