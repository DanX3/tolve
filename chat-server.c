#include "utils.h"

int go = 1;
int activeThreads = 0;
char* ringBuffer;
int readingPoint, writingPoint;

void Server(void);
void* Dispatcher(void*);
void* Worker(void*);

pthread_mutex_t logfileMutex;
pthread_mutex_t activeThreadsMutex;
hash_t H;
StringList loggedList;
int sock, newSocket;

void signalHandler(int signum) {
	saveHashInUserfile(H);
	close(sock);
	printf("Server interrupted: signal (%d) received\n", signum);
	exit(0);
}

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
	ringBuffer = calloc(256*SL, sizeof(char));
	readingPoint = 0;
	writingPoint = 0;
	signal(SIGTERM, signalHandler);
	signal(SIGINT, signalHandler);

	//Organizzazione socket server-side
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
	}
}

void*  Dispatcher(void* data) {

}

void*  Worker(void* data) {
	int socket = *(int*)data;
	char *username;
	char *input = calloc(SL, sizeof(char));
	msg_t* msg = calloc(1, sizeof(msg_t));
	username = calloc(SL, sizeof(char));

	//Apertura del canale con il thread dispatcher


	while (go) {
		if ( read(socket, input, SL) == 0 )
			exit(1);
		msg = unMarshal(input);
		switch(msg->type) {
		case MSG_LOGIN:
			username = msg->content;
			strcpy(username, msg->content);
			bzero(msg, sizeof(msg_t));
			if ( checkLoggedUser(username, loggedList) ) {
				SCError("Errore: hai gia' effettuato l'accesso", msg);
				writeErrorToLog("utente gia' connesso", username);
				write(socket, marshal(msg), SL);
				pthread_exit(0);
			}

			if ( CERCAHASH(username, H) == 0 ) {
				SCError("Errore: utente non registrato", msg);
				writeErrorToLog("utente non registrato", username);
				write(socket, marshal(msg), SL);
				pthread_exit(0);
			} 

			SCOK(msg);
			write(socket, marshal(msg), SL);
			addLoggedUser(username, loggedList);
			getDataFrom(username, H)->sockid = socket;
			writeAccessToLog(1, username);

			pthread_mutex_lock(&activeThreadsMutex);
			activeThreads++;
			pthread_mutex_unlock(&activeThreadsMutex);
			break;
		case MSG_REGLOG: {
			hdata_t *userInfo = string2hdata(msg->content);
			if ( INSERISCIHASH(userInfo->uname, userInfo, H) == 0 ) {
				SCError("Errore: collisione nella hash table o utente gia' registrato", msg);
				writeErrorToLog("collisione nella hash table o utente gia' registrato", username);
			} else 
				SCOK(msg);

			write(socket, marshal(msg), SL);
			break;
		}
		case MSG_SINGLE:
			if ( CERCAHASH(msg->receiver, H) == 0 ) {
				SCError("Errore: destinatario non registrato", msg);
				writeErrorToLog("destinatario non registrato", username);

				write(socket, marshal(msg), SL);
			} else
			if ( checkLoggedUser(msg->receiver, loggedList) == 0) {
				SCError("Errore: destinatario non connesso", msg);
				writeErrorToLog("destinatario non connesso", username);
				write(socket, marshal(msg), SL);
			} else {
				pthread_mutex_lock(&logfileMutex);
				writeMessageToLog(username,  msg->receiver, msg->content);
				pthread_mutex_unlock(&logfileMutex);

				int recvSock = getDataFrom(msg->receiver, H)->sockid;
				SCSingle(username, msg->content, msg);
				write(recvSock, marshal(msg), SL);

			}
			break;
		case MSG_BRDCAST: {
			SCBroadcast(username, msg->content, msg);
			int usersWritten = 0, i = -1;
			char *currentUser;
			while (usersWritten < activeThreads - 1) {
				i++;
				if (!existsUserAt(i, loggedList))		continue;
				currentUser = getUserAt(i, loggedList);
				if ( strcmp( currentUser, username) == 0 )	continue;

				//If the receiver is correct, send to him the message
				pthread_mutex_lock(&logfileMutex);
				writeMessageToLog(username,  msg->receiver, msg->content);
				pthread_mutex_unlock(&logfileMutex);

				write( getDataFrom(currentUser, H)->sockid, marshal(msg), SL);
				usersWritten++;
			}
			break;
		}
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

			pthread_mutex_lock(&activeThreadsMutex);
			activeThreads--;
			pthread_mutex_unlock(&activeThreadsMutex);

			pthread_exit(0);
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
