#include "serverT.h"

bool graph::ifexist(string &usr){
    if (parent.find(usr) == parent.end()){
        return false;
    }
    else{
        return true;
    }
}

void graph::add(string & usr){
    parent[usr] = usr;
    rank[usr] = 0;
    return;
}

string graph::find(string & usr){
    if (parent[usr] != usr){
        parent[usr] = find(parent[usr]);
    }
    return parent[usr];
}

void graph::myUnion(string &usrA, string &usrB){
    if (!ifexist(usrA)){
        add(usrA);
    }
    if (!ifexist(usrB)){
        add(usrB);
    }

    string parentA = find(usrA);
    string parentB = find(usrB);

    if (!parentA.compare(parentB)){
        return;
    }

    int rankA = rank[parentA];
    int rankB = rank[parentB];

    if (rankA >= rankB){
        parent[parentB] = parentA;
        if (rankA == rankB){
            rank[parentA]++;
        }
    }
    else{
        parent[parentA] = parentB;
    }
    return;
}

void graph::findCommonParent(string &usr, vector<string> & res){
    string usrParent = find(usr);
    for (auto & it:parent){
        if (!it.second.compare(usrParent)){
            res.push_back(it.first);
        }
    }
}

int buildMessageFromVector(char *msg, vector<string> & usr, bool nodeMsg){
    int msgSize = 0;
    int size = usr.size();
    char * ptr = msg;
    sprintf(ptr, "%d", nodeMsg);
    msgSize += (strlen(ptr) + 1);
    ptr += (strlen(ptr) + 1);
    sprintf(ptr, "%d", size);
    msgSize += (strlen(ptr) + 1);
    ptr += (strlen(ptr) + 1);
    
    for (int i = 0; i < size; i++){
        memcpy(ptr, usr[i].c_str(), usr[i].size() + 1);
        ptr += (usr[i].size() + 1);
        msgSize += (usr[i].size() + 1);
    }
    return msgSize;
}

graph::graph(const char * filename){
    ifstream infile(filename);
    string line;
    
    while(getline(infile, line)){
        stringstream streamLine(line);
        string usr1;
        string usr2;
        getline(streamLine, usr1, ' ');
        getline(streamLine, usr2, ' ');
        edge[usr1].push_back(usr2);
        myUnion(usr1, usr2);
    }
    infile.close();
    return;
}
void graph::findEdge(vector<string> & usrs, vector<string> &relatedEdge){
    for (auto & usr1: usrs){
        for (auto & usr2 : edge[usr1]){
            relatedEdge.push_back(usr1);
            relatedEdge.push_back(usr2);
        }
    }
    return;
}

int main(){
    
    
    int status;
    struct addrinfo hints, *serviceinfoC;
    char usrA[MAXLINE];
    char usrB[MAXLINE];
    struct sockaddr_storage clientaddrRecvfrom;
    socklen_t clientaddrlenRecvfrom = sizeof(struct sockaddr_storage);
    vector<string> nodeList;
    vector<string> edgeList;
    char nodeMsg[MAXLINE];
    char edgeMsg[MAXLINE];
    int sizeNodeMsg;
    int sizeEdgeMsg;
    /* build graph from file */
    graph myGraph(USR_FILENAME);
    
    /* open socket */
    int serverfd = OpenServerfd(PORT_T_SERVE_C, SOCK_DGRAM);
    fprintf(stdout, "The ServerT is up and running using UDP on port %s\n", PORT_T_SERVE_C);
    while (1){
        nodeList = {};
        edgeList = {};
        /* recv from central */
        bufferRecvfrom_t br;
        char delimiter = '\0';
        bufferRecvfromInit(&br, serverfd, (struct sockaddr *) &clientaddrRecvfrom, &clientaddrlenRecvfrom);

        if (bufferRecvfromWord(&br, usrA, MAXLINE, delimiter) < 0){
            fprintf(stderr, "error: serverT received invalid input from central\n");
            exit(1);
        }

        if (bufferRecvfromWord(&br, usrB, MAXLINE, delimiter) < 0){
            fprintf(stderr, "error: serverT received invalid input from central\n");
            exit(1);
        }
        fprintf(stdout, "The ServerT received a request from Central to get the topology.\n");
        /* find related subgraphs */
        string usrAString(usrA);
        string usrBString(usrB);
        if (!myGraph.find(usrAString).compare(myGraph.find(usrBString))){
            myGraph.findCommonParent(usrAString, nodeList);
        }
        else{
            myGraph.findCommonParent(usrAString, nodeList);
            myGraph.findCommonParent(usrBString, nodeList);
        }
        myGraph.findEdge(nodeList, edgeList);

        /* build message from graph */
        memset(nodeMsg, 0, sizeof(nodeMsg));
        memset(edgeMsg, 0, sizeof(edgeMsg));
        sizeNodeMsg = buildMessageFromVector(nodeMsg, nodeList, 1);
        sizeEdgeMsg = buildMessageFromVector(edgeMsg, edgeList, 0);

        /* send message back to central */
        Sendto(serverfd, nodeMsg, sizeNodeMsg, 0, (struct sockaddr *) &clientaddrRecvfrom, clientaddrlenRecvfrom);
        Sendto(serverfd, edgeMsg, sizeEdgeMsg, 0, (struct sockaddr *) &clientaddrRecvfrom, clientaddrlenRecvfrom);
        fprintf(stdout, "The ServerT finished sending the topology to Central.\n");
    }
}