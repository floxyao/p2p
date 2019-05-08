//===============================================================================================
//HW 4:     P2P
//Author:   Yao, Imantaka, Valdriz
//Course:   CECS-327
//Date:     4-10-19
//===============================================================================================
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h>
#include <unistd.h>
#include "helper.h"
#include <fcntl.h>
#include <arpa/inet.h>
#include <pthread.h>

#define SENDER_NAME "client 2: "
#define IP_ADDR "127.0.0.1"
#define PORT 8080
#define NUM_THREADS 3

//==========================================================================
// SOCKET STUFF
//==========================================================================
int       sock2, udp_sock, len, n;
struct    sockaddr_in client_addr2, serv_addr;   
struct    ServantData my_data;
socklen_t CLADDR_LEN = sizeof(client_addr2);
int       inet_pton(); //get rid of warning
char      msg[MSG_LEN];
char      buffer[BUF_SIZE] = {0};
char      guid[10];


//==========================================================================
// User Input Thread
// Gets the message input from user.
//==========================================================================
void* input_thread(void* arg){
    for(;;){
        char label[] = SENDER_NAME;
        fgets(msg, MSG_LEN, stdin);
        concat(label, msg); //concat label to message to identify sender
        strcpy(msg, label);
    }
    pthread_exit(NULL);
    return NULL;
}

//==========================================================================
// Send/Receive Socket
// Sends and receives messages to client. 
// *note sleep() is used because without it, the loop appears to be too fast
// and messages aren't being sent correctly
//==========================================================================
void* tcp_thread(void* arg){

    int n = send(sock2, &my_data, sizeof(my_data), 0); // send object
    bzero(buffer, sizeof(buffer));

    recv(sock2, &my_data, sizeof(my_data), 0);         // get message

    printf("Client 2 received: %d\n", my_data.GUID);

    /*-----------------------------------
     Change socket to non-blocking
        needs to happen after registration
    -----------------------------------*/
    fcntl(sock2, F_SETFL, O_NONBLOCK);

    for(;;){
        //int valread = recv( sock , buffer, BUF_SIZE, 0);
        //printf("Client 2 sending loop\n");
        send(sock2 , msg , strlen(msg) , 0);
        memset(msg, 0, MSG_LEN);                      //clear message
        printf("%s",buffer);                          //display message
        bzero(buffer, sizeof(buffer));                //flush buffer
        sleep(1);                                     //introduce delay or else loops too fast (?) 
    }

    close(sock2); 
    pthread_exit(NULL);
    return NULL; //silence
}

//==========================================================================
// Send/Receive Socket
// Sends and receives messages to client. 
// *note sleep() is used because without it, the loop appears to be too fast
// and messages aren't being sent correctly
//==========================================================================
void* udp_thread(void* arg){  
	/*-------------------------------
     Creating UDP socket 
    --------------------------------*/
	if ((udp_sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) { 
		printf("UDP socket 2 failed"); 
		exit(0); 
	} 
    else{
        printf("\nClient 2 UDP socket connected\n"); 
    }

	memset(&serv_addr, 0, sizeof(serv_addr)); 

	/*-------------------------------
     Fill server information
    --------------------------------*/
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(PORT); 
	serv_addr.sin_addr.s_addr = inet_addr(IP_ADDR); 

	/*-------------------------------
     Send message to UDP server
    --------------------------------*/
    for(;;){
        // after connecting to UDP, flag every 60 seconds

        
        sleep(2);


        sprintf(guid, "%d", my_data.GUID); // convert int to char


        sendto(udp_sock, (const char*)guid, strlen(guid), 
            0, (const struct sockaddr*)&serv_addr, 
            sizeof(serv_addr)); 

        printf("2 Message sent.\n");
    }
	close(udp_sock); 
}

//==========================================================================
// Main connects sockets, creates and join threads
// notes: AF_INET = domain of socket
//        SOCK_STREAM = type of socket (TCP/UDP)
//        0 = default protocol, TCP   */
//==========================================================================
int main(int argc, char const *argv[]) 
{

    if(argv[1] != NULL){
        strcpy(my_data.my_file, argv[1]);
    }
    else{
        printf("\ninsert file name\n");
        exit(0);
    }

    /*---------------------------------
     Creating socket file descriptor
    ----------------------------------*/
	if ((sock2 = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	}
    
	memset(&serv_addr, '0', sizeof(serv_addr));
    
    /*---------------------------------------------------------------------
     Specify address for socket to connect to
     Specify family of address so it knows what type of address
     Specify port; htons() converts the PORT to correct data format
     so structure understands the port number and where we need connect to
    ----------------------------------------------------------------------*/
	serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);
    
    /*----------------------------------------------------------
	 Convert IPv4 and IPv6 addresses from text to binary form
     serv_addr = struct holding info about address
     sin_addr is a struct itself containing the address itself
    -----------------------------------------------------------*/
    if(inet_pton(AF_INET, IP_ADDR, &serv_addr.sin_addr)<=0)
	{ 
		printf("\nInvalid address/ Address not supported \n"); 
		return -1; 
	} 

    /*-------------------------------------------------------
     Connect to the socket
     sock      = our socket
     serv_addr = cast our address to the right struct type
     size      = size of addr
     Returns an integer to indicate success/failure
    -------------------------------------------------------*/
	if (connect(sock2, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nClient 2 Connection Failed \n"); 
		return -1; 
    }
    else{
        printf("\nClient 2 Connection Established");
        printf("\nIP: %s",IP_ADDR);
        printf("\nPORT: %d\n\n",PORT);
    }
    
    /*---------------------
     Create/Join threads
    ---------------------*/
    int rc;
    pthread_t threads[NUM_THREADS];
    //printf("main(): creating input thread \n");
    rc = pthread_create(&threads[0], NULL, &input_thread, NULL);
    if(rc){
        printf("Error: unable to create thread, %d \n", rc);
        exit(-1);
    }
    //printf("main(): creating message thread \n");
    rc = pthread_create(&threads[1], NULL, &tcp_thread, NULL);
    if(rc){
        printf("Error: unable to create thread, %d \n", rc);
        exit(-1);
    }
    rc = pthread_create(&threads[2], NULL, &udp_thread, NULL);
    if(rc){
        printf("Error: unable to create thread, %d \n", rc);
        exit(-1);
    }
    //join threads
    for(int i = 0; i < NUM_THREADS; i++){
        rc = pthread_join(threads[i], NULL);
        if(rc){
            printf("Joining Thread Error: %d \n", rc);
        }
    }
    
    
    return 0;
}
