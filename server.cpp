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


using namespace std;



void start(int &server);
void *request(void *parameters);

int main(int argc, char const *argv[]){
	vector<pthread_t> pool; 
	int bufSize = 1024; 
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

	
	while (client = accept(server, (struct sockaddr *) &server_addr, &size)) {
		pthread_t thread;
		recv(client, buffer, bufSize, 0);
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
	int bufSize = 1024; 
	char buffer[bufSize]; 
	int connection = *((int*) client);
	strcpy(buffer, "\n-> Welcome to echo server...\n");
	send(connection, buffer, bufSize, 0);
	cout << "Se conecto un cliente" <<endl;
}