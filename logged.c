/*I have to implement log-handler functions */
//removeLoggedUser --> reshape buffer removing the user who made logout

#include "utils.h"

void addLoggedUser(char* username, char* buffer) {
	if (buffer[0] == '\0')
		strncpy(buffer, username, strlen(username));
	else
		sprintf(buffer, "%s:%s", buffer, username);
}


void listLoggedUser(char* buffer) {
	char* username = calloc(SL, sizeof(char));
	username = strtok(strdup(buffer), ":"); //strdup per non alterare buffer
	printf("%s\n", username);
	for (;;) {
		username = strtok(0, ":");
		if (username == 0)	break;
		fprintf(stderr, "%s\n", username);
	}
}

int checkLoggedUser(char *username, char *buffer) {
	if ( strstr(buffer, username) == 0 )	return 0;
	else					return 1;
}


void removeLoggedUser(char *userToDelete, char *buffer) {
	char* currentUser = calloc(SL, sizeof(char));
	char* toReturn = calloc(SL, sizeof(char));
	currentUser = strtok(buffer, ":");
	for (;;) {
		if  ( strcmp(userToDelete, currentUser) == 0 ) {
			currentUser = strtok(0, ":");
			continue;
		}
		sprintf(toReturn, "%s:%s", toReturn, currentUser);
		currentUser = strtok(0, ":");
		if (currentUser == 0)
			break;
	}
	if (toReturn[0] == ':')
		toReturn = toReturn + 1;
	strcpy(buffer, toReturn);
	//free(toReturn);
}

int main() {
	char* buffer = calloc(32*SL, sizeof(char));
	
	addLoggedUser("tiulalan", buffer);
	addLoggedUser("ventupath", buffer);
	addLoggedUser("sully46", buffer);
	listLoggedUser(buffer);
	
	printf("%d\n", checkLoggedUser("tiulalan", buffer));
	printf("%d\n", checkLoggedUser("toffax", buffer));
	removeLoggedUser("tiulalan", buffer);
	removeLoggedUser("ventupath", buffer);
	removeLoggedUser("sully46", buffer);
	//addLoggedUser("sully46", buffer);
	listLoggedUser(buffer);
	return 0;
}
