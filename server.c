//===============================================================================================
//HW 4:     P2P
//Author:   Yao, Imantaka, Valdriz
//Course:   CECS-327
//Date:     4-10-19
//Info:     We are essentially making Napster
//===============================================================================================
#include <unistd.h> 
#include <stdio.h> 
#include <sys/socket.h> 
#include <stdlib.h> 
#include <netinet/in.h> 
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include "helper.h"

#define SENDER_NAME "server: "
#define PORT 8080
#define NUM_THREADS 2


int       server_fd, udp_fd, new_socket, new_socket2;
struct    sockaddr_in server_address, client_address;
struct    ServantData rcv_data;
int       addrlen = sizeof(server_address);
char      msg[MSG_LEN];
char      buffer[BUF_SIZE] = {0};
ssize_t   n; 
socklen_t len; 
time_t    current_time;
struct    tm * time_info;

//================================================================================
// Function: tcp_thread
// Handles TCP socket connection/communication with TCP client
// This is run in one of the multithreads alongside UDP thread
//================================================================================
void* tcp_thread(void* arg){

    /*-------------------
     Creating TCP socket 
    ---------------------*/
	if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)
	{ 
		perror("TCP socket failed"); 
		exit(EXIT_FAILURE); 
    }
    else{
        printf("\nTCP Socket Created!\n");
    }
	server_address.sin_family = AF_INET;
	server_address.sin_addr.s_addr = INADDR_ANY; 
	server_address.sin_port = htons( PORT ); 
	
	/*-------------------------------------------
      TCP Bind - 
      once you have a socket, bind
      it to a port on your machine
    --------------------------------------------*/
	if (bind(server_fd, (struct sockaddr *)&server_address, sizeof(server_address))<0)
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
	if (listen(server_fd, 3) < 0) 
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
	if ((new_socket = accept(server_fd, (struct sockaddr *)&server_address, 
					(socklen_t*)&addrlen)) < 0)
	{ 
		perror("accept error 1\n"); 
		exit(EXIT_FAILURE); 
	}
    else{
        /*---------------------
            Register Client
        ----------------------*/
        printf("\nTCP Socket 1 Connected!\n");
        struct ServantData rcv_data;

        recv(new_socket , &rcv_data, sizeof(rcv_data), 0);        // get obj
        rcv_data.GUID = reg.size+1;

        time(&current_time);
        time_info = localtime(&current_time);
        strftime(rcv_data.time_string, sizeof(rcv_data.time_string), "%H:%M:%S", time_info);

        rcv_data.alive = TRUE;                                    // mark this client alive
        reg.servants[reg.size++] = rcv_data;
        print(rcv_data.GUID-1);                                    // print client information

        int n = send(new_socket, &rcv_data, sizeof(rcv_data), 0); // send back GUID
    }

    /*------------------------------------------------------------------------------
     Accept client 2 connection
    -------------------------------------------------------------------------------*/
    if ((new_socket2 = accept(server_fd, (struct sockaddr *)&server_address, 
					(socklen_t*)&addrlen)) < 0)
	{ 
		perror("accept error 2\n"); 
		exit(EXIT_FAILURE); 
	}
    else{
        /*---------------------
            Register Client
        ----------------------*/
        printf("\nTCP Socket 2 Connected!\n");
        //struct ServantData rcv_data;
        time_t t = time(NULL);

        recv(new_socket2 , &rcv_data, sizeof(rcv_data), 0);        // get obj

        rcv_data.GUID = reg.size+1;                                // GUID = size of reg cause unique

        time(&current_time);
        time_info = localtime(&current_time);
        strftime(rcv_data.time_string, sizeof(rcv_data.time_string), "%H:%M:%S", time_info);

        rcv_data.alive = TRUE;                                     // mark this client alive
        
        reg.servants[reg.size++] = rcv_data;
        print(rcv_data.GUID-1);                                    // print client information
  
        int m = send(new_socket2, &rcv_data, sizeof(rcv_data), 0); // send obj
    }

    /*-----------------------------------
     Change socket to non-blocking
     needs to happen after registration
    -----------------------------------*/
    fcntl(new_socket2, F_SETFL, O_NONBLOCK);
    fcntl(new_socket, F_SETFL, O_NONBLOCK);
    fcntl(server_fd, F_SETFL, O_NONBLOCK);

    for(;;){
        //printf("\ntcp .\n");
        int valread = recv( new_socket , buffer, BUF_SIZE, 0);
        int valread2 = recv( new_socket2 , buffer, BUF_SIZE, 0);

        // send(new_socket , msg , strlen(msg) , 0);
        // send(new_socket2 , msg , strlen(msg) , 0);
        // memset(msg, 0, MSG_LEN);
        //printf("\nTCP BUFFER\n");
        printf("%s", buffer);                                       //display message
        bzero(buffer, sizeof(buffer));                              //flush buffer
        sleep(3);                                                   //introduce delay or else loops too fast (?)                                  
    }

    pthread_exit(NULL);
}

//================================================================================
// Function: udp_thread
// Handles UDP connection/communication with UDP client
// Send "Datagrams" over network to notify client is still connected
// This is run in one of the multithreads along with TCP thread
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
        perror("udp bind failed"); 
        exit(EXIT_FAILURE); 
    }

    /*-----------------------------------------------------
      Wait for ping from UDP client
    ------------------------------------------------------*/
    
    for(;;){
        n = recvfrom(udp_fd, (char *)buffer, BUF_SIZE, MSG_WAITALL, (struct sockaddr *) &client_address, &len); 
        buffer[n] = '\0'; 
        
        printf("\n\n*ping* client[%s]", buffer); 
        int GUID = buffer[0] - '0';                       

        char _current_time[9];
        time(&current_time);
        time_info = localtime(&current_time);
        strftime(_current_time, sizeof(_current_time), "%H:%M:%S", time_info);

        int current_time = convert_to_seconds(_current_time);

        /*  
            1. Gets the udp message from each client
            2. Check which client it came from
            3. If client is alive and less than x time, update
            4. Else remove from registry and mark dead
        */
        for(int client_no=0; client_no<reg.size; client_no++){
            if( alive(client_no) == TRUE && reg.servants[client_no].GUID != GUID ){
                int timestamp = convert_to_seconds(reg.servants[client_no].time_string);

                if(abs(timestamp - current_time) >= END_TIME){
                    reg.servants[client_no] = (struct ServantData){ .GUID = 0, .my_file = "", .time_string = "", .alive = FALSE };
                    printf("\nremoving %d from registry\n",client_no+1);
                }
            }
            else{
                update_time(client_no);
                printf(" @time ");  
                puts(reg.servants[client_no].time_string);
            }
        }
        
        bzero(buffer, sizeof(buffer));
        sleep(1);    
    }
    close(udp_fd); 
    pthread_exit(NULL);
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
    pthread_t threads[NUM_THREADS];
    int rc = pthread_create(&threads[0], NULL, &tcp_thread, NULL);
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
