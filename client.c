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
#include <fcntl.h>
#include <pthread.h>
#include <arpa/inet.h>
#include "helper.h"
#include <stdbool.h>

#define SENDER_NAME "client 1: "
#define IP_ADDR "127.0.0.1"
#define IP_BROADCAST "255.255.255.255" 
#define SERVER_PORT 8080
#define BROADCAST_PORT 8081
#define DOWNLOAD_PORT 8082
#define NUM_THREADS 3


//global variables
int       sock, udp_sock, broadcast_sock, download_sock, broadcastPermission, brd_len, n;
struct    sockaddr_in client_addr, serv_addr, broadcastAddr, download_addr; 
struct    ServantData my_data;    
socklen_t CLADDR_LEN = sizeof(client_addr);
int       inet_pton(); //get rid of warning
char      msg[MSG_LEN];
char      buffer[BUF_SIZE] = {0};
char      download_message[BUF_SIZE] = {0};
char      guid[10];
char      fileName[MSG_LEN];
char      *broadcastMessage;
//thread data
int rc; 
pthread_t threads[NUM_THREADS];


//==========================================================================
// User Input Thread
// Gets the message input from user.
//==========================================================================
// void* input_thread(void* arg){
//     for(;;){
//         char label[] = SENDER_NAME;
//         fgets(msg, MSG_LEN, stdin);
//         concat(label, msg); //concat label to message to identify sender
//         strcpy(msg, label);
//     }
//     pthread_exit(NULL);
//     return NULL;
// }

