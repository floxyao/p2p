//===============================================================================================
//HW 4:     P2P
//Author:   Yao, Imantaka, Valdriz
//Course:   CECS-327
//Date:     4-10-19
//Info: 
//===============================================================================================
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include "struct.h"
#define SENDER_NAME "server: "
#define PORT 8080
#define NUM_CLIENTS 2
#define MSG_LEN 100
#define BUF_SIZE 1024


struct Registry{
    struct ServantData servants[2];
    int size;
} reg;
//==========================================================================
// SOCKET STUFF
//==========================================================================
int     server_fd, new_socket, new_socket2;
struct  sockaddr_in address, client_address;
int     addrlen = sizeof(address);
char    msg[MSG_LEN];
char    buffer[BUF_SIZE] = {0};

//==========================================================================
// Concat two strings 
// "server:  " + <message>
// "client1: " + <message>
//==========================================================================
void concat(char* p, char *q){
   int c, d;
   c = 0;
   while (p[c] != '\0') {
      c++;      
   }
   d = 0;
   while (q[d] != '\0') {
      p[c] = q[d];
      d++;
      c++;    
   }
   p[c] = '\0';
}

//==========================================================================
// Function: join
// Purpose: Registers the clients that join the network
//==========================================================================
void join(){
    // create object to accept data from clients
    struct ServantData rcv_data;

    /*------------------------------
     * Client 1
     *-----------------------------*/
    // get object from client 1
    recv(new_socket , &rcv_data, sizeof(rcv_data), 0);
    reg.size++;

    // generate GUID (just size for now)
    rcv_data.GUID = reg.size;
    reg.servants[0] = rcv_data;

    // send back GUID to client 1
    int n = send(new_socket, &rcv_data, sizeof(rcv_data), 0);


    /*------------------------------
     * Client 2
     *-----------------------------*/
    // get object from client 2
    recv(new_socket2 , &rcv_data, sizeof(rcv_data), 0);
    reg.size++;

    // generate GUID
    rcv_data.GUID = reg.size;
    reg.servants[1] = rcv_data;

    // send back GUID to client 2
    int m = send(new_socket2, &rcv_data, sizeof(rcv_data), 0);

    printf("\nServants Registered!\n");
}

//==========================================================================
// Function: publish
// Purpose: outputs information of the clients
//==========================================================================
void publish(){
    printf("\nPrinting Servants lists of files: \n");
    //#pragma pack(4)
    printf("Client 1 GUID: %d",   reg.servants[0].GUID);
    printf("\nClient 1 files %s\n", reg.servants[0].my_file);
    printf("\nClient 2 GUID: %d",   reg.servants[1].GUID);
    printf("\nClient 2 files %s\n", reg.servants[1].my_file);
    //#pragma pack(0)
}

//==========================================================================
// Create UDP Socket
// Send "Datagrams" over network to notify client is still connected
//==========================================================================
void create_UDP_connection(){
    // int sockfd;
    // /*-------------------------------
    //  Creating UDP socket file descriptor
    // --------------------------------*/
    // if ( (sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
    //     perror("socket creation failed"); 
    //     exit(EXIT_FAILURE); 
    // }else{
    //     printf("\nServer UDP Created!\n");
    // }

    // if ( bind(sockfd, (const struct sockaddr *)&address,  
    //         sizeof(address)) < 0 ) 
    // { 
    //     perror("bind failed"); 
    //     exit(EXIT_FAILURE); 
    // } 
      
    // char *hello = "Hello from server"; 
    // int len, n, MSG_CONFIRM; 
    // n = recvfrom(sockfd, (char *)buffer, BUF_SIZE,  
    //             MSG_WAITALL, ( struct sockaddr *) &client_address, 
    //             &len); 
    // buffer[n] = '\0'; 
    // printf("Client : %s\n", buffer); 
    // sendto(sockfd, (const char *)hello, strlen(hello),  
    //     MSG_CONFIRM, (const struct sockaddr *) &client_address, 
    //         len); 
    // printf("Hello message sent.\n");  
}

