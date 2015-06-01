#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>

#define SL 256 // string length
#define MSG_LOGIN	'L' //login
#define MSG_REGLOG 	'R' //register and login
#define MSG_OK		'O' //OK
#define MSG_ERROR 	'E' //error
#define MSG_SINGLE 	'S' //message to single user
#define MSG_BRDCAST	'B' //message broadcast
#define MSG_LIST	'I' //list 
#define MSG_LOGOUT 	'X' //logout
#define CMD_LOGOUT 	"#x"
#define CMD_LIST 	"#ls"
#define CMD_SEND 	"#dest"
#define HELP_PATH 	"help.txt"
#define DELIM_CHAR	":"
#define MAX_LOGGEDUSERS 256
#define USERNAME_LENGTH 32

#define LOGIN_DONE_YET		"Utente gia' connesso"
#define USER_NOT_REGISTERED	"Utente non registrato"
#define USER_REGISTERED_YET	"Utente gia' registrato"
#define HASH_COLLISION		"Collisione nella Hash Table"
#define RECV_NOT_REGISTERED	"Destinatario inesistente"
#define RECV_OFFLINE		"Destinatario non connesso"
#define CMD_NOT_FOUND		"Comando non trovato\n"
#define SHORT_HELP			"./chat-client -h for help\n"

#define max 2000
#define BUFFER_SIZE 1000

typedef char** StringList;

typedef struct struct_hdata_t {
	char * uname; // username
	char * fullname; // full name
	char * email; // email address
	int sockid; // socket identifier; -1 = not connected
} hdata_t;

typedef struct __msg_t__ {
	char type;
	char *sender;
	char *receiver;
	unsigned int msglen;
	char *content;
} msg_t;

typedef struct {
	char buffer[BUFFER_SIZE];
	pthread_mutex_t M;
	int readpos, writepos;
	int cont;
	pthread_cond_t PIENO;
	pthread_cond_t VUOTO;
} RingBuffer;

typedef struct cella * lista;
typedef lista          posizione;

struct cella {
  posizione precedente;
  void *    elemento;
  posizione successivo;
};

typedef lista * hash_t;