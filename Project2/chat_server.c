#include <sys/socket.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <string.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <sys/time.h>
#include <errno.h>
#define PortNumber 4418

void report(const char *msg, int exit_status){
	perror(msg);
	exit(exit_status);
}
int main(){

	int new_socket, client_socket[30], max_clients = 5, activity, valread, sd;
	int max_sd;
	char buffer[1025];
	
	fd_set readfds;
	
	for(int i = 0; i < max_clients;i++){
		client_socket[i] = 0;
	}

	int fd = socket(AF_INET,SOCK_STREAM,0);
	if(fd < 0) report("socket", 1);
	
	struct sockaddr_in saddr;
	memset(&saddr, 0 , sizeof(saddr));
	saddr.sin_family = AF_INET;
	saddr.sin_addr.s_addr = htonl(INADDR_ANY);
	saddr.sin_port = htons(PortNumber);

	if(bind(fd, (struct sockaddr *) &saddr, sizeof(saddr)) < 0){
		report("bind",1);
	}

	if(listen(fd,max_clients) < 0){
		report("listen",1);
	}
	printf("Listening on port %i for clients...\n", PortNumber);

	while(1){
		FD_ZERO(&readfds);

		FD_SET(fd, &readfds);
		max_sd = fd;

		for(int i = 0; i < max_clients; i ++){
			sd = client_socket[i];
			if(sd>0) FD_SET(sd, &readfds);
			if(sd>max_sd) max_sd = sd;
		}

		activity = select(max_sd + 1, &readfds, NULL, NULL, NULL);
		if((activity < 0) && (errno!=EINTR)){
			printf("select error");
		}
		
		struct sockaddr_in caddr;
                socklen_t len = sizeof(caddr);
		if(FD_ISSET(fd, &readfds)){
			if((new_socket = accept(fd, (struct sockaddr*)&caddr, &len))<0){
				report("accept",1);
			}

			printf("New connection:\nsocketfd - %d\nip - %s\nport - %d\n\n", new_socket, inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port));
			
			for(int i = 0; i < max_clients;i++){
				if(client_socket[i] == 0){
					client_socket[i] = new_socket;
					printf("Adding to list of sockets at %d\n", i);
					break;
				}
			}
		}
		
		for(int i = 0; i < max_clients; i++){
			sd = client_socket[i];
			printf("Inside second loop");
			if(sd>0 && FD_ISSET(sd,&readfds)){
				if((valread = recv(sd, buffer, sizeof(buffer), 0)) == 0){
					getpeername(sd,(struct sockaddr*)&caddr, &len);
					printf("Host disconected:\nip - %s\nport - %d\n\n", inet_ntoa(caddr.sin_addr), ntohs(caddr.sin_port));
					 close(sd);
					 client_socket[i] = 0;
				}
				else{
					buffer[valread] = '\0';
					for(int j = 0; j < max_clients; j++){
						if(client_socket[j] != 0 && client_socket[j] != sd){
							send(client_socket[j], buffer, strlen(buffer), 0);
						}
					}
				}
			}
		}

		/*
		char username[10];
		char message[1000];
		ssize_t bytesRead =  read(client_fd, username, sizeof(username));
		if(bytesRead<0){
			report("read", 0);
			close(client_fd);
			continue;
		}
		ssize_t bytesRead =  read(client_fd, message, sizeof(message));
                if(bytesRead<0){
                        report("read", 0);
                        close(client_fd);
                        continue;
                }
		fprintf(fp, "%s: %s\n", username, message);
		printf("Received from client %d\n%s: %s\n", client_fd, username, message);
		
		ssize_t bytesWrite = write(client_fd, buffer, bytesRead);
		if(bytesWrite<0){
			report("write", 0);
		}
		*/
	}

		return 0;	
}
