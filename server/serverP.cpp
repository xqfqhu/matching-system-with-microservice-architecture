#include "serverP.h"

processor::processor(int serverfd){
    
    char bufferHeader[MAXLINE];
    char buffer[MAXLINE];
    bufferRecvfrom_t br;
    char delimiter = '\0';
    int numEdge = 0;
    int numNode = 0;

    fd = serverfd;
    
    clientaddrlenRecvfrom = sizeof(struct sockaddr_storage);

    bufferRecvfromInit(&br, fd, (struct sockaddr *) &clientaddrRecvfrom, &clientaddrlenRecvfrom);
    for (int i = 0; i < 4; i++){
        bufferRecvfromWord(&br, bufferHeader, MAXLINE, delimiter);
        if (bufferHeader[0] == 'A'){
            bufferRecvfromWord(&br, buffer, MAXLINE, delimiter);
            usrA = buffer;
        }
        else if (bufferHeader[0] == 'B'){
            bufferRecvfromWord(&br, buffer, MAXLINE, delimiter);
            usrB = buffer;
        }
        else if (bufferHeader[0] == '0'){ //edge
            bufferRecvfromWord(&br, buffer, MAXLINE, delimiter);
            numEdge = atoi(buffer) / 2;
            for (int j = 0; j < numEdge; j++){
                string nodeA, nodeB;
                bufferRecvfromWord(&br, buffer, MAXLINE, delimiter);
                nodeA = buffer;
                bufferRecvfromWord(&br, buffer, MAXLINE, delimiter);
                nodeB = buffer;
                adj[nodeA].push_back({DBL_MAX, nodeB});
                adj[nodeB].push_back({DBL_MAX, nodeA});
            }

        }
        else{//node scores
            bufferRecvfromWord(&br, buffer, MAXLINE, delimiter);
            numNode = atoi(buffer);
            for (int j = 0; j < numNode; j++){
                string node;
                int score;
                bufferRecvfromWord(&br, buffer, MAXLINE, delimiter);
                node = buffer;
                bufferRecvfromWord(&br, buffer, MAXLINE, delimiter);
                score = atoi(buffer);
                nodeScore[node] = score;
                cost[node] = {DBL_MAX,{}};
            }
        }
    }
    calcEdgeScore();
}

void processor::calcEdgeScore(){
    for (auto & it : adj){
        double scoreA = nodeScore[it.first];
        for (auto & jt : it.second){
            double scoreB = nodeScore[jt.second];
            double edgeScore = abs(scoreA - scoreB) / (scoreA + scoreB);
            jt.first = edgeScore;
        }

    }
    return;
}

void processor::route(){
    cost[usrA] = {0, {}};
    pq.push({0, usrA});
    while (!pq.empty()){
        pair<double, string> cur = pq.top();
        cost[cur.second].second.push_back(cur.second);
        visited.insert(cur.second);
        if (!cur.second.compare(usrB)){
            break;
        }
        pq.pop();
        double curCost = cost[cur.second].first;
        for (auto & it: adj[cur.second]){
            if ((visited.find(it.second) == visited.end()) && (it.first != DBL_MAX) && ((curCost + it.first) < cost[it.second].first)){
                cost[it.second].first = curCost + it.first;
                cost[it.second].second = cost[cur.second].second;
                
                pq.push({cost[it.second].first, it.second});
            }
        }
    }
    return;
}

void processor::prepareMsg(){
    memset(msg, 0, sizeof(msg));
    int numHops;
    char * ptr = msg;
    msgSize = 0;
    
    route();
    
    if (cost[usrB].first == DBL_MAX){
        numHops = -1;
    }
    else{
        numHops = cost[usrB].second.size() - 2;
    }
    
    sprintf(ptr, "%d", numHops);
    msgSize += (strlen(ptr) + 1);
    ptr += (strlen(ptr) + 1);

    sprintf(ptr, "%s", usrA.c_str());
    msgSize += (strlen(ptr) + 1);
    ptr += (strlen(ptr) + 1);

    sprintf(ptr, "%s", usrB.c_str());
    msgSize += (strlen(ptr) + 1);
    ptr += (strlen(ptr) + 1);

    for (int i = 0; i < numHops; i++){
        sprintf(ptr, "%s", cost[usrB].second[i + 1].c_str());
        msgSize += (strlen(ptr) + 1);
        ptr += (strlen(ptr) + 1);
    }

    if (numHops != -1){
        sprintf(ptr, "%.2f", cost[usrB].first);
        msgSize += (strlen(ptr) + 1);
        ptr += (strlen(ptr) + 1);
    }
    return;
}

void processor::sendMessage(){
    Sendto(fd, msg, msgSize, 0, (struct sockaddr *) &clientaddrRecvfrom, clientaddrlenRecvfrom);
    return;
}

int main(){
    int status;

    /* open socket */
    int serverfd = OpenServerfd(PORT_P_SERVE_C, SOCK_DGRAM);
    fprintf(stdout, "The ServerP is up and running using UDP on port %s.\n", PORT_P_SERVE_C);

    while (1){
        /* build message */
        
        processor myProcessor(serverfd);
        fprintf(stdout, "The ServerP received the topology and score information.\n");
        myProcessor.prepareMsg();

        /* send message back to Central */
        myProcessor.sendMessage();
        fprintf(stdout, "The ServerP finished sending the results to the Central.\n");
    }
}