#ifndef __SERVERS_H__
#define __SERVERS_H__
#include <unordered_map>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "../util.h"
using namespace std;

const char SCORES_FILENAME[] = {'s', 'c', 'o', 'r', 'e', 's', '.', 't', 'x', 't', '\0'};

class scores{
    private:
        unordered_map<string, int> scoresL;
    public:
        scores(const char *filename);
        ~scores(){};
        void queryScore(string & usr, vector<string> &usrs, vector<int> & vals);
};

int buildMessageFromVector(char * msg, vector<string> & usrs, vector<int> & scoresL);
#endif