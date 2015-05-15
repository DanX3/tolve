#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <time.h>
#include <signal.h>
#include "hash.h" //!Keep this for including string stdio and stdlib

#define DELIM_CHAR ":"
#define USERFILE_NAME "user-file"
#define LOGFILE_NAME "log-file"
#define MAX_LOGGEDUSERS 256
#define USERNAME_LENGTH 32

typedef char** StringList;

//misc
char* timestamp(void);			//Restituisce il timestamp attuale
int cmdMatcher(char*);			//restituisce il comando richiesto
char* marshal(msg_t*);			//effettua il marshalling di un msg_t
msg_t* unMarshal(char*);		//effettua il parsing di una stringa con dati msg_t

//hdata
hdata_t* string2hdata(char*);		//effettua il parse di una stringa con dati hdata_t
char*  hdata2string(hdata_t*);		//converte i dati utente in una stringa con separatori
hdata_t* getDataFrom(char*, hash_t);	//ritorna il puntatore ai dati nella hash table
void loadUserfileInHash(hash_t);	//carica la hash table da un file di testo
void saveHashInUserfile(hash_t);	//salva l'attuale hash table in un file di testo

//log
void initLog();				//inizializza il log-file
char* writeAccess(int, char*);		//scrive l'accesso nel log-file
char* writeMessage(char*, char*, char*);//scrive un messaggio su log-file

//stringList
StringList initLoggedUser();		//inizializza la lista di utenti connessi
void addLoggedUser(char*, StringList);	//aggiunge un utente alla lista
char* listLoggedUser(StringList);	//elenca gli utenti connessi restituendoli in una stringa
int checkLoggedUser(char*, StringList);	//controlla se un utente e' connesso
void removeLoggedUser(char*, StringList);//rimuove un utente dalla lista degli utenti connessi

//message
void createMessage(char, char*, char*, char*, msg_t*);
void CSLogin(char*, msg_t*);		//Messaggi da Client a Server
void CSRelog(char*, char*, char*, msg_t*);//
void CSList(msg_t*);			//
void CSSingle(char*, msg_t*);		//
void CSLogout(msg_t*);			//
void CSBroadcast(char*, msg_t*);	//
void SCError(char*, msg_t*);		//Messaggi da Server a client
void SCOK(msg_t*);			//
void SCSingle(char*, char*, msg_t*);	//
void SCBroadcast(char*, char*, msg_t*);		//
void SCList(char*, msg_t*);			//

//ringBuffer
void initRingBuffer(RingBuffer*);	//Inizializza un tipo di dato RingBuffer
void writeBuffer(char*, RingBuffer*);	//Scrive un messaggio sul buffer circolare
char* readBuffer(RingBuffer*);		//Legge un messaggio da buffer circolare

#include "utils/misc.c"
#include "utils/log.c"
#include "utils/stringList.c"
#include "utils/hdata.c"
#include "utils/message.c"
#include "utils/ringBuffer.c"
