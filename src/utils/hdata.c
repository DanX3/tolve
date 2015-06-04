#include "hash.c"
char* userfilePath;

hdata_t* string2hdata(char *str) {
	hdata_t* toRet = malloc(sizeof(hdata_t));
	str = strtok(str, "\n");
	toRet->uname = strdup(strtok(str, DELIM_CHAR));
	toRet->fullname = strdup(strtok(NULL, DELIM_CHAR));
	toRet->email = strdup(strtok(NULL, DELIM_CHAR));
	toRet->sockid = -1;
	return toRet;
}

void hdata2string(hdata_t *val, char* ret) {
	if (val != 0)
		sprintf(ret, "%s:%s:%s", val->uname, val->fullname, val->email);
}

hdata_t* getDataFrom(char* username, hash_t H) {
	return (hdata_t*)((*(H+hashfunc(username)))->successivo)->elemento;
}

void saveHashInUserfile(hash_t H) {
	FILE *userFile = fopen(userfilePath, "w");
	int i;
	for (i=0; i<HL; i++) {
		//Metodo per arrivare al contenuto della cella hash
		hdata_t* user= (hdata_t*)((*(H+i))->successivo)->elemento;
		if ( user != 0 ) {
			char* userData = calloc(SL, sizeof(char));
			hdata2string(user, userData);
			fprintf(userFile, "%s\n", userData);
			free(userData);
		}
	}
	fclose(userFile);
}



void loadUserfileInHash(hash_t H, char* userfilePathArg){
	userfilePath = strdup(userfilePathArg);
	FILE *userFile = fopen(userfilePathArg, "r");

	//Crea il file se non esiste ancora
	if (userFile == 0) {
		FILE *userFile = fopen(userfilePathArg, "w");
		fclose(userFile);
		return;
	}

	char* buffer = calloc(SL, sizeof(char));
	hdata_t* userData;
	size_t len = SL;
	while (getline(&buffer, &len, userFile) >= 0) {
		userData = string2hdata(buffer);
		INSERISCIHASH(userData->uname, (void*)userData, H);
		userData = malloc(sizeof(hdata_t));
	}
	free(buffer);
	fclose(userFile);
}
