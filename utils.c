#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <time.h>
#include "hash.h" //!Keep this for including string stdio and stdlib

#define DELIM_CHAR ":"


char* timestamp(void);
char*  hdata2string(hdata_t*);
hdata_t* string2hdata(char*);
char* writeAccess(int, char*);
char* writeMessage(char*, char*, char*);


//Return a char[24] as a timestamp
char* timestamp() {
	time_t t = time(0);
	return strtok(ctime(&t), "\n"); //Delete the newLine char from timestamp
}

char* hdata2string(hdata_t *val) {
	char* toRet = calloc(256, sizeof(char));
	sprintf(toRet, "%s:%s:%s", val->uname, val->fullname, val->email);
	return toRet; 
}

hdata_t* string2hdata(char *str) {
	hdata_t *toRet = malloc(sizeof(hdata_t));
	toRet->uname = strtok(str, DELIM_CHAR);
	toRet->fullname = strtok(NULL, DELIM_CHAR);
	toRet->email = strtok(NULL, DELIM_CHAR);
	return toRet;
}


char* writeAccessToLog(int isItALogin, char* uname) {
	if ((isItALogin != 0) && (isItALogin != 1)) {
		fprintf(stderr, "Invalid argument for writeToLog");
		exit(1);
	}

	char* toRet = calloc(256, sizeof(char));
	char *whichLog = malloc(6);
	strcpy(whichLog, "logout");
	if (isItALogin)
		strcpy(whichLog, "login");
	sprintf(toRet, "%s:%s:%s", timestamp(), whichLog, uname);
	return toRet;
}

char* writeMessageToLog(char* sender, char* receiver, char* content){
	char* toRet = calloc(256, sizeof(char));
	sprintf(toRet, "%s:%s:%s:%s", timestamp(), sender, receiver, content);
	return toRet;
}

int main() {
	hash_t H = CREAHASH();
	char user[] = "tiulalan:Daniele Tolomelli:daniel.tolomelli@gmail.com";
	hdata_t *hash_user = string2hdata(user);
	INSERISCIHASH(hash_user->uname, hash_user, H);
	hdata_t* foundUser = CERCAHASH("Tiulalan", H);
	if (foundUser == NULL)
		printf("Tiulalan doesn't exists in the database\n");
	else
		printf("%s\n", hdata2string(foundUser));
	return 0;
}



