#ifndef __SERVERT_H__
#define __SERVERT_H__
#include <unordered_map>
#include <string>
#include <vector>
#include <fstream>
#include <sstream>
#include "../util.h"
using namespace std;

const char USR_FILENAME[] = {'e', 'd', 'g', 'e', 'l', 'i', 's', 't', '.', 't', 'x', 't', '\0'};
class graph{
    private:
        unordered_map<string, string> parent;
        unordered_map<string, int> rank;
        unordered_map<string, vector<string>> edge;
        void add(string &usr);
    public:
        graph(const char * filename);
        ~graph(){};
        bool ifexist(string &usr);
        string find(string &usr);
        void myUnion(string &usrA, string &usrB);
        void findCommonParent(string &usr, vector<string> &res);
        void findEdge(vector<string> & usrs, vector<string> &relatedEdge);
};

int buildMessageFromVector(char * msg, vector<string> & usr, bool nodeMsg);
#endif