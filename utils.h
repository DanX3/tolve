#include <pthread.h>
#include <sys/socket.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <time.h>
#include "hash.h" //!Keep this for including string stdio and stdlib

#define DELIM_CHAR ":"
#define USERFILE_NAME "user-file"
#define LOGFILE_NAME "log-file"
#define MAX_LOGGEDUSERS 256
#define USERNAME_LENGTH 32

typedef char** StringList;

char* timestamp(void);			//Restituisce il timestamp attuale
char*  hdata2string(hdata_t*);		//converte i dati utente in una stringa con separatori
hdata_t* string2hdata(char*);		//effettua il parse di una stringa con dati hdata_t
char* writeAccess(int, char*);		//scrive l'accesso nel log-file
char* writeMessage(char*, char*, char*);//scrive un messaggio su log-file
void loadUserfileInHash(hash_t);	//carica la hash table da un file di testo
void saveHashInUserfile(hash_t);	//salva l'attuale hash table in un file di testo
hdata_t* getDataFrom(char*, hash_t);	//ritorna il puntatore ai dati nella hash table
void initLog();				//inizializza il log-file
int cmdMatcher(char*);			//restituisce il comando richiesto
char* marshal(msg_t*);			//effettua il marshalling di un msg_t
msg_t* unMarshal(char*);		//effettua il parsing di una stringa con dati msg_t
StringList initLoggedUser();		//inizializza la lista di utenti connessi
void addLoggedUser(char*, StringList);	//aggiunge un utente alla lista
char* listLoggedUser(StringList);	//elenca gli utenti connessi restituendoli in una stringa
int checkLoggedUser(char*, StringList);	//controlla se un utente e' connesso
void removeLoggedUser(char*, StringList);//rimuove un utente dalla lista degli utenti connessi


#include "misc.c"
#include "log.c"
#include "stringList.c"
#include "hdata.c"
