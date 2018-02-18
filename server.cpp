#include <iostream>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <pthread.h>
#include <math.h>
#include <vector>
#include<fcntl.h>

using namespace std;

#define connects 1000
#define bufSize 1024


void start(int &server);
void *request(void *parameters);

int main(int argc, char const *argv[]){
	vector<pthread_t> pool; 
	//int bufSize = 1024; 
	char buffer[bufSize]; 
	int yes = 1;
	int connections = 0;
	int server, client; // socket file descriptors
	int portNum = 8080; // port number
	bool isExit = true; // var fo continue infinitly
	struct sockaddr_in server_addr;
	socklen_t size;
	start(server);
	server_addr.sin_family = AF_INET;
	server_addr.sin_addr.s_addr = htons(INADDR_ANY);
	server_addr.sin_port = htons(portNum);

	
	if (setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
		perror("setsockopt");
		exit(1);
	}
	
	if ((bind(server, (struct sockaddr*) &server_addr, sizeof(server_addr))) < 0) {
		cout << "- Error binding connection, the socket has already been established..." << endl;
		exit(-1);
	}

	size = sizeof(server_addr);
	cout << "- Looking for clients..." << endl;
	listen(server, 1);

	
	while (1) {
		client = accept(server, (struct sockaddr *) &server_addr, &size);
		pthread_t thread;
		//recv(client, buffer, bufSize, 0);
		if(client){
			pthread_create(&thread, NULL, request, (void*) &client);
			pool.push_back(thread);
			connections++;
		}

		if (client < 0){
			cout << "- Error on connecting..." << endl;
		}
	}

	close(server);
	return 0;
}

void start(int &server){
	cout << "\n- Starting server..." << endl;
	server = socket(AF_INET, SOCK_STREAM, 0);
	if (server < 0) {
		cout << "Error establishing socket ..." << endl;
		exit(-1);
	}

	cout << "- Socket server has been created..." << endl;
}

void *request(void *client){
	//int bufSize = 1024; 
	//char buffer[bufSize]; 
	//int connection = *((int*) client);
	//strcpy(buffer, "\n-> Welcome to echo server...\n");
	//send(connection, buffer, bufSize, 0);
	//cout << "Se conecto un cliente" <<endl;

	int n = *((int*) client);
	char mesg[99999], *reqline[3], data_to_send[bufSize], path[99999];
	int rcvd, fd, bytes_read;
	//memset( (void*)mesg, (int)'\0', 99999 );
	rcvd=recv(n, mesg, 99999, 0);
	
	if (rcvd<0)    // receive error
		fprintf(stderr,("recv() error\n"));
	else if (rcvd==0)    // receive socket closed
		fprintf(stderr,"Client disconnected unexpectedly.\n");
	else    // message received
	{
		cout << "1" <<endl;
		printf("%s", mesg);
		reqline[0] = strtok (mesg, " \t\n");
		if ( strncmp(reqline[0], "GET\0", 4)==0 )
		{
			reqline[1] = strtok (NULL, " \t");
			reqline[2] = strtok (NULL, " \t\n");
			if ( strncmp( reqline[2], "HTTP/1.0", 8)!=0 && strncmp( reqline[2], "HTTP/1.1", 8)!=0 )
			{
				write(n, "HTTP/1.0 400 Bad Request\n", 25);
			}
			else
			{
				if ( strncmp(reqline[1], "/\0", 2)==0 )
					reqline[1] = "/index.html";        //Because if no file is specified, index.html will be opened by default (like it happens in APACHE...

				strcpy(path, "./public");
				strcpy(&path[strlen("./public")], reqline[1]);
				printf("file: %s\n", path);

				if ( (fd=open(path, O_RDONLY))!=-1 )    //FILE FOUND
				{
					send(n, "HTTP/1.0 200 OK\n\n", 17, 0);
					while ( (bytes_read=read(fd, data_to_send, bufSize))>0 )
						write (n, data_to_send, bytes_read);
				}
				else    write(n, "HTTP/1.0 404 Not Found\n", 23); //FILE NOT FOUND
			}
		}
	}
	//Closing SOCKET
	shutdown (n, SHUT_RDWR);         //All further send and recieve operations are DISABLED...
	close(n);
	n=-1;
}