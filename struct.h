//===============================================================================================
//HW 4:     P2P
//Author:   Florence Yao
//Course:   CECS-327
//Date:     4-10-19
//Info: 
//===============================================================================================
#define NUM_CLIENTS 2

struct ServantData{
    uint32_t GUID;
    //int last_update;
    //const char *my_file[50];
    char my_file[50];

    //char* my_files[10];
    struct tm *time;
    uint32_t alive;
};

struct Registry{
    struct ServantData servants[NUM_CLIENTS];
    int size;
};

struct Registry reg;