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
#define END_TIME 30
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
void update_time(int index){
    // time_t t = time(NULL);
    // reg.servants[index].time->tm_hour = gmtime(&t)->tm_hour;
    // reg.servants[index].time->tm_min = gmtime(&t)->tm_min;
    // reg.servants[index].time->tm_sec = gmtime(&t)->tm_sec;

    time_t current_time;
    struct tm * time_info;
    time(&current_time);
    time_info = localtime(&current_time);
    strftime(reg.servants[index].time_string, sizeof(reg.servants[index].time_string), "%H:%M:%S", time_info);
    //puts(reg.servants[index].time_string);                               // GUID = size of reg cause unique
}

//================================================================================
// Function: convert_to_seconds
// Purpose: I'm extracting the 10s digit of the minutes and both digits of seconds
//          because we need to know every 200s. If I convert the time to seconds
//          it's a matter of simple arithmetic to calculate if 200s has passed
//================================================================================
int convert_to_seconds(char time_string[9]){
    int min;

    // 1:45:09
    // extract 10s digit of the time which is 5
    // if that digit is 0, the value is 10 since ASCII is 0-9
    if(time_string[4] == '0'){
        min = 10;
    }
    else{ // else convert the char to int  '9' -> 9
        min = time_string[4] - '0';
    }

    // 1:45:09
    // extract 10s digit of the time which is 0 and 9
    int temp = (time_string[6] - '0') * 10;
    int temp2 = time_string[7] - '0';
    int sec = temp + temp2;

    // return the conversion to seconds
    return SECONDS * min + sec;
}

//================================================================================
// Function: print
// Purpose: prints the client GUID's and 
//          files that the client has
//          client's timestamp
//================================================================================
void print(int index){
    printf("\n====================================\n");

    if(reg.servants[index].alive == TRUE){

        printf("Client %d GUID: %d", 
                index+1, reg.servants[index].GUID); // client_no is passed in as size of array 
                                                    // size of array was 0 when first called, so +1 
                                                    // to make the client GUID start at 1
        printf("\nClient %d files %s", 
                index+1, reg.servants[index].my_file);


        // printf("\nClient %d files %s", 
        //         GUID+1, reg.servants[GUID].my_files[0]);
        printf("\nClient %d timestamp ", index+1);
        puts(reg.servants[index].time_string);
    }
    printf("====================================\n");
}

//================================================================================
// function: alive
// returns 1 if client is alive
// returns 2 if client is dead
//================================================================================
int alive(int index){
    return reg.servants[index].alive;
}

//================================================================================
// function: client_is_registered
// returns 1 if client is registered
// returns 2 if client is not registered
//================================================================================
int client_is_registered(int GUID){
    for(int i=0; i<NUM_CLIENTS; i++){
        if(reg.servants[i].GUID == GUID){
            return TRUE;
        }
    }
    return FALSE;
}
