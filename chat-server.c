#include "utils.h"

int go = 1;

void Server(void);
void* Dispatcher(void*);
void* Worker(void*);

pthread_mutex_t logfileMutex;
hash_t H;
StringList loggedList;

void Server(){
	printf("server pid: %d\n", getpid());

	//Inizializzazione tabella utenti
	H = CREAHASH();
	loadUserfileInHash(H);

	//Inizializzazione logFile
	initLog();

	//Settando l'ambiente
	pthread_mutex_init(&logfileMutex, NULL);
	loggedList = initLoggedUser();

	//Organizzazione socket server-side
	int sock, newSocket;
	struct sockaddr_in my_addr;
	bzero(&my_addr,sizeof(struct sockaddr_in));
	sock = socket(AF_INET, SOCK_STREAM, 0);
	my_addr.sin_family = AF_INET;
	my_addr.sin_port = htons(5001);
	my_addr.sin_addr.s_addr = INADDR_ANY;
	if (bind(sock, (struct sockaddr *) &my_addr, sizeof(my_addr)) < 0) {
		perror("bind");
		exit(-1);	
	}	
	listen(sock, 10);
	while(go){
		newSocket = accept(sock, NULL, 0);
		pthread_t t;
		pthread_create(&t, 0, Worker, (void*)&newSocket);
		pthread_detach(t);
		//termina su SIGTERM o SIGINT (go = 0)
	}
	saveHashInUserfile(H);
	close(sock);
	exit(0);
}

void*  Dispatcher(void* data) {
	//while (go)
	//prod-cons buffer circolare
	//controlla se gli utenti sono online (hash table in mutex)
	//nel caso invia il messaggio tramite socket
}

void*  Worker(void* data) {
	int socket = *(int*)data;
	char *input, *username;
	msg_t *msg;
	username = calloc(SL, sizeof(char));
	while (go) {
		input = calloc(SL, sizeof(char));
		if ( read(socket, input, SL) == 0 )
			exit(1);

		msg = calloc(1, sizeof(msg_t));
		msg = unMarshal(input);
		switch(msg->type) {
		case MSG_LOGIN:
			username = msg->content;
			msg = calloc(1, sizeof(msg_t));
			if (CERCAHASH(username, H) == 0) {
				msg->type = MSG_ERROR;
				write(socket, marshal(msg), SL);
				pthread_exit(0);
			} else {
				msg->type = MSG_OK;
				write(socket, marshal(msg), SL);
				addLoggedUser(username, loggedList);
				writeAccessToLog(1, username);
			}
			break;
		case MSG_SINGLE:
			if (CERCAHASH(msg->receiver, H) == 0) {
				fprintf(stderr, "Error: invalid receiver\n");
				free(input);
				continue;
			}
			pthread_mutex_lock(&logfileMutex);
			writeMessageToLog(username,  msg->receiver, msg->content);
			pthread_mutex_unlock(&logfileMutex);
			break;
		case MSG_BRDCAST:
			//message_broadcast
			break;
		case MSG_LIST:
			write(socket, listLoggedUser(loggedList), SL);
			break;
		case MSG_LOGOUT:
			removeLoggedUser(username, loggedList);
			writeAccessToLog(0, username);
			free(username);
			free(input);
			free(msg);
			pthread_exit(0);
		default:
			fprintf(stderr, "Error: invalid commmand requsted\n");
			break;
		}
		free(input);
	}
	//exec reg o ls | prod-cons con dispatcher (buffer circolare)
}

int main(int argc, char** argv) {
	pid_t p;
	p = fork();
	if(p == 0)
		Server();
	else if(p < 0)
		printf("fork fallita");
	exit(0);

}
