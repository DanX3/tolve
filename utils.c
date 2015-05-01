#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <time.h>
#include "hash.h" //!Keep this for including string stdio and stdlib

#define DELIM_CHAR ":"
#define USERFILE_NAME "user-file"

/*
char* timestamp(void);
char*  hdata2string(hdata_t*);
hdata_t* string2hdata(char*);
char* writeAccess(int, char*);
char* writeMessage(char*, char*, char*);
void loadUserInHash(hash_t);
void saveHashInFile(hash_t);
*/

//Return a char[24] as a timestamp
char* timestamp() {
	time_t t = time(0);
	return strtok(ctime(&t), "\n"); //Delete the newLine char from timestamp
}

char* hdata2string(hdata_t *val) {
	if (val != 0) {
		char* toRet = calloc(SL, sizeof(char));
		sprintf(toRet, "%s:%s:%s", val->uname, val->fullname, val->email);
		return toRet; 
	} else {
		return '\0';
	}
}



char* writeAccessToLog(int isItALogin, char* uname) {
	if ((isItALogin != 0) && (isItALogin != 1)) {
		fprintf(stderr, "Invalid argument for writeToLog");
		exit(1);
	}

	char* toRet = calloc(SL, sizeof(
	char));
	char *whichLog = malloc(6);
	strcpy(whichLog, "logout");
	if (isItALogin)
		strcpy(whichLog, "login");
	sprintf(toRet, "%s:%s:%s", timestamp(), whichLog, uname);
	return toRet;
}

char* writeMessageToLog(char* sender, char* receiver, char* content){
	char* toRet = calloc(SL, sizeof(char));
	sprintf(toRet, "%s:%s:%s:%s", timestamp(), sender, receiver, content);
	return toRet;
}


/*
void saveHashInFile(hash_t H) {
	FILE *userFile = fopen(USERFILE_NAME, "w");
	char* buffer = calloc(SL, sizeof(char));
	lista l = *H;
	int i;
	for (i=0; i<HL; i++) {
		fprintf(userFile, "%s", hdata2string((hdata_t*)H[i]->elemento));
		l = SUCCLISTA(l);
	}
}
*/


void string2hdata(char *str, hdata_t* toRet) {
	str = strtok(str, "\n");
	toRet->uname = strtok(str, DELIM_CHAR);
	toRet->fullname = strtok(NULL, DELIM_CHAR);
	toRet->email = strtok(NULL, DELIM_CHAR);
}

//p ((hdata_t*)0x60cf30)->uname
void loadUserInHash(hash_t H){
#if 1
	FILE *userFile = fopen(USERFILE_NAME, "r");
	char* buffer = calloc(SL, sizeof(char));
	hdata_t* userData = (hdata_t*) malloc(sizeof(hdata_t));
	size_t len = SL;
	while (getline(&buffer, &len, userFile) >= 0) {
		buffer = strtok(buffer, "\n");
		userData->uname = strtok(buffer, DELIM_CHAR);
		userData->fullname = strtok(NULL, DELIM_CHAR);
		userData->email = strtok(NULL, DELIM_CHAR);
		INSERISCIHASH(userData->uname, (void*)userData, H);
		userData = malloc(sizeof(hdata_t));
	}
	hdata_t* userData2 = CERCAHASH("tiulalan", H);
	buffer = hdata2string(userData2);
	fprintf(stderr, "%s\n", buffer);
#endif
#if 1
	if (1) {
	userData = malloc(sizeof(hdata_t));
	userData->uname = "pato";
	userData->fullname = "Pato di sti coglioni";
	userData->email = "path@ventu.com";
	INSERISCIHASH(userData->uname, userData, H);
	}
	userData = malloc(sizeof(hdata_t));
	userData->uname = "mioZio";
	userData->fullname = "Mio Zio ";
	userData->email = "zioh@stick.com";
	INSERISCIHASH(userData->uname, userData, H);
	hdata_t* bs = CERCAHASH("pato", H);
	fprintf(stderr, "%s\n", hdata2string(bs));
#endif

}

int main() {
	hash_t H = CREAHASH();
	loadUserInHash(H);
	return 0;
}

