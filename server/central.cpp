#include "central.h"

int main(){
    
    int listenfdA, listenfdB, childfdA, childfdB, clientfdUdp, status;
    struct sockaddr_storage clientaddrA, clientaddrB, clientaddrRecvfrom;
    socklen_t clientlenA = sizeof(struct sockaddr_storage);
    socklen_t clientlenB = sizeof(struct sockaddr_storage);
    socklen_t clientaddrlenRecvfrom = sizeof(struct sockaddr_storage);
    char clientHostnameA[MAXLINE], clientHostnameB[MAXLINE], clientPortA[MAXLINE], clientPortB[MAXLINE];
    char usrA[MAXLINE], usrB[MAXLINE];
    char usrAWithFlag[MAXLINE], usrBWithFlag[MAXLINE];
    char graph1[MAXLINE];
    char graph2[MAXLINE];
    int graphSize1;
    int graphSize2;
    char scores[MAXLINE];
    int scoresSize;
    char match[MAXLINE];
    int matchSize;
    struct addrinfo hints, *serviceinfoT, *serviceinfoS, *serviceinfoP;

    /* open sockets */
    listenfdA = OpenServerfd(PORT_C_SERVE_A, SOCK_STREAM);
    listenfdB = OpenServerfd(PORT_C_SERVE_B, SOCK_STREAM);
    clientfdUdp = OpenServerfd(PORT_C_CLIENT2_TSP, SOCK_DGRAM);

    /* get udp sendto and recvfrom addresses */
    
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_DGRAM; 
    
    
    if ((status = getaddrinfo(LOCALHOST, PORT_T_SERVE_C, &hints, &serviceinfoT)) != 0){
        fprintf(stderr, "getaddrinfo failed (host %s:port %s): %s\n", LOCALHOST, PORT_T_SERVE_C, gai_strerror(status));
        exit(1);
    }
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_DGRAM; 
    if ((status = getaddrinfo(LOCALHOST, PORT_S_SERVE_C, &hints, &serviceinfoS)) != 0){
        fprintf(stderr, "getaddrinfo failed (host %s:port %s): %s\n", LOCALHOST, PORT_S_SERVE_C, gai_strerror(status));
        exit(1);
    }
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_socktype = SOCK_DGRAM; 
    if ((status = getaddrinfo(LOCALHOST, PORT_P_SERVE_C, &hints, &serviceinfoP)) != 0){
        fprintf(stderr, "getaddrinfo failed (host %s:port %s): %s\n", LOCALHOST, PORT_P_SERVE_C, gai_strerror(status));
        exit(1);
    }

    fprintf(stdout, "The Central server is up and running.\n");
    while (1){
        /* initialize results*/
        memset(graph1, 0, sizeof(graph1));
        memset(graph2, 0, sizeof(graph2));
        memset(match, 0, sizeof(match));
        memset(scores, 0, sizeof(scores));

        /* accept requests from clientA and clientB */
        childfdA = Accept(listenfdA, (struct sockaddr *) &clientaddrA, &clientlenA);

        Getnameinfo((struct sockaddr *) &clientaddrA, clientlenA, clientHostnameA, MAXLINE, 
                    clientPortA, MAXLINE, 0);

        readUsrname(childfdA, usrA, MAXLINE);
        
        fprintf(stdout, "The Central server received input=%s from the client using TCP over port %s.\n", usrA, clientPortA);
        childfdB = Accept(listenfdB, (struct sockaddr *) &clientaddrB, &clientlenB);
        
        
        Getnameinfo((struct sockaddr *) &clientaddrB, clientlenB, clientHostnameB, MAXLINE, 
        clientPortB, MAXLINE, 0);
        
        
        readUsrname(childfdB, usrB, MAXLINE);
        fprintf(stdout, "The Central server received input=%s from the client using TCP over port %s.\n", usrB, clientPortB);


        /* send a request to backend server T */
        Sendto(clientfdUdp, usrA, strlen(usrA) + 1, 0, serviceinfoT->ai_addr, serviceinfoT->ai_addrlen);
        Sendto(clientfdUdp, usrB, strlen(usrB) + 1, 0, serviceinfoT->ai_addr, serviceinfoT->ai_addrlen);
        fprintf(stdout, "The Central server sent a request to Backend-Server T\n");
        
        /* receive results from backen server T */
        graphSize1 = recvfrom(clientfdUdp, graph1, MAXLINE, 0, (struct sockaddr *) &clientaddrRecvfrom, &clientaddrlenRecvfrom);
        graphSize2 = recvfrom(clientfdUdp, graph2, MAXLINE, 0, (struct sockaddr *) &clientaddrRecvfrom, &clientaddrlenRecvfrom);
        fprintf(stdout, "The Central server received information from Backend-Server T using UDP over port %s.\n", PORT_C_CLIENT2_TSP);

        /* send a request to backend server S */
        
        if (graph1[0] == '1'){//send node info
            Sendto(clientfdUdp, graph1 + 2, graphSize1 - 2, 0, serviceinfoS->ai_addr, serviceinfoS->ai_addrlen);
        }
        else{
            Sendto(clientfdUdp, graph2 + 2, graphSize2 - 2, 0, serviceinfoS->ai_addr, serviceinfoS->ai_addrlen);
        }
        
        fprintf(stdout, "The Central server sent a request to Backend-Server S\n");
        /* receive scores from backend server S */
        scoresSize = recvfrom(clientfdUdp, scores, MAXLINE, 0, (struct sockaddr *) &clientaddrRecvfrom, &clientaddrlenRecvfrom);
        fprintf(stdout, "The Central server received information from Backend-Server S using UDP over port %s.\n", PORT_C_CLIENT2_TSP);


        /* send a request to backend server P */
        char * ptr = usrAWithFlag;
        char flag[] = {'A','\0'};
        memcpy(ptr, flag, strlen(flag) + 1);
        ptr += (strlen(flag) + 1);
        memcpy(ptr, usrA, strlen(usrA) + 1);

        ptr = usrBWithFlag;
        flag[0] = 'B';
        memcpy(ptr, flag, strlen(flag) + 1);
        ptr += (strlen(flag) + 1);
        memcpy(ptr, usrB, strlen(usrB) + 1);

        Sendto(clientfdUdp, usrAWithFlag, strlen(usrA) + strlen(flag) + 2, 0, serviceinfoP->ai_addr, serviceinfoP->ai_addrlen);
        Sendto(clientfdUdp, usrBWithFlag, strlen(usrB) + strlen(flag) + 2, 0, serviceinfoP->ai_addr, serviceinfoP->ai_addrlen);
        if (graph1[0] == '1'){//send edge info
            Sendto(clientfdUdp, graph2, graphSize2, 0, serviceinfoP->ai_addr, serviceinfoP->ai_addrlen);
        }
        else{
            Sendto(clientfdUdp, graph1, graphSize1, 0, serviceinfoP->ai_addr, serviceinfoP->ai_addrlen);
        }
        Sendto(clientfdUdp, scores, scoresSize, 0, serviceinfoP->ai_addr, serviceinfoP->ai_addrlen);
        fprintf(stdout, "The Central server sent a processing request to Backend-Server P.\n");
        matchSize = recvfrom(clientfdUdp, match, MAXLINE, 0, (struct sockaddr *) &clientaddrRecvfrom, &clientaddrlenRecvfrom);
        fprintf(stdout, "The Central server received the results from backend server P.\n");

        /* send match result to clients */
        Send(childfdA, match, matchSize, 0);
        fprintf(stdout, "The Central server sent the results to client A.\n");
        Send(childfdB, match, matchSize, 0);
        fprintf(stdout, "The Central server sent the results to client B.\n");
        Close(childfdA);
        Close(childfdB);

    }

}

void readUsrname(int fd, char * usrname, int size){
    bufferRecv_t br;
    char delimiter = '\0';
    bufferRecvInit(&br, fd);
    
    if (bufferRecvWord(&br, usrname, size, delimiter) < 0){
        fprintf(stderr, "error: central received invalid input from client\n");
        exit(1);
    }
    return;
}