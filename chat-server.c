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
	char *username;
	char *input = calloc(SL, sizeof(char));
	msg_t* msg = calloc(1, sizeof(msg_t));
	username = calloc(SL, sizeof(char));
	while (go) {
		if ( read(socket, input, SL) == 0 )
			exit(1);
		msg = unMarshal(input);
		switch(msg->type) {
		case MSG_LOGIN:
			username = msg->content;
			bzero(msg, sizeof(msg_t));
			if (CERCAHASH(username, H) == 0) {
				SCError("", msg);
				write(socket, marshal(msg), SL);
				pthread_exit(0);
			} else {
				SCOK(msg);
				write(socket, marshal(msg), SL);
				addLoggedUser(username, loggedList);
				getDataFrom(username, H)->sockid = socket;
				writeAccessToLog(1, username);
			}
			break;
		case MSG_REGLOG:
			if ( INSERISCIHASH(string2hdata(msg->content)->uname, string2hdata(msg->content), H) == 0 )
				SCError("Errore: collisione nell'inserimento nella hash table", msg);
			else
				SCOK(msg);
			write(socket, marshal(msg), SL);
			break;
		case MSG_SINGLE:
			if ( CERCAHASH(msg->receiver, H) == 0 ) {
				SCError("Errore: destinatario non registrato", msg);
				write(socket, marshal(msg), SL);
			} else
			if ( checkLoggedUser(msg->receiver, loggedList) == 0) {
				SCError("Errore: destinatario non connesso", msg);
				write(socket, marshal(msg), SL);
			} else {
				pthread_mutex_lock(&logfileMutex);
				writeMessageToLog(username,  msg->receiver, msg->content);
				pthread_mutex_unlock(&logfileMutex);

				int recvSock = getDataFrom(msg->receiver, H)->sockid;
				SCSingle(getDataFrom(username, H)->fullname, msg->content, msg);
				write(recvSock, marshal(msg), SL);

			}
			break;
		case MSG_BRDCAST:
			//message_broadcast
			break;
		case MSG_LIST:
			SCList(listLoggedUser(loggedList), msg);
			write(socket, marshal(msg), SL);
			break;
		case MSG_LOGOUT:
			removeLoggedUser(username, loggedList);
			getDataFrom(username, H)->sockid = -1;
			writeAccessToLog(0, username);
			free(input);
			free(msg);
			pthread_exit(0);
		default:
			fprintf(stderr, "Errore: comando richiesto non valido\n");
			break;
		}
		bzero(input, sizeof(input));
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
