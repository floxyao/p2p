//===============================================================================================
//HW 4:     P2P
//Author:   Yao, Imantaka, Valdriz
//Course:   CECS-327
//Date:     4-10-19
//Info:     We are essentially making a Napster
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
#define NUM_THREADS 2
#define MSG_LEN 100
#define BUF_SIZE 1024


struct Registry{
    struct ServantData servants[2];
    int size;
} reg;
//================================================================================
// SOCKET STUFF
//================================================================================
int     server_fd, udp_fd, new_socket, new_socket2;
struct  sockaddr_in server_address, client_address;
int     addrlen = sizeof(server_address);
char    msg[MSG_LEN];
char    buffer[BUF_SIZE] = {0};
ssize_t n; 
socklen_t len; 

//================================================================================
// Concat two strings 
// "server:  " + <message>
// "client1: " + <message>
//================================================================================
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

//================================================================================
// Function: join
// Purpose: Registers the clients that join the network
//================================================================================
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

//================================================================================
// Function: publish
// Purpose: outputs all client GUID's and 
//          files that each client has
//================================================================================
void publish(){
    printf("\nPrinting Servants lists of files: \n");

    printf("Client 1 GUID: %d",   reg.servants[0].GUID);
    printf("\nClient 1 files %s\n", reg.servants[0].my_file);
    printf("\nClient 2 GUID: %d",   reg.servants[1].GUID);
    printf("\nClient 2 files %s\n", reg.servants[1].my_file);
}

//================================================================================
// Function: udp_thread
// Handles UDP connection/communication with UDP client
// Send "Datagrams" over network to notify client is still connected
//================================================================================
void* udp_thread(void* arg){
    char *hello = "Hello from server"; 

    /*-----------------------------------------------------
     Creating UDP socket 
    ------------------------------------------------------*/
    if ( (udp_fd = socket(AF_INET, SOCK_DGRAM, 0)) < 0 ) { 
        perror("socket creation failed"); 
        exit(EXIT_FAILURE); 
    } else{
        printf("\nUDP Socket Created!\n"); 
    }
      
    memset(&server_address, 0, sizeof(server_address)); 
    memset(&client_address, 0, sizeof(client_address)); 
      
    /*-----------------------------------------------------
     Fill server information
    ------------------------------------------------------*/
    server_address.sin_family    = AF_INET; // IPv4 
    server_address.sin_addr.s_addr = INADDR_ANY; 
    server_address.sin_port = htons(PORT); 

    /*-----------------------------------------------------
      UDP Bind
    ------------------------------------------------------*/
    if ( bind(udp_fd, (const struct sockaddr *)&server_address, sizeof(server_address)) < 0 ) 
    { 
        perror("bind failed"); 
        exit(EXIT_FAILURE); 
    } else{
        printf("UDP Socket Connected!\n"); 
    }
      
    /*-----------------------------------------------------
     Wait for message from UDP client
    ------------------------------------------------------*/
    printf("Waiting for message from UDP...\n"); 
    n = recvfrom(udp_fd, (char *)buffer, BUF_SIZE, MSG_WAITALL, (struct sockaddr *) &client_address, &len); 
    buffer[n] = '\0'; 
    printf("Got message from UDP Client!\n");
    printf("Client: %s\n", buffer); 

    /*-----------------------------------------------------
     Send message back to UDP client
    ------------------------------------------------------*/
    sendto(udp_fd, (const char *)hello, strlen(hello), 0, (const struct sockaddr *) &client_address, len); 
    printf("Hello message sent.\n");  
      

    for(;;){
        sleep(3);
        printf("\nServer UDP Created!\n");
    }
    close(udp_fd); 
}

