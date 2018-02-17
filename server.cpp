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


using namespace std;



void start(int &server);
void *request(void *parameters);

int main(int argc, char const *argv[]){
	int yes = 1;
	int counter = 1;
	int server, client; // socket file descriptors
	int portNum = 8080; // port number
	int bufSize = 1024; // buffer size
	char buffer[bufSize]; // buffer to transmit
	bool isExit = false; // var fo continue infinitly
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
	client = accept(server, (struct sockaddr *) &server_addr, &size);

	cout << client << endl;

	if (client < 0){
		cout << "- Error on accepting..." << endl;
	}

	string echo;
	while (client > 0) {
		strcpy(buffer, "\n-> Welcome to echo server...\n");
		send(client, buffer, bufSize, 0);
		cout << "- Connected with the client, waiting for data..." << endl;
		do {
			cout << "\nClient: ";
			echo = "";
			do {
				recv(client, buffer, bufSize, 0);
				cout << buffer << " ";
				if (*buffer == '#') {
					isExit = true;
					*buffer = '*';
					echo = "Goodbye!";
				} else if ((*buffer != '#') && (*buffer != '*')) {
					echo += buffer;
					echo += " ";
				}
				cout << echo.c_str() << endl;
			} while (*buffer != '*');

			// sprintf(buffer, "%s", echo.c_str());
			// send(client, buffer, bufSize, 0);
		} while (!isExit);
		cout << "\n\n=> Connection terminated with IP " << inet_ntoa(server_addr.sin_addr);
		close(client);
		cout << "\nGoodbye..." << endl;
		exit(1);
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

void *request(void *parameters){

}