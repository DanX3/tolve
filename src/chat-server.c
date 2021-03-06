#include "utils/common.h"
#include "utils.h"
int go = 1;
int activeThreads = 0;
RingBuffer* ringBuffer;		//Tipo di dato per la gestione del buffer circolare

void Server(char*, char*);
void* Dispatcher(void*);
void* Worker(void*);

pthread_mutex_t logfileMutex;
pthread_mutex_t activeThreadsMutex;
hash_t H;			//Tipo di dato hash table
StringList loggedList;		//Tipo di dato per la gestione dei login
int sock, newSocket;

void signalHandler(int signum) {
	saveHashInUserfile(H);
	close(sock);
	free(ringBuffer);
	DISTRUGGIHASH(H);
	printf("Server interrupted: signal (%d) received\n", signum);
	exit(0);
}

void Server(char* userfile, char* logfile){
	printf("server pid: %d\n", getpid());

	//Setting the environment
	H = CREAHASH();
	loadUserfileInHash(H, userfile);
	pthread_mutex_init(&logfileMutex, NULL);
	initLog(logfile);
	loggedList = initLoggedUser();
	ringBuffer = malloc(sizeof(RingBuffer));
	initRingBuffer(ringBuffer);
	signal(SIGTERM, signalHandler);
	signal(SIGINT, signalHandler);

	//socket server-side organization 
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
#if 1
	pthread_t dispatcher;
	pthread_create(&dispatcher, 0, Dispatcher, 0);
#endif
	while(go){
		newSocket = accept(sock, NULL, 0);
		pthread_t t;
		pthread_create(&t, 0, Worker, (void*)&newSocket);
		pthread_detach(t);
	}
}

void* Dispatcher(void* data) {
	char* stringMessage = calloc(SL, sizeof(char));
	msg_t* msg = calloc(1, sizeof(msg_t));
	while (go) {
		msg = unMarshal(readBuffer(ringBuffer));
		switch(msg->type) {
		case MSG_SINGLE:
			freeWrite(getDataFrom(msg->receiver, H)->sockid, msg, SL);
			break;
		case MSG_BRDCAST: {
			int usersWritten = 0, i = -1;
			char *currentUser = calloc(USERNAME_LENGTH, sizeof(char));
			while (usersWritten < activeThreads - 1) {
				i++;
				if (!existsUserAt(i, loggedList))		continue;
				bzero(currentUser, sizeof(currentUser));
				getUserAt(i, loggedList, currentUser);
				if ( strcmp( currentUser, msg->sender) == 0 )	continue;

				//If the receiver is correct, send to him the message
				pthread_mutex_lock(&logfileMutex);
				writeMessageToLog(	msg->sender,
							getDataFrom(currentUser, H)->uname,
							msg->content);
				pthread_mutex_unlock(&logfileMutex);

				freeWrite( getDataFrom(currentUser, H)->sockid, msg, SL);
				usersWritten++;
			}
			break;
		}
		}
	}
	free(stringMessage);
	free(msg);
}

