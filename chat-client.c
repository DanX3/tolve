#include "utils.h"

char *username, *input;
int sock, go = 1;

void* clientSender(void* data) {
	char* input = calloc(SL, sizeof(char));
	size_t len = SL;
	msg_t *msg = calloc(1, sizeof(msg_t));
	int cmdId, go = 1;
	while (go) {
		getline(&input, &len, stdin);
		input = strtok(input, "\n");
		
		cmdId = cmdMatcher(input);
		bzero(msg, sizeof(msg));
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
			go = 0;
			printf("A presto %s!\n", username);
		}
		write(sock, marshal(msg), SL);
	}
	exit(0);
}

void* clientListener(void* data) {
	char* buffer = calloc(SL, sizeof(char));
	for (;;) {
		read(sock, buffer, SL);
		printf("%s\n", buffer);
	}
}

int main(int argc, char** argv) {
	username = calloc(SL, sizeof(char));
	if (argc > 1) {
		if( strcmp(argv[1], "-h") == 0){
			printf("Scriviamo l'aiuto\n");
			exit(0);
		}

		if ( strcmp(argv[1], "-r") == 0 ) {
			//registra l'utente nella hash
		} else {
			username = argv[1];
		}

	} else {
		printf("Error 2: ./chat-client -h for help\n");
		exit(2);
	}

		
	//Gestione dell'apertura della socket
	struct sockaddr_in server; 
	sock = socket(AF_INET, SOCK_STREAM, 0);
	
	bzero(&server, sizeof(struct sockaddr_in));
	server.sin_family = AF_INET;
	server.sin_port = htons(5001);
	server.sin_addr.s_addr = inet_addr("127.0.0.1");
	
	connect(sock, (struct sockaddr*) &server, sizeof(struct sockaddr_in));


	//Gestione del Login
	msg_t* msg = calloc(1, sizeof(msg_t));
	msg->type = MSG_LOGIN;
	msg->msglen = strlen(username);
	msg->content = username;
	write(sock, marshal(msg), SL);

	free(msg);
	input = calloc(SL, sizeof(char));
	read(sock, input, SL);
	msg = calloc(1, sizeof(msg_t));
	msg = unMarshal(input);
	if (msg->type == MSG_ERROR) {
		fprintf(stderr, "Error 1: user not found\n");
		exit(1);
	} else
		printf("Benvenuto %s!\n", username);
		
	
	//Lancio dei thread sender e listener
	pthread_t sender, listener;
	pthread_create(&sender, 0, clientSender, 0);
	pthread_create(&listener, 0, clientListener, 0);
	pthread_join(sender, 0);
	exit(0);
}
