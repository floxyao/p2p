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

#define SENDER_NAME "client 2: "
#define IP_ADDR "127.0.0.1"
#define IP_BROADCAST "255.255.255.255"
#define SERVER_PORT 8080
#define BROADCAST_PORT 8081
#define DOWNLOAD_PORT 8082
#define NUM_THREADS 3
#define MAXRECVSTRING 255  /* Longest string to receive */

//==========================================================================
// SOCKET STUFF
//==========================================================================
int       sock2, udp_sock,broadcast_sock, broadcast_receive_sock, download_sock, client_sock, broadcastPermission, brd_len, n;
struct    sockaddr_in client_addr2, serv_addr, broadcastAddr1, broadcastAddr2, download_addr;
struct    ServantData my_data;
socklen_t CLADDR_LEN = sizeof(client_addr2);
socklen_t down_sock_len = sizeof(download_addr);
int       inet_pton(); //get rid of warning
char      msg[MSG_LEN];
char      targetFile[MSG_LEN];
char      buffer[BUF_SIZE] = {0};
char      download_message[BUF_SIZE] = {0};
char      guid[10];
char      fileName[MSG_LEN];
// char      fileName[MSG_LEN];
char      *broadcastMessage;



//==========================================================================
// Function: download
// downloads file from target client
//==========================================================================
void start_download(){
    // printf("got into start_download\n");
    /*---------------------------------
     Creating socket file descriptor
    ----------------------------------*/
	if ((download_sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
	{
		printf("\n Download Socket creation error \n");
		return -1;
	}
    else{
        printf("\np2p connection initiated\n");
    }

    download_addr.sin_family = AF_INET;
    download_addr.sin_addr.s_addr = INADDR_ANY;
    download_addr.sin_port = htons( DOWNLOAD_PORT);


    //reuse ports and addr
    int reuse = 1;
    if (setsockopt(download_sock, SOL_SOCKET, SO_REUSEADDR, (const char*)&reuse, sizeof(reuse)) < 0)
        perror("setsockopt(SO_REUSEADDR) failed");

    #ifdef SO_REUSEPORT
        if (setsockopt(download_sock, SOL_SOCKET, SO_REUSEPORT, (const char*)&reuse, sizeof(reuse)) < 0)
            perror("setsockopt(SO_REUSEPORT) failed");
    #endif

	/*-------------------------------------------
      TCP Bind -
      once you have a socket, bind
      it to a port on your machine
    --------------------------------------------*/
	if (bind(download_sock, (struct sockaddr *)&download_addr, sizeof(download_addr))<0)
	{
		perror("tcp bind failed");
		exit(EXIT_FAILURE);
    }

    /*-----------------------------
      TCP Listen -
      original socket server_fd
      listens for more incoming
      connections
    ------------------------------*/
	if (listen(download_sock, 3) < 0)
	{
		perror("listen error");
		exit(EXIT_FAILURE);
	}
    else{
        printf("\nTCP Listening...\n");
    }

    /*------------------------------------------------------------------------------
     Accept client 1 connection
    -------------------------------------------------------------------------------*/
	if ((download_sock = accept(download_sock, (struct sockaddr *)&download_addr,
					(socklen_t*)&down_sock_len)) < 0)
	{
		perror("accept error 1\n");
		exit(EXIT_FAILURE);
	}
    else{
        /*--------------------------
            Start download process
        --------------------------*/
        printf("\np2p Connected!\n");

        // time_t t = time(NULL);


        while(true){
            int valread = recv(download_sock , download_message, BUF_SIZE, 0);   //check target file

            if(valread > 0){
                strcpy(targetFile, download_message );
                printf("Target file: %s\n", targetFile);
                // printf("Testing: %s\n", download_message);
                bzero(download_message, BUF_SIZE); //clear buffer to re use
                strcpy(download_message, "syn+ack");
                send(download_sock, download_message, BUF_SIZE, 0);
                printf("message: %s\n", download_message);
                printf("Message sent\n");
                // printf("valread: %d\n", valread);
                bzero(download_message, BUF_SIZE);



                break;
            }
        }


        // int n = send(new_socket, &rcv_data, sizeof(rcv_data), 0); // send back GUID
    }

    printf("closing p2p connection. . .\n");
    close(download_sock);
    printf("press enter to continue\n");
    // printf("\n====================================\n");
    // printf("    1. Find file\n");
    // printf("    2. Exit\n");
    // printf("====================================\n");
    // printf("Please enter the number of your choice: ");
    // close(client_sock);
}


//==========================================================================
// Function: download
// downloads file from target client
//==========================================================================
void connect_download(char targetFile[MSG_LEN]){
    // printf("got into connect download\n");
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

    // strcpy(download_message, "");
    printf("target: %s\n", targetFile);
    send(download_sock, targetFile, sizeof(targetFile), 0); // send target file
    printf("message sent \n");
    // printf("message: %s\n", download_message);
    bzero(download_message,BUF_SIZE);

    while(true){ //wait for message back
        // printf("in while loop\n");
        int valread = recv(download_sock, download_message, BUF_SIZE,0);

        if(valread > 0){
            printf("received response!\n");
            printf("Message: %s\n", download_message);



            break;
        }
    }
    printf("closing p2p connection. . . .\n");
    close(download_sock);
}

//==========================================================================
// Function: broadcast
// Broadcasts message to find desired client to connect with
// input: target (target GUID)
//==========================================================================
void broadcast(char target[10]){
    printf("Searching for client. . .\n");
    // printf("got into broadcast\n");
    broadcastMessage = target;
    // printf("broadcastMessage: %s\n", broadcastMessage);

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
    memset(&broadcastAddr1, 0, sizeof(broadcastAddr1));   /* Zero out structure */
    broadcastAddr1.sin_family = AF_INET;                 /* Internet address family */
    broadcastAddr1.sin_addr.s_addr = inet_addr(IP_BROADCAST);/* Broadcast IP address */
    broadcastAddr1.sin_port = htons(BROADCAST_PORT);         /* Broadcast port */

    brd_len = strlen(broadcastMessage);  /* Find length of sendString */

    for (int i = 0; i < 5; i++) /* Run forever */
    {
         /* Broadcast sendString in datagram to clients every 3 seconds*/
         if (sendto(broadcast_sock, broadcastMessage, brd_len, 0, (struct sockaddr *)
               &broadcastAddr1, sizeof(broadcastAddr1)) != brd_len)
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
// Function: broadcast
// listens for broadcast messages
// input: target (target GUID)
//==========================================================================
void* broadcastReceive(){
    // printf("Got into broadcast receive\n");
    char recvString[MAXRECVSTRING+1]; /* Buffer for received string */
    int recvStringLen;                /* Length of received string */

    /* Construct bind structure */
    memset(&broadcastAddr2, 0, sizeof(broadcastAddr2));   /* Zero out structure */
    broadcastAddr2.sin_family = AF_INET;                 /* Internet address family */
    broadcastAddr2.sin_addr.s_addr = htonl(INADDR_ANY);  /* Any incoming interface */

    broadcastAddr2.sin_port = htons(BROADCAST_PORT);      /* Broadcast port */

   for(;;)
    {

       if ((broadcast_receive_sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0)
       {
           printf("Socket() failed\n");
       }

       /* Bind to the broadcast port */
       if (bind(broadcast_receive_sock, (struct sockaddr *) &broadcastAddr2, sizeof(broadcastAddr2)) < 0)
       {
        //    printf("bind() failed\n");
           close(broadcast_receive_sock);
          continue;
       }


       /* Receive a single datagram from the server */
       if ((recvStringLen = recvfrom(broadcast_receive_sock, recvString, MAXRECVSTRING, 0, NULL, 0)) < 0)
       {
          printf("recvfrom() failed");
       }
       else{
            recvString[recvStringLen] = '\0';
            // printf("Received: %s\n", recvString);    /* Print the received string */

            char id[10];
            sprintf(id, "%d", my_data.GUID);
            if(strcmp(recvString, id) == 0){
                //initiate tcp connection
                start_download();
            }


       }
       close(broadcast_receive_sock); //close socket
       sleep(5);
    }

    // exit(0);
    // close(broadcast_sock);
    pthread_exit(NULL);
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

    printf("Client 2 GUID: %d\n", my_data.GUID);

    /*-----------------------------------
     Change socket to non-blocking
        needs to happen after registration
    -----------------------------------*/
    fcntl(sock2, F_SETFL, O_NONBLOCK);

		int choice;
    bool stay = true;
    while(stay){
        

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
                sscanf(fileName, "%s", targetFile);

                printf("target file: %s\n", targetFile);
                //send rquested file to server
                send(sock2, targetFile, strlen(targetFile), 0);

                printf("\nfinding file. . . .\n");

                //wait for 15 secs for valid response
                for(int i = 0; i < 15; i++){ //
                    int valread = recv( sock2 , buffer, BUF_SIZE, 0);
                    // printf("buffer right after recv: %s\n", buffer);
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
                            connect_download(targetFile);
                            break;
                        }

                    }
                    sleep(1);
                }

                bzero(buffer, sizeof(buffer));
                memset(targetFile, 0, MSG_LEN); //clear msg for later use
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
                // printf("\nincorrect input\n");
                break;
            }
        }
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
	serv_addr.sin_port = htons(SERVER_PORT); 
	serv_addr.sin_addr.s_addr = inet_addr(IP_ADDR); 

	/*-------------------------------
     Send message to UDP server
    --------------------------------*/
    for(;;){
        // after connecting to UDP, flag every 60 seconds

        
        sleep(15);


        sprintf(guid, "%d", my_data.GUID); // convert int to char


        sendto(udp_sock, (const char*)guid, strlen(guid), 
            0, (const struct sockaddr*)&serv_addr, 
            sizeof(serv_addr)); 

        // printf("2 Message sent.\n");
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
    int i = 1; // start at index 1

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
     Specify port; htons() converts the SERVER_PORT to correct data format
     so structure understands the port number and where we need connect to
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
	if (connect(sock2, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
	{
		printf("\nClient 2 Connection Failed \n");
		return -1;
    }
    else{
        printf("\nClient 2 Connection Established");
        printf("\nIP: %s",IP_ADDR);
        printf("\nPORT: %d\n\n",SERVER_PORT);
    }

    /*---------------------
     Create/Join threads
    ---------------------*/
    int rc;
    pthread_t threads[NUM_THREADS];
    //printf("main(): creating input thread \n");
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

    rc = pthread_create(&threads[2], NULL, &broadcastReceive, NULL);
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
