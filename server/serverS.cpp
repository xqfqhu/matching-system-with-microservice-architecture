#include "serverS.h"
scores::scores(const char *filename){
    ifstream infile(filename);
    string line;
    
    while(getline(infile, line)){
        stringstream streamLine(line);
        string usr;
        string score;
        getline(streamLine, usr, ' ');
        getline(streamLine, score, ' ');
        
        scoresL[usr] = stoi(score);
    }
    infile.close();
    return;
}

void scores::queryScore(string & usr, vector<string> &usrs, vector<int> & vals){
    usrs.push_back(usr);
    vals.push_back(scoresL[usr]);
    return;
}

int main(){
    
    int status;
    
    struct sockaddr_storage clientaddrRecvfrom;
    socklen_t clientaddrlenRecvfrom = sizeof(struct sockaddr_storage);
    vector<string> usrs;
    vector<int> scoresL;
    char msg[MAXLINE];
    char usr[MAXLINE];
    int size;
    int numNode;
    
    /* build scores from file */
    scores myScores(SCORES_FILENAME);

    /* open socket */
    int serverfd = OpenServerfd(PORT_S_SERVE_C, SOCK_DGRAM);
    fprintf(stdout, "The ServerS is up and running using UDP on port %s\n", PORT_S_SERVE_C);

    while(1){
        /* recv request from Central */
        bufferRecvfrom_t br;
        char delimiter = '\0';
        bufferRecvfromInit(&br, serverfd, (struct sockaddr *) &clientaddrRecvfrom, &clientaddrlenRecvfrom);
        bufferRecvfromWord(&br, usr, MAXLINE, delimiter);
        numNode = atoi(usr);
        for (int i = 0; i< numNode; i++){
            bufferRecvfromWord(&br, usr, MAXLINE, delimiter);
            string usrString(usr);
            myScores.queryScore(usrString, usrs, scoresL);
        }
        fprintf(stdout, "The ServerS received a request from Central to get the scores.\n");
        /* build message from scores */
        memset(msg, 0, sizeof(msg));
        size = buildMessageFromVector(msg, usrs, scoresL);

        /* send message back to Central */
        Sendto(serverfd, msg, size, 0, (struct sockaddr *) &clientaddrRecvfrom, clientaddrlenRecvfrom);
        fprintf(stdout, "The ServerS finished sending the scores to Central.\n");
    }
}

int buildMessageFromVector(char * msg, vector<string> & usrs, vector<int> & scoresL){
    int msgSize = 0;
    int size = usrs.size();
    char * ptr = msg;
    
    bool flag = 1;

    sprintf(ptr, "%d", flag);
    msgSize += (strlen(ptr) + 1);
    ptr += (strlen(ptr) + 1);
    
    
    sprintf(ptr, "%d", size);
    msgSize += (strlen(ptr) + 1);
    ptr += (strlen(ptr) + 1);
    
    for (int i = 0; i < size; i++){
        memcpy(ptr, usrs[i].c_str(), usrs[i].size() + 1);
        ptr += (usrs[i].size() + 1);
        msgSize += (usrs[i].size() + 1);
        sprintf(ptr, "%d", scoresL[i]);
        msgSize += (strlen(ptr) + 1);
        ptr += (strlen(ptr) + 1);
        
    }
    return msgSize;
}