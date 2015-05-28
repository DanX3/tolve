#define LOGIN_DONE_YET		"Utente gia' connesso"
#define USER_NOT_REGISTERED	"Utente non registrato"
#define HASH_COLLISION		"Collisione nella Hash Table"
#define RECV_NOT_REGISTERED	"Destinatario inesistente"
#define RECV_OFFLINE		"Destinatario non connesso"
#define CMD_NOT_FOUND		"Comando non trovato\n"

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

void writeErrorToLog(char* content, char* username){
	FILE *logfile = fopen(LOGFILE_NAME, "a");
	fprintf(logfile, "Errore da %s: %s\n", username, content);
	fclose(logfile);
}
