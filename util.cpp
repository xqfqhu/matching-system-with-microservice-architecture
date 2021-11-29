#include "util.h"

void unixError(char *msg){
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(0);
}
ssize_t Send(int sockfd, const void *buf, size_t len, int flags){
    ssize_t sendCount;
    if ((sendCount = send(sockfd, buf, len, flags)) < 0){
        unixError("Send error");
    }
    return sendCount;
}
ssize_t Sendto(int sockfd, const void *buf, size_t len, int flags,
               const struct sockaddr *dest_addr, socklen_t addrlen){
    ssize_t sendCount;
    if (sendCount = sendto(sockfd, buf, len, flags, dest_addr, addrlen) < 0){
        unixError("Sendto error");
    }
}
int openServerfd(const char * port, int socktype){
    struct addrinfo hints, *serviceinfo, * s;
    int serverfd, status;
    int optval = 1;

    /* initialize hints */
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = socktype; 
    hints.ai_flags = AI_PASSIVE | AI_ADDRCONFIG; // accept connection from any IP address
    hints.ai_flags |= AI_NUMERICSERV; // numeric port
    
    /* get a list of addrinfo that might work */
    if ((status = getaddrinfo(NULL, port, &hints, &serviceinfo)) != 0){
        fprintf(stderr, "getaddrinfo failed (port %s): %s\n", port, gai_strerror(status));
        return -2;
    }

    /* walk the list for one that we can bind to */
    for (s = serviceinfo; s; s = s->ai_next){
        if ((serverfd = socket(s->ai_family, s->ai_socktype, s->ai_protocol)) < 0){
            continue;
        }
        /* enable address reusage to avoid address already in use error */
        setsockopt(serverfd, SOL_SOCKET, SO_REUSEADDR,(const void *) &optval, sizeof(int));

        if (bind(serverfd, s->ai_addr, s->ai_addrlen) == 0){// listen on every available network interface
            break;
        }

        if (close(serverfd) < 0){
            fprintf(stderr, "openServerfd: close failed: %s\n", strerror(errno));
            return -1;
        }
    }

    /* cleanup to avoid memory leak */
    freeaddrinfo(serviceinfo);
    if (!s){
        return -1;
    }


    /* make the socket a listening socket if TCP*/
    if (socktype == SOCK_STREAM){
        if (listen(serverfd, MAXLISTENQ) < 0){
            close(serverfd);
            return -1;
        }
    }
    return serverfd;
};

int OpenServerfd(const char * port, int socktype){
    int serverfd = openServerfd(port, socktype);

    if (serverfd < 0){
        unixError("Openserverfd error.");
    }
    return serverfd;
}

int openClientfd(const char * serverHostname, const char * serverPort, const char * clientPort, int socktype){
    int clientfd, status, optval = 1;;
    struct addrinfo hints, *serviceinfoServer, * s, *serviceinfoClient, *c;

    /* initialize hint */
    memset(&hints, 0, sizeof(struct addrinfo)); 
    hints.ai_socktype = socktype; 
    hints.ai_flags = AI_NUMERICSERV; // numeric port
    hints.ai_flags |= AI_ADDRCONFIG; // does not return ipv6 if ipv6 is not supported

    /* get a list of addrinfo structure given host, port, and hints */
    if ((status = getaddrinfo(serverHostname, serverPort, &hints, &serviceinfoServer)) != 0){ 
        fprintf(stderr, "getaddrinfo failed (%s:%s): %s\n", serverHostname, serverPort, gai_strerror(status));
        return -2;
    }

    if (clientPort != NULL){
        if ((status = getaddrinfo(LOCALHOST, clientPort, &hints, &serviceinfoClient)) != 0){ 
            fprintf(stderr, "getaddrinfo failed (%s:%s): %s\n", LOCALHOST, clientPort, gai_strerror(status));
            return -2;
        }
    }

    /* try to connect with each addrinfo struct, stop on success */
    for (s = serviceinfoServer; s; s = s->ai_next){ 
        if ((clientfd = socket(s->ai_family, s->ai_socktype, s->ai_protocol)) < 0){
            continue;
        }

        if (clientPort != NULL){
            /* enable address reusage to avoid address already in use error */
            setsockopt(clientfd, SOL_SOCKET, SO_REUSEADDR,(const void *) &optval, sizeof(int));
            
            for (c = serviceinfoClient; c; c = c->ai_next){
                if (bind(clientfd, c->ai_addr, c->ai_addrlen) == 0){
                    break;
                }
            }
        }
        if (connect(clientfd, s->ai_addr, s->ai_addrlen) == 0){
            break;
        }

        if (close(clientfd) < 0){
            fprintf(stderr, "openClientfd: close failed: %s\n", strerror(errno));
            return -1;
        }
    }
    /* cleanup to prevent memory leak */
    freeaddrinfo(serviceinfoServer);
    if (clientPort != NULL){
        freeaddrinfo(serviceinfoClient);
    }

    /* return clientfd on success, return -1 on failure */
    if (!s || !c){
        return -1;
    }
    else{
        return clientfd;
    }
}
int OpenClientfd(const char * serverHostname, const char * serverPort, const char * clientPort, int socktype){
    int status = openClientfd(serverHostname, serverPort, clientPort, socktype);
    if (status < 0){
        unixError("Open client fd failed");
    }
    return status;
}

