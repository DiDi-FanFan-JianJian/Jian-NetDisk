#ifndef GLOBAL_MSG_H
#define GLOBAL_MSG_H

#include <iostream>
#include <string>
#include <vector>
#include <fstream>
#include <io.h>
#include <direct.h>
#include <map>
#include <queue>
#include <windows.h>

#define STOP 0
#define RUNNING 1
#define WAITING 2

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
    char md5[33] = "00000000000000000000000000000000";
    int working;
};

struct Global_Msg {
    string username;
    vector<string> path;
    vector<int> cur_dir; // ��¼Ŀ¼id
    int copyfile_dir_id;  // ����(����) �� �ļ�(�ļ���) ��Ŀ¼id
    string copyfile_name; // ����(����) �� �ļ�(�ļ���) ԭ��
    int copyfile_id;
    int copyfile_status;  // PASTE_NOFILE   PASTE_COPYFILE    PASTE_CUTFILE   PASTE_COPYDIR   PASTE_CUTDIR

    vector<LoadFileInfo> upload_file_list;
    vector<LoadFileInfo> download_file_list;

    map<string, string> file_md5;
    map<string, int> file_size; // �ļ�md5��Ӧ���ֽ���
    map<string, int> block_num; // �ļ�md��Ӧ���ܿ���
    map<string, int> cur_idx;   // ��ǰ�Ѵ���Ŀ���

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

    // ���һ��LoadFileInfo
    void add_upload_file() {
    }
    void add_download_file() {
    }
    // ɾ��һ��LoadFileInfo
    void del_upload_file() {
    }
    void del_download_file() {
    }

    // ����ʹ��
    void test_info() {
        if (upload_file_list.size() != 0 || download_file_list.size() != 0) {
            return;
        }
//        // ����ĸ������ļ�
//        for (int i = 0; i < 4; i++) {
//            LoadFileInfo info;
//            info.filename = "test" + to_string(i);
//            info.did = i;
//            info.fid = i;
//            info.finished_size = 0;
//            info.file_size = i * i + 1;
//            info.file_path = "test" + to_string(i);
//            info.working = false;
//            upload_file_list.push_back(info);
//        }
//        // ����ĸ������ļ�
//        for (int i = 0; i < 4; i++) {
//            LoadFileInfo info;
//            info.filename = "test" + to_string(i);
//            info.did = i;
//            info.fid = i;
//            info.finished_size = 0;
//            info.file_size = i * i + 1;
//            info.file_path = "test" + to_string(i);
//            info.working = false;
//            download_file_list.push_back(info);
//        }
    }

    // �����е�LoadFileInfo�Ĺ���״̬����Ϊfalse
    void set_all_file_working_false() {
        for (auto &&it: upload_file_list) {
            it.working = 0;
        }
        for (auto &&it: download_file_list) {
            it.working = 0;
        }
    }
    // ��upload_file_list��download_file_listд���ļ����´��ٴε�¼ʱʹ��
    void write_file_list() {
        // д֮ǰ�Ƚ����еĹ���״̬����Ϊfalse
        set_all_file_working_false();

        // c++�ж��ļ���C:\\netdisk\\username�Ƿ���ڣ���������ݹ鴴��
        string prefix = "C:\\netdisk";
        if (_access(prefix.c_str(), 0) == -1) {
            _mkdir(prefix.c_str());
        }
        prefix += "\\" + username;
        if (_access(prefix.c_str(), 0) == -1) {
            _mkdir(prefix.c_str());
        }

        // д��C:\netdisk\username\upload_file_list.txt
        ofstream ofs("C:\\netdisk\\" + username + "\\upload_file_list.txt");
        for (auto &&it: upload_file_list) {
            ofs << it.filename << " " << it.did << " " << it.fid << " " << it.finished_size << " " << it.file_size << " " << it.file_path << " " << it.working << endl;
        }
        ofs.close();
        // д��C:\netdisk\username\download_file_list.txt
        ofs.open("C:\\netdisk\\" + username + "\\download_file_list.txt");
        for (auto it: download_file_list) {
            ofs << it.filename << " " << it.did << " " << it.fid << " " << it.finished_size << " " << it.file_size << " " << it.file_path << " " << it.working << endl;
        }
        ofs.close();
    }
    // ���ļ��ж�ȡupload_file_list��download_file_list
    void read_file_list() {
        // ��ȡC:\netdisk\username\upload_file_list.txt
        ifstream ifs("C:\\netdisk\\" + username + "\\upload_file_list.txt");
        // ����ļ������ڣ������
        if (ifs.is_open()) {
            string filename;
            int did;
            int fid;
            int finished_size;
            int file_size;
            string file_path;
            bool working;
            while (ifs >> filename >> did >> fid >> finished_size >> file_size >> file_path >> working) {
                LoadFileInfo info;
                info.filename = filename;
                info.did = did;
                info.fid = fid;
                info.finished_size = finished_size;
                info.file_size = file_size;
                info.file_path = file_path;
                info.working = working;
                upload_file_list.push_back(info);
            }
            ifs.close();
        }
        // ��ȡC:\netdisk\username\download_file_list.txt
        ifs.open("C:\\netdisk\\" + username + "\\download_file_list.txt");
        // ����ļ������ڣ������
        if (ifs.is_open()) {
            string filename;
            int did;
            int fid;
            int finished_size;
            int file_size;
            string file_path;
            bool working;
            while (ifs >> filename >> did >> fid >> finished_size >> file_size >> file_path >> working) {
                LoadFileInfo info;
                info.filename = filename;
                info.did = did;
                info.fid = fid;
                info.finished_size = finished_size;
                info.file_size = file_size;
                info.file_path = file_path;
                info.working = working;
                download_file_list.push_back(info);
            }
            ifs.close();
        }
    }


    string GbkToUtf8(const char *src_str)
    {
        int len = MultiByteToWideChar(CP_ACP, 0, src_str, -1, NULL, 0);
        wchar_t* wstr = new wchar_t[len + 1];
        memset(wstr, 0, len + 1);
        MultiByteToWideChar(CP_ACP, 0, src_str, -1, wstr, len);
        len = WideCharToMultiByte(CP_UTF8, 0, wstr, -1, NULL, 0, NULL, NULL);
        char* str = new char[len + 1];
        memset(str, 0, len + 1);
        WideCharToMultiByte(CP_UTF8, 0, wstr, -1, str, len, NULL, NULL);
        string strTemp = str;
        if (wstr) delete[] wstr;
        if (str) delete[] str;
        return strTemp;
    }

    string Utf8ToGbk(const char *src_str)
    {
        int len = MultiByteToWideChar(CP_UTF8, 0, src_str, -1, NULL, 0);
        wchar_t* wszGBK = new wchar_t[len + 1];
        memset(wszGBK, 0, len * 2 + 2);
        MultiByteToWideChar(CP_UTF8, 0, src_str, -1, wszGBK, len);
        len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
        char* szGBK = new char[len + 1];
        memset(szGBK, 0, len + 1);
        WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
        string strTemp(szGBK);
        if (wszGBK) delete[] wszGBK;
        if (szGBK) delete[] szGBK;
        return strTemp;
    }
};

#endif // GLOBAL_MSG_H
