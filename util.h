#ifndef __UTIL_H__
#define __UTIL_H__

/* common header files */
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <algorithm>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/wait.h>
/* misc constants */
const int MAXLINE = 8192;
const int MAXLISTENQ = 1024;
const char LOCALHOST[] = {'l','o','c','a','l','h','o','s','t', '\0'};
const char PORT_C_SERVE_A[] = {'2', '5', '6', '8', '5', '\0'};
const char PORT_C_SERVE_B[] = {'2', '6', '6', '8', '5', '\0'};
const char PORT_C_CLIENT2_TSP[] = {'2', '4', '6', '8', '5', '\0'};
const char PORT_T_SERVE_C[] = {'2', '1', '6', '8', '5', '\0'};
const char PORT_S_SERVE_C[] = {'2', '2', '6', '8', '5', '\0'};
const char PORT_P_SERVE_C[] = {'2', '3', '6', '8', '5', '\0'};

/* data structure */
typedef struct{
    char buf[MAXLINE]; // internal buffer
    int fd; // fd for the internal buffer
    int cnt; // number of unprocessed byte in the internal buffer
    char * ptr; // pointer to the next byte
} bufferRecv_t;
typedef struct{
    char buf[MAXLINE]; // internal buffer
    int fd; // fd for the internal buffer
    int cnt; // number of unprocessed byte in the internal buffer
    char * ptr; // pointer to the next byte
    struct sockaddr * src_addr;
    socklen_t * addrlen;
}bufferRecvfrom_t;

/* common utilities */
void unixError(char *msg); // prints errno


int openServerfd(const char * port, int socktype); 
int openClientfd(const char * serverHostname, const char * serverPort, const char * clientPort, int socktype);



void bufferRecvInit(bufferRecv_t * brp, int fd); // initialize buffered recv 
ssize_t bufferRecv(bufferRecv_t * brp, char * usrbuf, size_t n); // transfer min(n, brp->cnt) bytes from internal buffer to usrbuf. refill internal buffer when necessary
ssize_t bufferRecvWord(bufferRecv_t *brp, void * usrbuf, size_t maxlen, char delimiter); // transfer one word to the user buffer. words are separated by delimiter
void bufferRecvfromInit(bufferRecvfrom_t * brp, int fd, struct sockaddr *src_addr, socklen_t *addrlen); // initialize buffered recv 
ssize_t bufferRecvfrom(bufferRecvfrom_t * brp, char * usrbuf, size_t n); // transfer min(n, brp->cnt) bytes from internal buffer to usrbuf. refill internal buffer when necessary
ssize_t bufferRecvfromWord(bufferRecvfrom_t *brp, void * usrbuf, size_t maxlen, char delimiter);

/* wrappers */
ssize_t Send(int sockfd, const void *buf, size_t len, int flags);
ssize_t Sendto(int sockfd, const void *buf, size_t len, int flags,
               const struct sockaddr *dest_addr, socklen_t addrlen);
void Close(int fd);
int Accept(int sockfd, struct sockaddr * addr, socklen_t * addrlen);
void Getnameinfo(const struct sockaddr * addr, socklen_t addrlen,
                       char * host, socklen_t hostlen,
                       char * serv, socklen_t servlen, int flags);
int OpenServerfd(const char * port, int socktype); 
int OpenClientfd(const char * serverHostname, const char * serverPort, const char * clientPort, int socktype);
void BufferRecvInit(bufferRecv_t * brp, int fd);  
ssize_t BufferRecv(bufferRecv_t * brp, char * usrbuf, size_t n); 
ssize_t BufferRecvWord(bufferRecv_t *brp, void * usrbuf, size_t maxlen, char delimiter);
void BufferRecvfromInit(bufferRecvfrom_t * brp, int fd, struct sockaddr *src_addr, socklen_t *addrlen);  
ssize_t BufferRecvfrom(bufferRecvfrom_t * brp, char * usrbuf, size_t n); 
ssize_t BufferRecvfromWord(bufferRecvfrom_t *brp, void * usrbuf, size_t maxlen, char delimiter);
#endif