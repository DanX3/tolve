/*I have to implement log-handler functions */
//removeLoggedUser --> reshape loggedList removing the user who made logout


#include "utils.h"
#define MAX_LOGGEDUSERS 256
#define USERNAME_LENGTH 32

typedef char** StringList;

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


char* listLoggedUser(StringList db) {
	int i;
	char* toRet = calloc(MAX_LOGGEDUSERS*USERNAME_LENGTH, sizeof(char));
	for(i=0; i<MAX_LOGGEDUSERS; i++){
		if (db[i] == 0)
			continue;
		sprintf(toRet,"%s%s:",toRet,db[i]);
	}
	toRet[strlen(toRet) - 1] = '\0';
	return toRet;
}
	

int checkLoggedUser(char *username, StringList db) {
	int i;
	for(i=0; i<MAX_LOGGEDUSERS; i++){
		if (db[i] == 0)
			continue;
		if ( strcmp(username, db[i]) == 0)
			return 1;		
	}
	return 0;
}


void removeLoggedUser(char *userToDelete, StringList db) {
	int i=-1;
	while(strcmp(userToDelete,db[++i])!= 0) {//scorro array fino all'occorrenza
		if (i == MAX_LOGGEDUSERS)
			break;	
	}
	free(db[i]);
	db[i] = 0;
}

int main() {
	StringList loggedList = initLoggedUser();
	
	addLoggedUser("tiulalan", loggedList);
	addLoggedUser("ventupath", loggedList);
	addLoggedUser("sully46", loggedList);
	printf("%s\n" ,listLoggedUser(loggedList));
	
	printf("%d\n", checkLoggedUser("tiulalan", loggedList));
	printf("%d\n", checkLoggedUser("toffax", loggedList));
	removeLoggedUser("tiulalan", loggedList);
//	addLoggedUser("toffax", loggedList);
//	removeLoggedUser("ventupath", loggedList);
	printf("%s\n" ,listLoggedUser(loggedList));
//	removeLoggedUser("toffax", loggedList);		
//	printf("%s\n" ,listLoggedUser(loggedList));
	//removeLoggedUser("sully46", loggedList);
	//addLoggedUser("toffax", loggedList);
	//printf("%s\n" ,listLoggedUser(loggedList));
	return 0;
}
