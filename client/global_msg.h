#ifndef GLOBAL_MSG_H
#define GLOBAL_MSG_H

#include <iostream>
#include <string>
#include <vector>
using namespace std;

struct Global_Msg {
    string username;
    vector<string> path;
    int cur_dir;
};

#endif // GLOBAL_MSG_H
