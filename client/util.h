#ifndef __CLIENTCOMMON__
#define __CLIENTCOMMON__
#include "../util.h"

typedef struct hop_t{
    hop_t * next;
    hop_t * prev;
    char usr[MAXLINE];
} hop_t;
class match_t{
    public:
        char usrA[MAXLINE];
        char usrB[MAXLINE];
        double gap;
        int numHop;
        hop_t * head;
        hop_t * tail;

        match_t();
        ~match_t();
};

void readMatchResult(int fd, match_t * matchResult);
void printOnScreenMessage(FILE * stream, match_t * matchResult, bool reverse);
void client(const char * serverHost, const char * serverPort, char * usrname, bool reverse);
#endif