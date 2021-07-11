CC = g++
CFLAGS = -std=c++11

all: client.cpp server.cpp
	$(CC) $(CFLAGS) -D_REENTRANT server.cpp -o server -lpthread
	$(CC) $(CFLAGS) -D_REENTRANT client.cpp -o client -lpthread

clean:
	rm  client server