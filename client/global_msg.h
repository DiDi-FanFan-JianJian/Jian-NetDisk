#ifndef GLOBAL_MSG_H
#define GLOBAL_MSG_H

#include <iostream>
#include <string>
#include <vector>

#define PASTE_NOFILE 0
#define PASTE_COPYFILE 1
#define PASTE_CUTFILE 2
#define PASTE_COPYDIR 3
#define PASTE_CUTDIR 4

using namespace std;

struct LoadFileInfo {
    string filename;
    int did;
    int fid;
    int finished_size;
    int file_size;
    string file_path;
    bool working;
};

struct Global_Msg {
    string username;

    int copyfile_dir_id;  // 复制(剪切) 的 文件(文件夹) 的目录id
    string copyfile_name; // 复制(剪切) 的 文件(文件夹) 原名
    int copyfile_id;
    int copyfile_status;  // PASTE_NOFILE   PASTE_COPYFILE    PASTE_CUTFILE   PASTE_COPYDIR   PASTE_CUTDIR


    vector<string> path;
    vector<int> cur_dir; // 记录目录id
    vector<LoadFileInfo> upload_file_list;
    vector<LoadFileInfo> download_file_list;

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
