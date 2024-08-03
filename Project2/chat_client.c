#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/tcp.h>

#define Host "localhost"
#define PortNumber 4418

void report(const char *msg, int exit_status){
        perror(msg);
        exit(exit_status);
}

int main(){	
	
	int sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) report("socket",1);
	
	struct hostent* hptr = gethostbyname(Host);
	if(!hptr) report("gethostbyname", 1);
	if(hptr->h_addrtype != AF_INET) report("bad address family", 1);
	
	struct sockaddr_in saddr;
	memset(&saddr, 0, sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = ((struct in_addr*) hptr->h_addr_list[0])->s_addr;
	saddr.sin_port = htons(PortNumber);
	if(connect(sockfd, (struct sockaddr*) &saddr, sizeof(saddr)) < 0) report("connect", 1);

	char message[1025];
	
	while(1){
		printf("Enter message: ");
		fgets(message,sizeof(message),stdin);
		message[strcspn(message,"\n")] = '\0';

                send(sockfd, message, strlen(message), 0);
		
		ssize_t bytesRead = recv(sockfd, message, strlen(message)-1,0);	
		
		if(bytesRead <= 0){
			printf("Server disconnected.\n");
			break;
		}
		
		message[bytesRead]='\0';
		printf("Recieved: %s\n", message);
	}

	/*
	printf("--Enter your message: "); 
	scanf(" %[^\n]", message);
	
	write(sockfd, username, strlen(username));
	write(sockfd, message, strlen(message));

	char buffer[1000];
	ssize_t bytesRead = read(sockfd, buffer, sizeof(buffer));
	if(bytesRead < 0){
		report("read",1);
	}
	printf("Received from server: %.*s\n", (int)bytesRead, buffer);
	*/
	close(sockfd);
	return 0;
}
