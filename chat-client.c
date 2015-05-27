#include "utils.h"

char *username, *input;
int sock, go = 1;

void* clientSender(void* data) {
	char* input = calloc(SL, sizeof(char));
	size_t len = SL;
	msg_t *msg = calloc(1, sizeof(msg_t));
	while (go) {
		getline(&input, &len, stdin);
		switch(cmdMatcher(input)) {

		case 0:	fprintf(stderr, "Error:not a valid command\n");	continue;
		case MSG_LOGIN:		CSLogin(username, msg);		break;
		case MSG_SINGLE:	CSSingle(input, msg);		break;
		case MSG_BRDCAST:	CSBroadcast(input, msg);	break;
		case MSG_LIST:		CSList(msg);			break;
		case MSG_LOGOUT:	CSLogout(msg); 	go = 0;		break;
		}
		write(sock, marshal(msg), SL);
	}
	printf("A presto %s!\n", username);
	exit(0);
}

void* clientListener(void* data) {
	msg_t* msg = calloc(1, sizeof(msg_t));
	char* buffer = calloc(SL, sizeof(char));
	for (;;) {
		read(sock, buffer, SL);

		msg = unMarshal(buffer);
		switch(msg->type) {
		case MSG_SINGLE:
			printf("%s:%s:%s\n", msg->sender, username,  msg->content);
			break;
		case MSG_BRDCAST:
			printf("%s:*:%s\n", msg->sender, msg->content);
			break;
		case MSG_LIST:
			printf("%s\n", msg->content);
			break;
		case MSG_ERROR:
			fprintf(stderr, "%s\n", msg->content);
			break;
		default:
			fprintf(stderr, "Errore: client ha ricevuto un comando sconosciuto\n");
			break;
		}
	}
}

int main(int argc, char** argv) {
	username = calloc(SL, sizeof(char));
	int haveToRegister = 0;
	if (argc > 1) {
		if( strcmp(argv[1], "-h") == 0){
			FILE* help = fopen(HELP_PATH, "r");
			size_t len = 0;
			char *line = 0;
			ssize_t read;
			while ( (read = getline(&line, &len, help)) != -1 )
				printf("%s", line);
			exit(0);
		}

		if ( strcmp(argv[1], "-r") == 0 ) {
			haveToRegister = 1;
			username = argv[3];
		}
		else
			username = argv[1];
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


	msg_t* msg = calloc(1, sizeof(msg_t));

	//Gestione della registrazione
	if (haveToRegister) {
		char* fullname = calloc(SL, sizeof(char));
		char* email;
		char* name = strtok(argv[2], " ");
		char* surname = strtok(0, " ");
		sprintf(fullname, "%s %s", name, surname);
		email = strdup(strtok(0, "\n"));
		CSRelog(username, fullname, email, msg);
		write(sock, marshal(msg), SL);

		input = calloc(SL, sizeof(char));
		read(sock, input, SL);
		msg = unMarshal(input);
		if (msg->type == MSG_ERROR) {
			fprintf(stderr, "%s\n", msg->content);
			exit(3);
		}
		printf("Sei stato registrato con successo\n");

	}

	//Gestione del Login
	CSLogin(username, msg);
	write(sock, marshal(msg), SL);

	input = calloc(SL, sizeof(char));
	read(sock, input, SL);
	bzero(msg, sizeof(msg));
	msg = unMarshal(input);
	if (msg->type == MSG_ERROR) {
		fprintf(stderr, "%s\n", msg->content);
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
