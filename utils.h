#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <time.h>
#include "hash.h" //!Keep this for including string stdio and stdlib

#define DELIM_CHAR ":"
#define USERFILE_NAME "user-file"
#define LOGFILE_NAME "log-file"

char* timestamp(void);
char*  hdata2string(hdata_t*);
hdata_t* string2hdata(char*);
char* writeAccess(int, char*);
char* writeMessage(char*, char*, char*);
void loadUserfileInHash(hash_t);
void saveHashInUserfile(hash_t);


//Return a char[24] as a timestamp
char* timestamp() {
	time_t t = time(0);
	return strtok(ctime(&t), "\n"); //Delete the newLine char from timestamp
}

hdata_t* string2hdata(char *str) {
	hdata_t* toRet = malloc(sizeof(hdata_t));
	str = strtok(str, "\n");
	toRet->uname = strdup(strtok(str, DELIM_CHAR));
	toRet->fullname = strdup(strtok(NULL, DELIM_CHAR));
	toRet->email = strdup(strtok(NULL, DELIM_CHAR));
	toRet->sockid = -1;
	return toRet;
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



void writeAccessToLog(int isItALogin, char* uname) {
	if ((isItALogin != 0) && (isItALogin != 1)) {
		fprintf(stderr, "Invalid argument for writeToLog");
		exit(1);
	}

	FILE *logfile = fopen(LOGFILE_NAME, "a");
	char* toRet = calloc(SL, sizeof(char));
	char *whichLog = malloc(6);
	strcpy(whichLog, "logout");
	if (isItALogin)
		strcpy(whichLog, "login");
	sprintf(toRet, "%s:%s:%s", timestamp(), whichLog, uname);
	fprintf(logfile, "%s\n", toRet);
	fclose(logfile);
}

void writeMessageToLog(char* sender, char* receiver, char* content){
	FILE *logfile = fopen(LOGFILE_NAME, "a");
	char* toRet = calloc(SL, sizeof(char));
	sprintf(toRet, "%s:%s:%s:%s", timestamp(), sender, receiver, content);
	fprintf(logfile, "%s\n", toRet);
	fclose(logfile);
}


void saveHashInUserfile(hash_t H) {
	FILE *userFile = fopen(USERFILE_NAME, "w");
	char* buffer = calloc(SL, sizeof(char));
	int i;
	for (i=0; i<HL; i++) {
		//Metodo per arrivare al contenuto della cella hash
		hdata_t* user= (hdata_t*)((*(H+i))->successivo)->elemento;
		if ( user != 0 ) 
			fprintf(userFile, "%s\n", hdata2string(user));
	}
}



void loadUserfileInHash(hash_t H){
	FILE *userFile = fopen(USERFILE_NAME, "r");
	char* buffer = calloc(SL, sizeof(char));
	hdata_t* userData;
	size_t len = SL;
	while (getline(&buffer, &len, userFile) >= 0) {
		userData = string2hdata(buffer);
		INSERISCIHASH(userData->uname, (void*)userData, H);
		userData = malloc(sizeof(hdata_t));
	}
}

void initLog() {
	FILE* logfile = fopen(LOGFILE_NAME, "w");
	char* welcomeMessage = malloc(256 * sizeof(char));
	sprintf(welcomeMessage, "** Chat Server started @ %s **", timestamp());

	int i;
	for (i=0; i<strlen(welcomeMessage); i++)	fprintf(logfile, "*");
	fprintf(logfile, "\n");
	fprintf(logfile, "%s\n", welcomeMessage);
	for (i=0; i<strlen(welcomeMessage); i++)	fprintf(logfile, "*");
	fprintf(logfile, "\n");

	fclose(logfile);
}

int cmdMatcher(char* cmd) {
	//leggo quale comando si vuole 2eseguire, modificando un altra istanza
	//di cmd, per non intaccare la stringa originale
	cmd  = strtok(strdup(cmd), " ");
	if (strcmp(cmd, "#ls") == 0)	return MSG_LIST;
	if (strcmp(cmd, "#logout")==0)	return MSG_LOGOUT;

	if (strcmp(cmd, "#dest") == 0)	{
		cmd = strtok(0, " ");
		if (cmd[0] == ':')	return MSG_BRDCAST;
		else			return MSG_SINGLE;
	}
	return 0;

}

char* marshal(msg_t *msg){
	char* toRet = calloc(SL, sizeof(char));
	sprintf(toRet, "%c:%s:%s:%u:%s", 
		msg->type, msg->sender, msg->receiver, msg->msglen, msg->content);
	return toRet;

}

msg_t* unMarshal(char* str){
	msg_t* toRet 	= calloc(1, sizeof(msg_t));
	str = strtok(str, DELIM_CHAR);
	toRet->type 	= str[0];
	toRet->sender 	= strdup(strtok(0, DELIM_CHAR));
	toRet->receiver = strdup(strtok(0, DELIM_CHAR));
	toRet->msglen 	= atoi  (strtok(0, DELIM_CHAR));
	toRet->content 	= strdup(strtok(0, DELIM_CHAR));
}