//==========================================================================
// Function: download
// downloads file from target client
//==========================================================================
void connect_download(){
    printf("got into connect download\n");
    /*---------------------------------
     Creating socket file descriptor
    ----------------------------------*/
	if ((download_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	}
    
	memset(&download_addr, '0', sizeof(download_addr));
    
    /*---------------------------------------------------------------------
     Specify address for socket to connect to
     Specify family of address so it knows what type of address
     Specify SERVER_PORT; htons() converts the SERVER_PORT to correct data format
     so structure understands the SERVER_PORT number and where we need connect to
    ----------------------------------------------------------------------*/
	download_addr.sin_family = AF_INET;
    download_addr.sin_port = htons(DOWNLOAD_PORT);
    
    /*----------------------------------------------------------
	 Convert IPv4 and IPv6 addresses from text to binary form
     serv_addr = struct holding info about address
     sin_addr is a struct itself containing the address itself
    -----------------------------------------------------------*/
    if(inet_pton(AF_INET, IP_ADDR, &download_addr.sin_addr)<=0)
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
	if (connect(download_sock, (struct sockaddr *)&download_addr, sizeof(download_addr)) < 0) 
	{ 
		printf("\np2p Connection Failed \n"); 
		perror("Error: ");
    }
    else{
        printf("\np2p Connection Established");
        printf("\nIP: %s",IP_ADDR);
        printf("\nPORT: %d\n\n",DOWNLOAD_PORT);
    }
    
    strcpy(download_message, "syn");
    send(download_sock, download_message, sizeof(my_data), 0); // send syn message
    printf("message sent \n");
    bzero(download_message,BUF_SIZE);
    while(true){ //wait for message back
        int valread = recv(download_sock, download_message, BUF_SIZE,0);

        if(valread > 0){
            printf("Testing: %s\n", download_message);
            break;
        }
    }
    close(download_sock);
}

//==========================================================================
// Function: broadcast
// Broadcasts message to find desired client to connect with
// input: target (target GUID)
//==========================================================================
void broadcast(char target[10]){
    printf("got into broadcast\n");
    broadcastMessage = target;
    printf("broadcastMessage: %s\n", broadcastMessage);

    /* Create socket for sending/receiving datagrams */
    if ((broadcast_sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
    //if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
       printf("socket() failed");
    }
        //DieWithError("socket() failed");

    /* Set socket to allow broadcast */
    broadcastPermission = 1;
    if (setsockopt(broadcast_sock, SOL_SOCKET, SO_BROADCAST, (void *) &broadcastPermission, 
    //if (setsockopt(sock, SOL_SOCKET, SO_REUSEPORT, (void *) &broadcastPermission, 
          sizeof(broadcastPermission)) < 0)
    {
       printf("setsockeopt() failed");
    }
          //DieWithError("setsockopt() failed");

    /* Construct local address structure */
    memset(&broadcastAddr, 0, sizeof(broadcastAddr));   /* Zero out structure */
    broadcastAddr.sin_family = AF_INET;                 /* Internet address family */
    broadcastAddr.sin_addr.s_addr = inet_addr(IP_BROADCAST);/* Broadcast IP address */
    broadcastAddr.sin_port = htons(BROADCAST_PORT);         /* Broadcast port */

    brd_len = strlen(broadcastMessage);  /* Find length of sendString */
   
    for (int i = 0; i < 5; i++) /* Run forever */
    {
         /* Broadcast sendString in datagram to clients every 3 seconds*/
         if (sendto(broadcast_sock, broadcastMessage, brd_len, 0, (struct sockaddr *) 
               &broadcastAddr, sizeof(broadcastAddr)) != brd_len)
         {
             printf("sendto() sent a different number of bytes than expected");
         }
             //DieWithError("sendto() sent a different number of bytes than expected");

        sleep(1);   /* Avoids flooding the network */
    }
    /* NOT REACHED */
    close(broadcast_sock);
}

//==========================================================================
// Send/Receive Socket
// Sends and receives messages to client. 
// *note sleep() is used because without it, the loop appears to be too fast
// and messages aren't being sent correctly
//==========================================================================
void* tcp_thread(void* arg){

    int n = send(sock, &my_data, sizeof(my_data), 0);
    bzero(buffer, sizeof(buffer));

    recv(sock, &my_data, sizeof(my_data), 0);

    printf("Client 1 GUID: %d\n", my_data.GUID);

    /*-----------------------------------
     Change socket to non-blocking
        needs to happen after registration
    -----------------------------------*/
    fcntl(sock, F_SETFL, O_NONBLOCK);

    int choice;
    bool stay = true;
    while(stay){
        // int valread = recv( sock , buffer, BUF_SIZE, 0);
        // printf("Client 1 sending loop\n");
        // send(sock , msg , strlen(msg) , 0);
        // memset(msg, 0, MSG_LEN);                      //clear message
        // printf("%s",buffer);                          //display message
        // bzero(buffer, sizeof(buffer));                //flush buffer
        // sleep(1);                                     //introduce delay or else loops too fast (?) 

        choice = 0; //reset choice 
        choice = menu();
   
        // printf("Got choice\n");
        // printf("Choice: %d\n", choice);
        
        switch(choice){
            case 1: //find file
            {
                
                //get file name from user
                printf("Please enter the name of the file: ");
                fgets(fileName ,MSG_LEN, stdin);
                sscanf(fileName, "%s", msg);

                //send rquested file to server
                send(sock, msg, strlen(msg), 0);
                memset(msg, 0, MSG_LEN); //clear msg for later use
                printf("\nfinding file. . . .\n");

                //wait for 15 secs for valid response
                for(int i = 0; i < 15; i++){ //
                    int valread = recv( sock , buffer, BUF_SIZE, 0);
                    printf("buffer right after recv: %s\n", buffer);
                    if(valread > 0){
                        if( strcmp(buffer, "0") == 0 ){
                            printf("File not in registry\n");
                            break; 
                        }
                        if(buffer[0] >= '0' && buffer[0] <= '9'){ //start fetching process
                            printf("Target GUID: %s\n",buffer);    //display target guid
                            // bzero(buffer, sizeof(buffer));  
                            char target[10];
                            sscanf(buffer, "%s", target);
                            // printf("target: %s\n", target);
                            broadcast(target);
                            connect_download();
                            break;
                        }
                        
                    }
                    sleep(1);
                }
                
                bzero(buffer, sizeof(buffer));  

                break;
            }
            case 2: //exit
            {
                printf("\nExiting. . . \n");
                // stay = false;
                exit(0);
                break;
            }
            default:
            {
                printf("\nincorrect input\n");
                break;
            }
        }
    }

    close(sock); 
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
		printf("UDP socket failed"); 
		exit(0); 
	} 
    else{
        // printf("\nClient 1 UDP socket connected\n"); 
    }

	memset(&serv_addr, 0, sizeof(serv_addr)); 

	/*-------------------------------
     Fill server information
    --------------------------------*/
	serv_addr.sin_family = AF_INET; 
	serv_addr.sin_port = htons(SERVER_PORT); 
	serv_addr.sin_addr.s_addr = inet_addr(IP_ADDR); 

	/*-------------------------------
     Send message to UDP server
    --------------------------------*/
    for(;;){
        // after connecting to UDP, flag every 60 seconds


        sleep(5);

        sprintf(guid, "%d", my_data.GUID); 


        // send time 
        sendto(udp_sock, (const char*)guid, strlen(guid), 
            0, (const struct sockaddr*)&serv_addr, 
            sizeof(serv_addr)); 

        // printf("1 Message sent.\n");

        //memset(message, 0, sizeof(message));
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
    // const char *temp = argv[1];
    // my_data.my_files[0] = malloc(sizeof(temp) + 1);
    
    int i = 1; // start at index 1

    if(argv[1] != NULL){
        strcpy(my_data.my_file, argv[1]);
    }
    else{
        printf("\ninsert file name\n");
        exit(0);
    }
    //strcpy(my_data.my_file, argv[1]);
    // do{
    //     strcpy(my_data.my_file, argv[i]);
    //     i++;
    // }while(argv[i] != NULL){
    
    /*---------------------------------
     Creating socket file descriptor
    ----------------------------------*/
	if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{ 
		printf("\n Socket creation error \n"); 
		return -1; 
	}
    
	memset(&serv_addr, '0', sizeof(serv_addr));
    
    /*---------------------------------------------------------------------
     Specify address for socket to connect to
     Specify family of address so it knows what type of address
     Specify SERVER_PORT; htons() converts the SERVER_PORT to correct data format
     so structure understands the SERVER_PORT number and where we need connect to
    ----------------------------------------------------------------------*/
	serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(SERVER_PORT);
    
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
	if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) 
	{ 
		printf("\nClient 1 Connection Failed \n"); 
		return -1; 
    }
    else{
        printf("\nClient 1 Connection Established");
        printf("\nIP: %s",IP_ADDR);
        printf("\nPORT: %d\n\n",SERVER_PORT);
    }
    
    /*-----------------------------------
     Change the socket to non-blocking
    -----------------------------------*/
    fcntl(sock, F_SETFL, O_NONBLOCK);
    
    /*---------------------
    Create/Join threads
    ---------------------*/

    //printf("main(): creating input thread \n");
    // rc = pthread_create(&threads[0], NULL, &input_thread, NULL);
    // if(rc){
    //     printf("Error: unable to create thread, %d \n", rc);
    //     exit(-1);
    // }
    //printf("main(): creating message thread \n");
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
    //join threads
    for(int i = 0; i < NUM_THREADS; i++){
        rc = pthread_join(threads[i], NULL);
        if(rc){
            printf("Joining Thread Error: %d \n", rc);
        }
    }
    
    return 0;
}