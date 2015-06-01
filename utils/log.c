#include "misc.c"

char* logPath;

void initLog(char* logPathArg) {
	logPath = strdup(logPathArg);
	FILE* logfile = fopen(logPath, "w");
	char* welcomeMessage = malloc(256 * sizeof(char));
	sprintf(welcomeMessage, "** Chat Server started @ %s **", timestamp());

	int i;
	for (i=0; i<strlen(welcomeMessage); i++)	fprintf(logfile, "*");
	fprintf(logfile, "\n");
	fprintf(logfile, "%s\n", welcomeMessage);
	for (i=0; i<strlen(welcomeMessage); i++)	fprintf(logfile, "*");
	fprintf(logfile, "\n");

	fclose(logfile);
	free(welcomeMessage);
}

void writeAccessToLog(int isItALogin, char* uname) {
	if ((isItALogin != 0) && (isItALogin != 1)) {
		fprintf(stderr, "Invalid argument for writeToLog");
		exit(1);
	}

	FILE *logfile = fopen(logPath, "a");
	char* toRet = calloc(SL, sizeof(char));
	char *whichLog = malloc(6);
	strcpy(whichLog, "logout");
	if (isItALogin)
		strcpy(whichLog, "login");
	sprintf(toRet, "%s:%s:%s", timestamp(), whichLog, uname);
	fprintf(logfile, "%s\n", toRet);
	fclose(logfile);
	free(toRet);
}

void writeMessageToLog(char* sender, char* receiver, char* content){
	FILE *logfile = fopen(logPath, "a");
	char* toRet = calloc(SL, sizeof(char));
	sprintf(toRet, "%s:%s:%s:%s", timestamp(), sender, receiver, content);
	fprintf(logfile, "%s\n", toRet);
	fclose(logfile);
	free(toRet);
}

void writeErrorToLog(char* content, char* username){
	FILE *logfile = fopen(logPath, "a");
	fprintf(logfile, "Errore da %s: %s\n", username, content);
	fclose(logfile);
}
