#include <stdio.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>

struct sockaddr_in* createIPv4Address(char *ip, int port);

int createTCPIpv4Socket(){
        return socket(AF_INET, SOCK_STREAM,0);
}

struct sockaddr_in* createIPv4Address(char *ip, int port){

        struct sockaddr_in *address = malloc(sizeof(struct sockaddr_in));
        address->sin_family = AF_INET;
        address->sin_port = htons(port);

        if(strlen(ip) == 0)
                address->sin_addr.s_addr = INADDR_ANY;
        else
                inet_pton(AF_INET,ip,&address->sin_addr.s_addr);
        return address;
}

struct AcceptedSocket *acceptIncomingConnection(int serverSocketFD);
struct AcceptedSocket{
	int acceptedSocketFD;
	struct sockaddr_in address;
	int error;
	bool acceptedSuccessfully;
};

struct AcceptedSocket *acceptIncomingConnection(int serverSocketFD){
	struct sockaddr_in clientAddress;
        int clientAddressSize = sizeof(clientAddress);
        int clientSocketFD = accept(serverSocketFD,(struct sockaddr*)&clientAddress,&clientAddressSize);
	struct AcceptedSocket* acceptedSocket = malloc(sizeof(struct AcceptedSocket));
	acceptedSocket->address = clientAddress;
	acceptedSocket->acceptedSocketFD = clientSocketFD;
	acceptedSocket->acceptedSuccessfully = clientSocketFD>0;

	if(!acceptedSocket->acceptedSuccessfully)
		acceptedSocket->error = clientSocketFD;

	return acceptedSocket;
}

void receiveAndPrintIncomingData(int socketFD);
void receiveAndPrintIncomingData(int socketFD){
	
	char buffer[1024];

        while(1){
                ssize_t amountReceived = recv(clientSocketFD->acceptedSocketFD, buffer, 1024, 0);

                if(amountReceived>0){
                        buffer[amountReceived]=0;
                        printf("Response was %s\n", buffer);

			sendReceivedMessageTotheOtherClients(buffer,socketFD);
                }
                if(amountReceived<0)
                        break;
        }

	 close(clientSocket->acceptedSocketFD);
}

void sendReceivedMessageToTheOtherClients(char *buffer, int socketFD);
void sendReceivedMessageToTheOtherClients(char *buffer, int socketFD){
	for(int i = 0; i < acceptedSocketsCount;i++){
		if(acceptedSockets[i].acceptedSocketFD != socketFD){
			send(acceptedSockets[i].acceptedSocketFD, buffer, strlen(buffer), 0);
		}
	}
}
void startAcceptingIncomingConnections(int serverSocketFD);
void startAcceptingIncomingConnections(int serverSocketFD){
	
	while(1){
                struct AcceptedSocket* clientSocket = acceptIncomingConnection(serverSocketFD);
                acceptedSockets[acceptedSocketsCount++] = *clientSocket;
		receiveAndPrintIncomingDataOnSeperateThread(clientSocket);
		
	}

}

void receiveAndPrintIncomingDataOnSeparateThread(struct AcceptedSocket *pSocket);
void receiveAndPrintIncomingDataOnSeparateThread(struct AcceptedSocket *pSocket){
	pthread_t id();
	pthread_create(&id,NULL,receiveAndPrintIncomingData,pSocket->acceptedSocketFD)

}

struct AcceptedSocket acceptedSockets[10];
int acceptedSocketsCount = 0;

int main(nt argc, char *argv[]){
	
	int serverSocketFD = createTCPIpv4Socket();
	struct sockaddr_in *serverAddress = createIPv4Address("",argv[1]);
	
	int result = bind(serverSocketFD, (struct sockaddr*)serverAddress, sizeof(*serverAddress));
	if(result == 0)
		printf("Socket was bound\n");

	int listenResult = listen(serverSocketFD,10);
	
	startAcceptingIncomingConnections(serverSocketFD);
	
	shutdown(serverSocketFD,SHUT_RDWR);

	return 0;
}
