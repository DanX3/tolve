#include "utils.h"

int main(int argc, char** argv) {
	char* username = calloc(SL, sizeof(char));
	if (argc > 1) {
		if( strcmp(argv[1], "-h") == 0){
			printf("Scriviamo l'aiuto\n");
			exit(0);
		}

		if ( strcmp(argv[1], "-r") == 0 ) {
			//registra l'utente nella hash
		} else 
			username = argv[1];

	} else {
		printf("Error 2: ./chat-client -h for help\n");
		exit(2);
	}
		

	struct sockaddr_in server; 
	int sock = socket(AF_INET, SOCK_STREAM, 0);
	
	bzero(&server, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(5001);
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	connect(sock, (struct sockaddr*) &server, sizeof(struct sockaddr_in));

	char* input = calloc(SL, sizeof(char));
	size_t len = SL;
	msg_t *msg;
	int cmdId;

	//Gestione del Login
	msg = calloc(1, sizeof(msg_t));
	msg->type = MSG_LOGIN;
	msg->msglen = strlen(username);
	msg->content = username;
	write(sock, marshal(msg), SL);

	msg = calloc(1, sizeof(msg_t));
	read(sock, input, SL);
	msg = unMarshal(input);
	if (msg->type == MSG_ERROR) {
		fprintf(stderr, "Error 1: user not found\n");
		exit(1);
	} else
		printf("Benvenuto %s!\n", username);
		

	
	//Ciclo per l'interazione con l'utente
	do {
		input = calloc(SL, sizeof(char));
		getline(&input, &len, stdin);
		input = strtok(input, "\n");
		
		cmdId = cmdMatcher(input);
		msg = calloc(1, sizeof(msg_t));
		//Devo togliere "#dest " dalla mia stringa di comando
		strtok(input, " ");
		switch(cmdId) {
		case 0:
			fprintf(stderr, "Error:not a valid command\n");
			continue;
		case MSG_LOGIN:
			msg->type = MSG_LOGIN;
			msg->msglen = strlen(username);
			msg->content = username;
			break;
		case MSG_SINGLE:
			msg->type = MSG_SINGLE;
			msg->receiver = strdup(strtok(0, DELIM_CHAR));
			msg->content = strdup(strtok(0, DELIM_CHAR));
			msg->msglen = strlen(msg->content);
			break;
		case MSG_BRDCAST:
			msg->content = strdup(strtok(0, DELIM_CHAR));
			msg->msglen = strlen(msg->content);
			break;
		case MSG_LIST:
			msg->type = MSG_LIST;
			break;
		case MSG_LOGOUT:
			msg->type = MSG_LOGOUT;
			break;
		}
		write(sock, marshal(msg), SL);

		if (cmdId == MSG_LIST) {
			char *buffer = calloc(MAX_LOGGEDUSERS*USERNAME_LENGTH, sizeof(char));
			read(sock, buffer, MAX_LOGGEDUSERS*USERNAME_LENGTH);
			printf("%s\n", buffer);
			
		}
		
	} while( cmdId != MSG_LOGOUT );
	printf("A presto %s!\n", username);
	exit(0);
}