void create_tcp(){
    /*-------------------------------
     Creating TCP socket file descriptor
    --------------------------------*/
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{ 
		perror("TCP socket failed"); 
		exit(EXIT_FAILURE); 
    }
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY; 
	address.sin_port = htons( PORT ); 
	
	/*-------------------------------------------------------------------
      Bind - once you have a socket, bind() it to a port on your machine
    -------------------------------------------------------------------*/
	if (bind(server_fd, (struct sockaddr *)&address, sizeof(address))<0)
	{ 
		perror("bind failed"); 
		exit(EXIT_FAILURE); 
    }
    else{
        printf("\nConnected!\n");
    }

    /*-------------------------------------------------------------------------
      Listen - original socket server_fd listens for more incoming connections
    -------------------------------------------------------------------------*/
	if (listen(server_fd, 3) < 0) 
	{ 
		perror("listen error"); 
		exit(EXIT_FAILURE); 
	}

    /*-------------------------------------------------------------
     Accept - someone is waiting on you to accept their connection
     new_socket = the connection between you and the client

     And create threads
    --------------------------------------------------------------*/
    pthread_t threads[NUM_CLIENTS];
    int num_thread = 0;

    /*---------------------------
     Accept client 1 connection
    ----------------------------*/
	if ((new_socket = accept(server_fd, (struct sockaddr *)&address, 
					(socklen_t*)&addrlen)) < 0)
	{ 
		perror("accept error 1\n"); 
		exit(EXIT_FAILURE); 
	}
    else{
        /*---------------------------
         Socket 1 connected
        ----------------------------*/
        printf("\nSocket 1 Connected!");
    }

    /*---------------------------
     Accept client 2 connection
    ----------------------------*/
    if ((new_socket2 = accept(server_fd, (struct sockaddr *)&address, 
					(socklen_t*)&addrlen)) < 0)
	{ 
		perror("accept error 2\n"); 
		exit(EXIT_FAILURE); 
	}
    else{
        /*---------------------------
         Socket 2 connected
        ----------------------------*/
        printf("\nSocket 2 Connected!");
    }

    /*-----------------------------------------
     Change server socket to non-blocking
        client1 & 2 sockets shouldn't be non-block
        because they need to wait for server to 
        assign them a GUID
    ------------------------------------------*/
    //fcntl(new_socket2, F_SETFL, O_NONBLOCK);
    //fcntl(new_socket, F_SETFL, O_NONBLOCK);
    fcntl(server_fd, F_SETFL, O_NONBLOCK);
}

//==========================================================================
// Main connects sockets, creates and join threads
// notes: AF_INET = domain of socket
//        SOCK_STREAM = type of socket (TCP/UDP)
//        0 = default protocol, TCP   */
//==========================================================================
int main(int argc, char const *argv[])
{
    create_tcp();
    
    /*---------------------------
     Join (Register Clients)
    ----------------------------*/
    join();
    printf("joined\n");  

    /*---------------------------
     Publish 
    ----------------------------*/
    publish();
    printf("published\n"); 

    for(;;){
        // okay i registered the clients, waiting for a message from clients
        int valread = recv( new_socket , buffer, BUF_SIZE, 0);
        int valread2 = recv( new_socket2 , buffer, BUF_SIZE, 0);

        // send(new_socket , msg , strlen(msg) , 0);
        // send(new_socket2 , msg , strlen(msg) , 0);
        // memset(msg, 0, MSG_LEN);
        printf("%s", buffer);                         //display message
        bzero(buffer, sizeof(buffer));                //flush buffer
        sleep(1);                                     //introduce delay or else loops too fast (?)                                  
    }

    close(new_socket);
    close(new_socket2);

    return 0;
}

    // #pragma pack(4)
    // printf("\nClient 1 GUID: %d", rcv_data.GUID);
    // printf("\nClient 1 files %s\n", rcv_data.my_file);
    // #pragma pack(0)