//================================================================================
// Func: tcp_thread
// Handles TCP socket connection/communication with TCP client
//================================================================================
void* tcp_thread(void* arg){

    /*--------------------------------------------------------------------------
     Creating TCP socket 
    ---------------------------------------------------------------------------*/
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{ 
		perror("TCP socket failed"); 
		exit(EXIT_FAILURE); 
    }
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY; 
	server_address.sin_port = htons( PORT ); 
	
	/*--------------------------------------------------------------------------
      TCP Bind - once you have a socket, bind() it to a port on your machine
    ---------------------------------------------------------------------------*/
	if (bind(server_fd, (struct sockaddr *)&server_address, sizeof(server_address))<0)
	{ 
		perror("tcp bind failed"); 
		exit(EXIT_FAILURE); 
    }
    else{
        printf("\nTCP Connected!\n");
    }

    /*-------------------------------------------------------------------------
      TCP Listen - original socket server_fd listens for more incoming connections
    -------------------------------------------------------------------------*/
	if (listen(server_fd, 3) < 0) 
	{ 
		perror("listen error"); 
		exit(EXIT_FAILURE); 
	}

    /*---------------------------------
     Accept client 1 connection
    ----------------------------------*/
	if ((new_socket = accept(server_fd, (struct sockaddr *)&server_address, 
					(socklen_t*)&addrlen)) < 0)
	{ 
		perror("accept error 1\n"); 
		exit(EXIT_FAILURE); 
	}
    else{
        printf("\nSocket 1 Connected!");
    }

    /*----------------------------------
     Accept client 2 connection
    -----------------------------------*/
    if ((new_socket2 = accept(server_fd, (struct sockaddr *)&server_address, 
					(socklen_t*)&addrlen)) < 0)
	{ 
		perror("accept error 2\n"); 
		exit(EXIT_FAILURE); 
	}
    else{
        printf("\nSocket 2 Connected!");
    }

    /*----------------------------------
     Join (Register Clients)
    -----------------------------------*/
    join();

    /*----------------------------------
     Publish 
    -----------------------------------*/
    publish();

    /*-----------------------------------
     Change socket to non-blocking
        needs to happen after registration
    -----------------------------------*/
    fcntl(new_socket2, F_SETFL, O_NONBLOCK);
    fcntl(new_socket, F_SETFL, O_NONBLOCK);
    fcntl(server_fd, F_SETFL, O_NONBLOCK);

    for(;;){
        printf("\nTCP Thread\n");
        // okay i registered the clients, waiting for a message from clients
        int valread = recv( new_socket , buffer, BUF_SIZE, 0);
        int valread2 = recv( new_socket2 , buffer, BUF_SIZE, 0);

        // send(new_socket , msg , strlen(msg) , 0);
        // send(new_socket2 , msg , strlen(msg) , 0);
        // memset(msg, 0, MSG_LEN);
        printf("%s", buffer);                         //display message
        bzero(buffer, sizeof(buffer));                //flush buffer
        sleep(3);                                     //introduce delay or else loops too fast (?)                                  
    }
}

//================================================================================
// Main connects sockets, creates and join threads
// notes: AF_INET = domain of socket
//        SOCK_STREAM = type of socket (TCP/UDP)
//        0 = default protocol, TCP   */
//================================================================================
int main(int argc, char const *argv[])
{
    /*-------------------------------
     Create UDP and TCP threads
    --------------------------------*/
    int rc;
    pthread_t threads[NUM_THREADS];
    rc = pthread_create(&threads[0], NULL, &tcp_thread, NULL);
    if(rc){
        printf("Error: unable to create thread, %d \n", rc);
        exit(-1);
    }
    rc = pthread_create(&threads[1], NULL, &udp_thread, NULL);
    if(rc){
        printf("Error: unable to create thread, %d \n", rc);
        exit(-1);
    }

    /*-------------------------------
     Join UDP and TCP threads
    --------------------------------*/
    for(int i = 0; i < NUM_THREADS; i++){
        rc = pthread_join(threads[i], NULL);
        if(rc){
            printf("Joining Thread Error: %d \n", rc);
        }
    }
    for(;;){
        printf("Joining Thread Error: \n");
    }

    close(new_socket);
    close(new_socket2);

    return 0;
}
