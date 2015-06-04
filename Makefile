CC=gcc
CFLAGS= -Wall

chat: chat-server chat-client 
	
install:
	mv chat-server bin/
	mv chat-client bin/

chat-server: chat-server.o utils/*.o
	$(CC) $(CFLAGS) -o chat-server chat-server.o src/utils/*.o -lpthread

chat-server.o:
	$(CC) -c src/chat-server.c

chat-client:chat-client.o utils/*.o
	$(CC) $(CFLAGS) -o chat-client chat-client.o src/utils/*.o -lpthread

chat-client.o:
	$(CC) -c src/chat-client.c

utils/*.o: src/utils/*.c
	$(CC) -c src/utils/*.c
	mv log.o src/utils/
	mv stringList.o src/utils/
	mv hdata.o src/utils/
	mv message.o src/utils/
	mv ringBuffer.o src/utils/
	
clean:	
	rm *.o
	rm src/utils/*.o