void*  Worker(void* data) {
	int socket = *(int*)data;
	char *input = calloc(SL, sizeof(char));
	msg_t* msg;
	char *username = calloc(SL, sizeof(char));


	while (go) {
		bzero(input, sizeof(input));
		if ( read(socket, input, SL) == 0 )
			exit(1);
		msg = unMarshal(input);
		switch(msg->type) {
		case MSG_LOGIN:
			strcpy(username, msg->content);
			if ( checkLoggedUser(username, loggedList) ) {
				SCError(LOGIN_DONE_YET, msg);
				writeErrorToLog(LOGIN_DONE_YET, username);
				freeWrite(socket, msg, SL);
				pthread_exit(0);
			}

			if ( CERCAHASH(username, H) == 0 ) {
				SCError(USER_NOT_REGISTERED, msg);
				writeErrorToLog(USER_NOT_REGISTERED, username);
				freeWrite(socket, msg, SL);
				pthread_exit(0);
			}

			SCOK(msg);
			freeWrite(socket, msg, SL);
			addLoggedUser(username, loggedList);
			getDataFrom(username, H)->sockid = socket;
			writeAccessToLog(1, username);

			pthread_mutex_lock(&activeThreadsMutex);
			activeThreads++;
			pthread_mutex_unlock(&activeThreadsMutex);
			break;
		case MSG_REGLOG: {
			hdata_t *userInfo = string2hdata(msg->content);
			if ( strlen(userInfo->uname) > 31) {
				SCError(USER_TOO_LONG, msg);
				writeErrorToLog(USER_TOO_LONG, userInfo->uname);
				freeWrite(socket, msg, SL);
				pthread_exit(0);
			}


			if ( CERCAHASH(userInfo->uname, H) != 0 ) {
				SCError(USER_REGISTERED_YET, msg);
				writeErrorToLog(USER_REGISTERED_YET, userInfo->uname);
				freeWrite(socket, msg, SL);
				pthread_exit(0);
			}

			if ( INSERISCIHASH(userInfo->uname, userInfo, H) == 0 ){
				SCError(HASH_COLLISION, msg);
				writeErrorToLog(HASH_COLLISION, userInfo->uname);
				freeWrite(socket, msg, SL);
				pthread_exit(0);
			}

			SCOK(msg);
			freeWrite(socket, msg, SL);
			break;
		}
		case MSG_SINGLE:
			if ( CERCAHASH(msg->receiver, H) == 0 ) {
				SCError(RECV_NOT_REGISTERED, msg);
				pthread_mutex_lock(&activeThreadsMutex);
				writeErrorToLog(RECV_NOT_REGISTERED, username);
				pthread_mutex_unlock(&activeThreadsMutex);

				freeWrite(socket, msg, SL);
				//write(socket, marshal(msg), SL);
			} else
			if ( checkLoggedUser(msg->receiver, loggedList) == 0) {
				SCError(RECV_OFFLINE, msg);
				pthread_mutex_lock(&activeThreadsMutex);
				writeErrorToLog(RECV_OFFLINE, username);
				pthread_mutex_unlock(&activeThreadsMutex);
				//write(socket, marshal(msg), SL);
				freeWrite(socket, msg, SL);
			} else {
				msg->sender = username;
				char* marshalled = calloc(SL, sizeof(char));
				marshalDirect(msg, marshalled);
				writeBuffer(marshalled, ringBuffer);
				free(marshalled);

				pthread_mutex_lock(&logfileMutex);
				writeMessageToLog(username,  msg->receiver, msg->content);
				pthread_mutex_unlock(&logfileMutex);

			}
			break;
		case MSG_BRDCAST:
			SCBroadcast(username, msg->content, msg);
			char* marshalled = calloc(SL, sizeof(char));
			marshalDirect(msg, marshalled);
			writeBuffer(marshalled, ringBuffer);
			free(marshalled);
			break;
		case MSG_LIST:
			SCList(listLoggedUser(loggedList, activeThreads), msg);
			freeWrite(socket, msg, SL);
			break;
		case MSG_LOGOUT:
			removeLoggedUser(username, loggedList);
			getDataFrom(username, H)->sockid = -1;
			pthread_mutex_lock(&activeThreadsMutex);
			writeAccessToLog(0, username);
			pthread_mutex_unlock(&activeThreadsMutex);
			free(input);
			free(msg);
			free(username);

			pthread_mutex_lock(&activeThreadsMutex);
			activeThreads--;
			pthread_mutex_unlock(&activeThreadsMutex);

			pthread_exit(0);
		}
		free(msg);
	}
}



int main(int argc, char** argv) {
	pid_t p;
	p = fork();
	if(p == 0) {
		signal(SIGTERM, signalHandler);
		Server(argv[1], argv[2]);
	}
	else if(p < 0)
		printf("fork fallita");
	exit(0);
}
