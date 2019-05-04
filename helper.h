//===============================================================================================
//HW 4:     P2P
//Author:   Florence Yao
//Course:   CECS-327
//Date:     4-10-19
//Info: 
//===============================================================================================
#include "struct.h"
#include "time.h"
#define TRUE 1
#define FALSE 0
#define END_TIME 120
#define SECONDS 60
#define NUM_FILES 1
#define MSG_LEN 100
#define BUF_SIZE 1024


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
// function: update_time
// update timestamp of client
//================================================================================
void update_time(int GUID){
    time_t t = time(NULL);
    reg.servants[GUID].time->tm_hour = gmtime(&t)->tm_hour;
    reg.servants[GUID].time->tm_min = gmtime(&t)->tm_min;
    reg.servants[GUID].time->tm_sec = gmtime(&t)->tm_sec;
}

//================================================================================
// function: show_time
// print timestamp of client
//================================================================================
void show_time(int GUID){
    printf("%d:",  reg.servants[GUID].time->tm_hour);
    printf("%d:",  reg.servants[GUID].time->tm_min);
    printf("%d", reg.servants[GUID].time->tm_sec);
}

//================================================================================
// Function: print
// Purpose: prints the client GUID's and 
//          files that the client has
//          client's timestamp
//================================================================================
void print(int GUID){
    printf("\n====================================\n");

    if(reg.servants[GUID].alive == TRUE){

        printf("Client %d GUID: %d", 
                GUID+1, reg.servants[GUID].GUID); // client_no is passed in as size of array 
                                                    // size of array was 0 when first called, so +1 
                                                    // to make the client GUID start at 1
        printf("\nClient %d files %s", 
                GUID+1, reg.servants[GUID].my_file);


        // printf("\nClient %d files %s", 
        //         GUID+1, reg.servants[GUID].my_files[0]);

        printf("\nClient %d time %d:%d:%d", 
                GUID+1, reg.servants[GUID].time->tm_hour,
                        reg.servants[GUID].time->tm_min,
                        reg.servants[GUID].time->tm_sec);
    }
    printf("\n====================================\n");
}

//================================================================================
// function: alive
// returns 1 if client is alive
// returns 2 if client is deadqqqqqqq
//================================================================================
int alive(int GUID){
    return reg.servants[GUID].alive;
}

//================================================================================
// function: check_clients_if_alive_or_dead
//    1. Gets the udp message from each client
//    2. Check which client it came from
//    3. If client is alive then see when the last ping was
//    4. If it was less than X time then update time stamp
//    5. Else remove from registry and mark dead
//================================================================================
// void check_clients_if_alive_or_dead(int GUID, int pingtime){
    
//     for(int client_no=0; i<NUM_CLIENTS; client_no++){
//         if(reg.servants[i].GUID == GUID){
//             printf(" last updated ");
//             show_time(client_no);

//             // get current time and last updated time 

//             //int pingtime  = SECONDS * gmtime(&t)->tm_min + gmtime(&t)->tm_sec;
//             int timestamp = SECONDS * reg.servants[i].time->tm_min + reg.servants[i].time->tm_sec;

//             // this block checks the client is dead or alive
//             // convert to seconds for easy check
//             if((abs(timestamp - pingtime) < END_TIME)){

//                 update_time(client_no);

//                 printf(" updated to ");
//                 show_time(client_no);
//                 // make unavailable from registry
//             }
//             else{
//                 reg.servants[client_no] = (struct ServantData){ .GUID = 0, 
//                                                                 .my_file = "", 
//                                                                 .time = NULL,
//                                                                 .alive = FALSE };

//                 printf("\nremoving from registry\n");
//             }
//         }
//     }
// }
