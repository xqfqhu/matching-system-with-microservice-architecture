#include "util.h"
match_t::match_t(){
    this->numHop = 0;
    this->head = NULL;
    this->tail = NULL;
}
match_t::~match_t(){
    hop_t * next;
    hop_t * cur = head;
    for (int i = 0; i < this->numHop; i++){
        next = head->next;
        delete cur;
        cur = next;
        
    }
}
void readMatchResult(int fd, match_t * matchResult){
    bufferRecv_t br;
    char buffer[MAXLINE];
    char delimiter = '\0';
    bufferRecvInit(&br, fd);
    
    /* read numHop */
    if (bufferRecvWord(&br, buffer, MAXLINE, delimiter) < 0){
        fprintf(stderr, "error: invalid return value, empty return value\n");
        exit(1);
    }
    matchResult->numHop = atoi(buffer);

    /* read usrA and usrB */
    if (bufferRecvWord(&br, matchResult->usrA, MAXLINE, delimiter) < 0){
        fprintf(stderr, "error: invalid return value, usrA missing\n");
        exit(1);
    }
    if (bufferRecvWord(&br, matchResult->usrB, MAXLINE, delimiter) < 0){
        fprintf(stderr, "error: invalid return value, usrB missing\n");
        exit(1);
    }

    /* read hops if exist*/
    for (int i = 0; i < matchResult->numHop; i++){
        int size = bufferRecvWord(&br, buffer, MAXLINE, delimiter);
        hop_t * curHop = new hop_t;
        memcpy(curHop->usr, buffer, size);
        curHop->next = NULL;
        curHop->prev = matchResult->tail;
        if (matchResult->head == NULL){
            matchResult->head = curHop;
            matchResult->tail = curHop;
        }
        else{
            matchResult->tail->next = curHop;
            matchResult->tail = curHop;
        }
    }

    /* read gap if exists */
    if (matchResult->numHop != -1){
        if (bufferRecvWord(&br, buffer, MAXLINE, delimiter) < 0){
            fprintf(stderr, "error: invalid return value, gap missing\n");
            exit(1);
        }
        matchResult->gap = atof(buffer);
    }
    return;
}

void printOnScreenMessage(FILE * stream, match_t * matchResult, bool reverse){
    /* print message if no compatibility */
    if (matchResult->numHop == -1){
        fprintf(stream, "Found no compatibility for %s and %s\n", matchResult->usrA, matchResult->usrB);
        return;
    }

    /* print message if at least one match found */
    if (reverse){
        fprintf(stream, "Found compatibility for %s and %s:\n", matchResult->usrB, matchResult->usrA);
        fprintf(stream, "%s --- ", matchResult->usrB);
        hop_t * cur = matchResult->tail;
        for (int i = 0; i < matchResult->numHop; i++){
            fprintf(stream, "%s --- ", cur->usr);
            cur = cur->prev;
        }
        fprintf(stream, "%s\n", matchResult->usrA);
    }
    else{
        fprintf(stream, "Found compatibility for %s and %s:\n", matchResult->usrA, matchResult->usrB);
        fprintf(stream, "%s --- ", matchResult->usrA);
        hop_t * cur = matchResult->head;
        for (int i = 0; i < matchResult->numHop; i++){
            fprintf(stream, "%s --- ", cur->usr);
            cur = cur->next;
        }
        fprintf(stream, "%s\n", matchResult->usrB);
    }
    
    fprintf(stream, "Matching Gap : %.2f\n", matchResult->gap);
    return;
}

void client(const char * serverHost, const char * serverPort, char * usrname, bool reverse){
    int clientfd2C;
    struct sockaddr_storage clientaddr;
    size_t username_size;
    match_t matchResult;
    /* sanity check */
    
    fprintf(stdout,"The client is up and running.\n");
    username_size = strlen(usrname) + 1;
    

    /* open client socket */
    
    clientfd2C = OpenClientfd(serverHost, serverPort, NULL, SOCK_STREAM);

    /* write to client socket */
    Send(clientfd2C, usrname, username_size, 0);
    fprintf(stdout, "The client sent %s to the Central server.\n", usrname);
    
    /* read return value */
    readMatchResult(clientfd2C, &matchResult);

    /* print on screen messages */
    printOnScreenMessage(stdout, &matchResult, reverse);
    Close(clientfd2C);
    return;
}