void bufferRecvInit(bufferRecv_t * brp, int fd){
    brp->fd = fd;
    brp->cnt = 0;
    brp->ptr = brp->buf;
}
void BufferRecvInit(bufferRecv_t * brp, int fd){
    bufferRecvInit(brp, fd);
}
ssize_t bufferRecv(bufferRecv_t * brp, char * usrbuf, size_t n){
    int cnt;
    if (brp->cnt <= 0){
        brp->cnt = recv(brp->fd, brp->buf, sizeof(brp->buf), 0);

        if (brp->cnt < 0){
            if (errno != EINTR){ // if refilling failed because of external signal, return
                return -1; 
            }
        }
        else if (brp->cnt == 0){ // EOF
            return 0; 
        }
        else{
            brp->ptr = brp->buf; // reset internal pointer
        }
    }
    (n < brp->cnt)? (cnt = n) : (cnt = brp->cnt);
    memcpy(usrbuf, brp->ptr,cnt);
    brp->ptr += cnt;
    brp->cnt -= cnt;
    return cnt;
}
ssize_t BufferRecv(bufferRecv_t * brp, char * usrbuf, size_t n){
    ssize_t rc = bufferRecv(brp, usrbuf, n);
    if (rc < 0){
        unixError("BufferRead error");
    }
    return rc;
}
ssize_t bufferRecvWord(bufferRecv_t *brp, void * usrbuf, size_t maxlen, char delimiter){
    int rc, i;
    char c, * cur = (char *) usrbuf;
    for (i = 0; i < maxlen; i++){
        if ((rc = bufferRecv(brp, &c, 1)) == 1){
            *cur = c;
            cur++;
            if (c == delimiter){
                i++;
                break;
            }
        }
        else if (rc == 0){
            if (i == 0) return 0;
            else break;
        }
        else{
            return -1;
        }
    }
    return i;
}

ssize_t BufferRecvWord(bufferRecv_t *brp, void * usrbuf, size_t maxlen, char delimiter){
    ssize_t rc = bufferRecvWord(brp, usrbuf, maxlen, delimiter);
    if (rc < 0){
        unixError("BufferRecvWord error");
    }
    return rc;
}


void bufferRecvfromInit(bufferRecvfrom_t * brp, int fd, struct sockaddr * src_addr, socklen_t *addrlen){
    brp->fd = fd;
    brp->cnt = 0;
    brp->ptr = brp->buf;
    brp->src_addr = src_addr;
    brp->addrlen = addrlen;
}
void BufferRecvfromInit(bufferRecvfrom_t * brp, int fd, struct sockaddr * src_addr, socklen_t *addrlen){
    bufferRecvfromInit(brp, fd, src_addr, addrlen);
}
ssize_t bufferRecvfrom(bufferRecvfrom_t * brp, char * usrbuf, size_t n){
    int cnt;
    if (brp->cnt <= 0){
        brp->cnt = recvfrom(brp->fd, brp->buf, sizeof(brp->buf), 0, brp->src_addr, brp->addrlen);

        if (brp->cnt < 0){
            if (errno != EINTR){ // if refilling failed because of external signal, return
                return -1; 
            }
        }
        else if (brp->cnt == 0){ // EOF
            return 0; 
        }
        else{
            brp->ptr = brp->buf; // reset internal pointer
        }
    }
    (n < brp->cnt)? (cnt = n) : (cnt = brp->cnt);
    memcpy(usrbuf, brp->ptr,cnt);
    brp->ptr += cnt;
    brp->cnt -= cnt;
    return cnt;
}
ssize_t BufferRecvfrom(bufferRecvfrom_t * brp, char * usrbuf, size_t n){
    ssize_t rc = bufferRecvfrom(brp, usrbuf, n);
    if (rc < 0){
        unixError("BufferRead error");
    }
    return rc;
}
ssize_t bufferRecvfromWord(bufferRecvfrom_t *brp, void * usrbuf, size_t maxlen, char delimiter){
    int rc, i;
    char c, * cur = (char *) usrbuf;
    for (i = 0; i < maxlen; i++){
        if ((rc = bufferRecvfrom(brp, &c, 1)) == 1){
            *cur = c;
            cur++;
            if (c == delimiter){
                i++;
                break;
            }
        }
        else if (rc == 0){
            if (i == 0) return 0;
            else break;
        }
        else{
            return -1;
        }
    }
    return i;
}

ssize_t BufferRecvfromWord(bufferRecvfrom_t *brp, void * usrbuf, size_t maxlen, char delimiter){
    ssize_t rc = bufferRecvfromWord(brp, usrbuf, maxlen, delimiter);
    if (rc < 0){
        unixError("BufferRecvWord error");
    }
    return rc;
}
void Close(int fd){
    int status;
    status = close(fd);
    if (status < 0){
        unixError("Close error");
    }
}

int Accept(int sockfd, struct sockaddr * addr, socklen_t * addrlen){
    int childfd;
    childfd = accept(sockfd, addr, addrlen);
    if (childfd < 0){
        unixError("Accept error");
    }
    return childfd;
}

void Getnameinfo(const struct sockaddr * addr, socklen_t addrlen,
                       char * host, socklen_t hostlen,
                       char * serv, socklen_t servlen, int flags){
    int status;
    status = getnameinfo(addr, addrlen, host, hostlen, serv, servlen, flags);
    if (status != 0){
        fprintf(stderr, "%s: %s\n", "Getnameinfo error", gai_strerror(status));
        exit(0);
    }
}