CC = gcc

all:
	$(CC) -o client client.c
	$(CC) -o serv1 serv1.c
	$(CC) -o serv2 serv2.c
	$(CC) -o serv3 serv3.c
	$(CC) -o serv4 serv4.c




clean:
	rm -rf serv1
	rm -rf serv2
	rm -rf serv3
	rm -rf serv4
	rm -rf client

	
