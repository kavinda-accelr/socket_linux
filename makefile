CC=g++
CFLAGS=-std=c++11 -g

client: client.cpp
	$(CC) $(CFLAGS) -c client.cpp -o client.o
	$(CC) $(CFLAGS) -o client.out client.o

server: server.cpp
	$(CC) $(CFLAGS) -c server.cpp -o server.o
	$(CC) $(CFLAGS) -o server.out server.o

run_client: client
	./client.out

run_server: server
	./server.out

clean:
	rm -f *.o *.out
