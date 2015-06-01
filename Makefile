CC=gcc

all: chat-server chat-client

chat-server: chat-server.o utils/*.o
	$(CC) -o chat-server chat-server.o utils/*.o -lpthread

chat-server.o:chat-server.c
	$(CC) -c chat-server.c

chat-client:chat-client.o utils/*.o
	$(CC) -o chat-client chat-client.o utils/*.o -lpthread

chat-client.o:chat-client.c
	$(CC) -c chat-client.c

utils/*.o: utils/*.c
	$(CC) -c *utils/*.c
	mv log.o utils/
	mv stringList.o utils/
	mv misc.o utils/
	mv hdata.o utils/
	mv hash.o utils/
	mv message.o utils/
	mv ringBuffer.o utils/

utils/*.c:

clean:
	rm *.o
	rm utils/*.o
