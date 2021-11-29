#include "../util.h"
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <queue>
#include <float.h>

using namespace std;
class processor{
    private:
        string usrA;
        string usrB;
        char msg[MAXLINE];
        int msgSize;
        unordered_map<string, vector<pair<double, string>>> adj;
        unordered_map<string, int> nodeScore;
        unordered_map<string, pair<double,vector<string>>> cost;
        unordered_set<string> visited;
        priority_queue<pair<double, string>, vector<pair<double, string>>, greater<pair<double, string>>> pq;
        void route();
        void calcEdgeScore();
        struct sockaddr_storage clientaddrRecvfrom;
        socklen_t clientaddrlenRecvfrom;
        int fd;
    public:
        processor(int fd);
        ~processor(){};
        void prepareMsg();
        void sendMessage();

};