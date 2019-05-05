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

    char my_file[50];

    char time_string[9];

    uint32_t alive;
};

struct Registry{
    struct ServantData servants[NUM_CLIENTS];
    int size;
};

struct Registry reg;