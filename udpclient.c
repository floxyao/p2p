// UDP client program 
#include <arpa/inet.h> 
#include <netinet/in.h> 
#include <stdio.h> 
#include <stdlib.h> 
#include <strings.h> 
#include <sys/socket.h> 
#include <sys/types.h> 
#define PORT 8080 
#define MAXLINE 1024 
int main() 
{ 
	int sock, n, len; 
	char buffer[MAXLINE]; 
	char* message = "Hello from Client"; 
	struct sockaddr_in servaddr; 

	/*-------------------------------
     Creating UDP socket 
    --------------------------------*/
	if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { 
		printf("UDP socket failed"); 
		exit(0); 
	} 

	memset(&servaddr, 0, sizeof(servaddr)); 

	/*-------------------------------
     Fill server information
    --------------------------------*/
	servaddr.sin_family = AF_INET; 
	servaddr.sin_port = htons(PORT); 
	servaddr.sin_addr.s_addr = inet_addr("127.0.0.1"); 
        printf("Test\n"); 

	/*-------------------------------
     Send message back to UDP server
    --------------------------------*/
	sendto(sock, (const char*)message, strlen(message), 
		0, (const struct sockaddr*)&servaddr, 
		sizeof(servaddr)); 

    /*-------------------------------
     Wait for message from UDP server
    --------------------------------*/
    n = recvfrom(sock, (char *)buffer, MAXLINE,  
                MSG_WAITALL, (struct sockaddr *) &servaddr, 
                &len); 
    buffer[n] = '\0'; 
    printf("Got message from UDP Server!\n"); 
    printf("Server : %s\n", buffer); 
	close(sock); 
	return 0; 
} 
