//Return a char[24] as a timestamp
char* timestamp() {
	time_t t = time(0);
	return strtok(ctime(&t), "\n"); //Delete the newLine char from timestamp
}

int cmdMatcher(char* cmd) {
	//leggo quale comando si vuole 2eseguire, modificando un altra istanza
	//di cmd, per non intaccare la stringa originale
	if (strncmp(cmd, CMD_LIST, strlen(CMD_LIST)) == 0 )	return MSG_LIST;
	if (strncmp(cmd, CMD_LOGOUT, strlen(CMD_LOGOUT)) == 0 )	return MSG_LOGOUT;

	if (strncmp(cmd, CMD_SEND, strlen(CMD_SEND)) == 0) {
		if (cmd[6] == ':')	return MSG_BRDCAST;
		else			return MSG_SINGLE;
	}
	return 0;

}

char* marshal(msg_t *msg){
	char* toRet = calloc(SL, sizeof(char));
	sprintf(toRet, "%c:%s:%s:%u:%s\n", 
		msg->type, msg->sender, msg->receiver, msg->msglen, msg->content);
	return toRet;

}

msg_t* unMarshal(char* str){
	msg_t* toRet 	= calloc(1, sizeof(msg_t));
	char* backup = strdup(str);
	str = strtok(str, DELIM_CHAR);
	toRet->type 	= str[0];
	toRet->sender 	= strdup(strtok(0, DELIM_CHAR));
	toRet->receiver = strdup(strtok(0, DELIM_CHAR));
	toRet->msglen 	= atoi  (strtok(0, DELIM_CHAR));
	toRet->content  = strdup(strtok(0, "\n"));
}

void signalHandler(int signum) {
	printf("Server interrupted: signal (%d) received\n", signum);
	exit (signum);
}
