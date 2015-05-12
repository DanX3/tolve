void createMessage(char type, char *sender, char *receiver, char *content, msg_t* msg) {
	if (msg != 0)
		bzero(msg, sizeof(msg));
	else
		msg = calloc(1, sizeof(msg_t));
	
	msg->type = type;
	msg->sender = sender;
	msg->receiver = receiver;
	msg->content = content;
	if (content != 0)
		msg->msglen = strlen(content);
}

void CSLogin(char *username, msg_t* msg) {
	createMessage(MSG_LOGIN, 0, 0, username, msg);
}

void CSRelog(char* username, char* fullname, char* email, msg_t* msg) {
	char* userInfo = calloc(SL, sizeof(char));
	sprintf(userInfo, "%s:%s:%s", username, fullname, email);
	createMessage(MSG_REGLOG, 0, 0, userInfo, msg);
}

void CSList(msg_t* msg) {
	createMessage(MSG_LIST, 0, 0, 0, msg);
}

void CSSingle(char* cmd, msg_t* msg) {
	strtok(cmd, " ");
	char* username = strdup(strtok(0, DELIM_CHAR));
	char* content  = strdup(strtok(0, "\n"));
	createMessage(MSG_SINGLE, 0, username, content, msg);
}

void CSLogout(msg_t* msg) {
	createMessage(MSG_LOGOUT, 0, 0, 0, msg);
}

void CSBroadcast(char* content, msg_t* msg){
	strtok(content, DELIM_CHAR);
	createMessage(MSG_BRDCAST, 0, 0, strtok(0, "\n"), msg);
}

void SCError(char* errorMessage, msg_t* msg) {
	createMessage(MSG_ERROR, 0, 0, errorMessage, msg);
}

void SCOK(msg_t* msg) {
	createMessage(MSG_OK, 0, 0, 0, msg);
}

void SCSingle(char* sender, char* content, msg_t* msg) {
	createMessage(MSG_SINGLE, sender, 0, content, msg);
}

void SCBroadcast(char* sender, char* content, msg_t* msg) {
	createMessage(MSG_BRDCAST, sender, 0, content, msg);
}

SCList(char* loggedList, msg_t* msg) {
	createMessage(MSG_LIST, 0, 0, loggedList, msg);
}
