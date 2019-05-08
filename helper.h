//===============================================================================================
//HW 4:     P2P
//Author:   Florence Yao
//Course:   CECS-327
//Date:     4-10-19
//Info: 
//===============================================================================================
#include "struct.h"
#include "time.h"
#include <stdio.h>
#include <stdlib.h>
#define TRUE 1
#define FALSE 0
#define END_TIME 10
#define SECONDS 60
#define NUM_CLIENTS 2
#define NUM_FILES 1
#define MSG_LEN 50
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
// 
//
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

    if(index < 0){
        printf("Invalid GUID");
        exit(0);
    }

    if(reg.servants[index].alive == TRUE){
        printf("Client %d GUID: %d", 
                index+1, reg.servants[index].GUID); // client_no is passed in as size of array 
                                                    // size of array was 0 when first called, so +1 
                                                    // to make the client GUID start at 1
        printf("\nClient %d files: %s", 
                index+1, reg.servants[index].my_file);


        // printf("\nClient %d files %s", 
        //         GUID+1, reg.servants[GUID].my_files[0]);

        printf("\nClient %d timestamp ", index+1);
        puts(reg.servants[index].time_string);     
    }
    
    printf("\n====================================\n");
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
// function: menu
// purpose: Print out menu options for user
// returns: choice(int)
//================================================================================
int menu(){
    int choice = 0;
    int max = 10;
    char input[max];

    printf("\n====================================\n");
    printf("    1. Find file\n");
    printf("    2. Exit\n");
    printf("====================================\n");
    printf("Please enter the number of your choice: ");
    fgets(input,max, stdin);
    sscanf(input, "%d", &choice);
    // printf("Choice: %d", choice);
    // printf("Choice: %d\n", choice);
    bzero(input, sizeof(input));
    return choice;
}

//================================================================================
// function: search_registry()
// purpose: search registry for desired file
// returns: client_guid (zero if file is not in registry)
//================================================================================

int search_registry(char fileName[BUF_SIZE]){
    int GUID = 0; //guid of client with file

    printf("Got into search registry\n");
    
    for(int i = 0; i < reg.size; i++){
        // printf("Got into for loop\n");
        if( strcmp(fileName ,reg.servants[i].my_file ) == 0 ){
            printf("Found file!\n");
            GUID = reg.servants[i].GUID; //servant[0].GUID is 1
            break;
        }
    }

    printf("exiting search reg\n");
    return GUID;
}

void copy(struct ServantData* dest, struct ServantData* src){
    for(int i=0; i<reg.size; i++){
        dest[i].GUID = src[i].GUID;
        dest[i].alive = src[i].alive;
        strcpy(dest[i].my_file, src[i].my_file);
        strcpy(dest[i].time_string, src[i].time_string);
    }
}

//================================================================================
// function: remove_client
// inputs: index of client to be removed, temporary new  array
// this function uses the temporary new array to add all copies of
// alive clients. the client to be removed is marked dead, so it 
// doesn't get copied.
//================================================================================
void remove_client(int client_no, struct ServantData temp[NUM_CLIENTS]){
    //struct ServantData temp[NUM_CLIENTS];

    for(int i=0, j=0; i<reg.size; i++){
        if(i == client_no){ // if this is the client, skip it 
            printf("\nclient %d .... removing",reg.servants[client_no].GUID);
            continue;
        }
        else{
            temp[j].GUID = reg.servants[i].GUID;
            temp[j].alive = reg.servants[i].alive;
            strcpy(temp[j].my_file, reg.servants[i].my_file);
            strcpy(temp[j].time_string, reg.servants[i].time_string);
            j++;
        }
    }
}