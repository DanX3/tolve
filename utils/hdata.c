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

char* hdata2string(hdata_t *val) {
	if (val != 0) {
		char* toRet = calloc(SL, sizeof(char));
		sprintf(toRet, "%s:%s:%s", val->uname, val->fullname, val->email);
		return toRet; 
	} else {
		return '\0';
	}
}

hdata_t* getDataFrom(char* username, hash_t H) {
	return (hdata_t*)((*(H+hashfunc(username)))->successivo)->elemento;
}

void saveHashInUserfile(hash_t H) {
	FILE *userFile = fopen(userfilePath, "w");
	char* buffer = calloc(SL, sizeof(char));
	int i;
	for (i=0; i<HL; i++) {
		//Metodo per arrivare al contenuto della cella hash
		hdata_t* user= (hdata_t*)((*(H+i))->successivo)->elemento;
		if ( user != 0 ) 
			fprintf(userFile, "%s\n", hdata2string(user));
	}
}



void loadUserfileInHash(hash_t H, char* userfilePathArg){
	userfilePath = strdup(userfilePathArg);
	printf("%s\n", userfilePath);
	FILE *userFile = fopen(userfilePathArg, "r");
	char* buffer = calloc(SL, sizeof(char));
	hdata_t* userData;
	size_t len = SL;
	while (getline(&buffer, &len, userFile) >= 0) {
		userData = string2hdata(buffer);
		printf("Loaded %s in hash table\n", userData->uname);
		INSERISCIHASH(userData->uname, (void*)userData, H);
		userData = malloc(sizeof(hdata_t));
	}
}

