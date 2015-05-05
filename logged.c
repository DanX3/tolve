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
	username = strtok(strdup(buffer), ":");
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

int main() {
	char* buffer = calloc(32*SL, sizeof(char));
	
	addLoggedUser("tiulalan", buffer);
	addLoggedUser("ventupath", buffer);
	addLoggedUser("sully46", buffer);
	listLoggedUser(buffer);
	
	printf("%d\n", checkLoggedUser("tiulalan", buffer));
	printf("%d\n", checkLoggedUser("toffax", buffer));
	
	listLoggedUser(buffer);
	return 0;
}
