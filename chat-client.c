#include "utils.h"

int main(int argc, char** argv) {
	char* username = calloc(SL, sizeof(char));
	if (argc > 2) {
		if( strcmp(argv[1], "-h") == 0){
			printf("Scriviamo l'aiuto\n");
			exit(0);
		}

		if ( strcmp(argv[1], "-r") == 0 ) {
			//registra l'utente nella hash
		} else 
			username = argv[1];

	}
	struct sockaddr_in server; 
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	
	bzero(&server, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(5001);
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	connect(sock, (struct sockaddr*) &server, sizeof(struct sockaddr_in));

	char* input;
	size_t len = SL;
	msg_t *msg;

	//Gestione del Login
	msg = calloc(1, sizeof(msg_t));
	msg->msglen = strlen(username);
	msg->sender = username;
	write(sock, marshal(msg), SL);
	//Controllo login?
	

	do {
		input = calloc(SL, sizeof(char));
		getline(&input, &len, stdin);
		input = strtok(input, "\n");
		
		int cmdId = cmdMatcher(input);
		msg = calloc(1, sizeof(msg_t));
		//Devo togliere "#dest " dalla mia stringa di comando
		strtok(input, " ");
		switch(cmdId) {
		case 0:
			fprintf(stderr, "Error:not a valid command\n");
			continue;
		case MSG_SINGLE:
			msg->type = MSG_SINGLE;
			msg->receiver = strdup(strtok(0, DELIM_CHAR));
			msg->content = strdup(strtok(0, DELIM_CHAR));
			msg->msglen = strlen(msg->content);
			break;
		case MSG_LOGIN:
			msg->type = MSG_LOGIN;
			msg->msglen = strlen(username);
			msg->content = username;
			break;
		}
		write(sock, marshal(msg), SL);
			

	} while(strcmp(input, "exit") != 0);
	exit(0);
}

