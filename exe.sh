killall server
gcc -g chat-server.c -o server -lpthread
gcc -g chat-client.c -o client -lpthread
./server
./client tiulalan
