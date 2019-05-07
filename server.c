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
#define MSG_LEN 100
#define BUF_SIZE 1024

int       server_fd, udp_fd, new_socket, new_socket2;
struct    sockaddr_in server_address, client_address;
struct    ServantData rcv_data;
int       addrlen = sizeof(server_address);
char      msg[MSG_LEN];
char      udp_buffer[BUF_SIZE] = {0};
char      tcp_buffer[BUF_SIZE] = {0};

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
            Register Client 1
        ----------------------*/
        printf("\nTCP Socket 1 Connected!\n");
        struct ServantData rcv_data;
        // time_t t = time(NULL);           

        recv(new_socket , &rcv_data, sizeof(rcv_data), 0);        // get obj
         rcv_data.GUID = reg.size+1;

<<<<<<< Updated upstream
<<<<<<< Updated upstream
        rcv_data.GUID = reg.size+1;                               // GUID = size of reg cause unique
        rcv_data.time = gmtime(&t);                               // set time stamp
=======
=======
>>>>>>> Stashed changes
        time(&current_time);
        time_info = localtime(&current_time);
        strftime(rcv_data.time_string, sizeof(rcv_data.time_string), "%H:%M:%S", time_info);
        // rcv_data.GUID = reg.size+1;                               // GUID = size of reg cause unique
        // printf("GUID f: %d", rcv_data.GUID);
        // rcv_data.time = gmtime(&t);                              // set time stamp
<<<<<<< Updated upstream
>>>>>>> Stashed changes
=======
>>>>>>> Stashed changes
        rcv_data.alive = TRUE;                                    // mark this client alive
        reg.servants[reg.size++] = rcv_data;                      //transfer client data to server registry
        print(rcv_data.GUID-1);                                    // print client information

        // printf("client 1 guid: %d\n", rcv_data.GUID);
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
            Register Client 2
        ----------------------*/
        printf("\nTCP Socket 2 Connected!\n");
        struct ServantData rcv_data;
        // time_t t = time(NULL);

        recv(new_socket2 , &rcv_data, sizeof(rcv_data), 0);        // get obj

        rcv_data.GUID = reg.size+1;                                // GUID = size of reg cause unique
<<<<<<< Updated upstream
<<<<<<< Updated upstream
        rcv_data.time = gmtime(&t);                                // set time stamp
=======
=======
>>>>>>> Stashed changes
        time(&current_time);
        time_info = localtime(&current_time);
        strftime(rcv_data.time_string, sizeof(rcv_data.time_string), "%H:%M:%S", time_info);
        // rcv_data.time = gmtime(&t);                                // set time stamp
<<<<<<< Updated upstream
>>>>>>> Stashed changes
=======
>>>>>>> Stashed changes
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
        
        int valread = recv( new_socket , tcp_buffer, BUF_SIZE, 0); //store requested file in tcp_buffer
        // int valread2 = recv( new_socket2 , tcp_buffer, BUF_SIZE, 0);

        printf("\nvalread: %d\n", valread);
<<<<<<< Updated upstream



=======
        printf("\nvalread2: %d\n", valread2);
        //Error checking
        // if(valread < 0){
        //     perror("Error: "); 
        // }
        // if(valread2 < 0){
        //     perror("Error: "); 
        // }
        printf("dog.txt: %s\n",reg.servants[0].my_file);
<<<<<<< Updated upstream
>>>>>>> Stashed changes
=======
>>>>>>> Stashed changes
        //check if client wants to search for file
        if(valread > 0){
            printf("Requested file: %s\n", tcp_buffer);
            //search registry 
            char GUID[10];
            sprintf(GUID, "%d", search_registry(tcp_buffer)); 
            printf("GUID before sending: %s\n", GUID);
            send(new_socket, GUID, strlen(GUID), 0 ); //send guid of client with desired file( 0 if file not in registry)
        }
        
                                     
        bzero(tcp_buffer, sizeof(tcp_buffer));     //display message                     //flush buffer
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
        n = recvfrom(udp_fd, (char *)udp_buffer, BUF_SIZE, MSG_WAITALL, (struct sockaddr *) &client_address, &len); 
        udp_buffer[n] = '\0'; 
        // printf("Ping from Client %s @ ", udp_buffer); 
        int GUID = udp_buffer[0] - '0';                       

        char _current_time[9];
        time(&current_time);
        time_info = localtime(&current_time);
        strftime(_current_time, sizeof(_current_time), "%H:%M:%S", time_info);

        // check who sent the UDP message
        // int CLIENT_NO = udp_buffer[0] - '0';                        // convert ASCII to int  
        // printf("client id = %d", CLIENT_NO);

        // check_clients_if_alive_or_dead(CLIENT_NO);
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

                //printf("\n\nclient %d\n timestamp %d\n current time %d\n\n",client_no+1,timestamp,current_time);

                if(abs(timestamp - current_time) >= END_TIME){
                    //printf("\n calculating \n");
                    reg.servants[client_no] = (struct ServantData){ .GUID = 0, .my_file = "", .time_string = "", .alive = FALSE };
                    printf("\nremoving %d from registry\n",client_no+1);
                }
            }
            else{
                 //printf("\nupdating client %d time to ",i+1);
                update_time(client_no);
                printf(" @time ");  
                puts(reg.servants[client_no].time_string);
            }
        }


        bzero(udp_buffer, sizeof(udp_buffer));
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