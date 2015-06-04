#include "common.h"
StringList initLoggedUser() {
	StringList loggedUser = calloc(MAX_LOGGEDUSERS, sizeof(char*));
    return loggedUser;
}

void addLoggedUser(char* username, StringList db ) {
	int i=-1;
	if ( checkLoggedUser(username, db) == 0){
		while(db[++i] != 0);	//esco alla prima occorrenza vuota dell'array
		db[i] = strdup(username);
	}
}


char* listLoggedUser(StringList db, int activeUsers) {
	int i;
	char* toRet = calloc(activeUsers*USERNAME_LENGTH, sizeof(char));
	for(i=0; i<activeUsers; i++){
		if (db[i] == 0)		continue;
		sprintf(toRet,"%s%s:",toRet,db[i]);
	}
	toRet[strlen(toRet) - 1] = '\0';
	return toRet;
}


int checkLoggedUser(char *username, StringList db) {
	int i;
	for(i=0; i<MAX_LOGGEDUSERS; i++){
		if (db[i] == 0)				continue;
		if ( strcmp(username, db[i]) == 0)	return 1;
	}
	return 0;
}

int existsUserAt(int i, StringList db) {
	if ( db[i] == 0 )
		return 0;
	return 1;
}

int getUserAt(int i, StringList db, char* ret) {
	if (db[i] == 0)
        return 0;
	memcpy(ret, db[i], strlen(db[i]));
	return 1;

}

void removeLoggedUser(char *userToDelete, StringList db) {
	int i = -1;

	while (1) {
		if (++i == MAX_LOGGEDUSERS)		return;
		if (db[i] == 0)				continue;
		if (strcmp(userToDelete, db[i]) == 0)	break;
	}

	free(db[i]);
	db[i] = 0;
}
