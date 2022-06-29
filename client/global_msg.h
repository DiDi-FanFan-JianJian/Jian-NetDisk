#ifndef GLOBAL_MSG_H
#define GLOBAL_MSG_H

#include <iostream>
#include <string>
#include <vector>
using namespace std;

struct Global_Msg {
    string username;
    vector<string> path;
    vector<int> cur_dir; // ¼ÇÂ¼Ä¿Â¼id

    string get_path() {
        string p = "/";
        for (auto it: path) {
            p += it + "/";
        }
        return p;
    }

    int get_cur_id() {
        size_t sz = cur_dir.size();
        return cur_dir[sz - 1];
    }

    void go_back() {
        if (path.size() > 1) {
            path.pop_back();
            cur_dir.pop_back();
        }
    }
};

#endif // GLOBAL_MSG_H
