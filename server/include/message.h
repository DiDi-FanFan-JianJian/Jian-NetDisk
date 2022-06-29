#pragma once
#include <cstring>
#include <string>
using namespace std;

constexpr int BLOCK_SIZE = 1024 * 16;
constexpr int NAME_SIZE = 20;
constexpr int FILE_NUM = 50; // �ļ������ļ��������

// ����msg[0]�ж���Ϣ���ͣ���Message(msg + 1)���н���
constexpr int MSG_TYPE_LOGIN = 1;
constexpr int MSG_TYPE_REGISTER = 2;
constexpr int MSG_UPLOAD_FILE = 3;
constexpr int MSG_UPLOAD_BLOCK = 4;
constexpr int MSG_CREATE_DIR = 5;
constexpr int MSG_CREATE_FILE_DIR = 6;
constexpr int MSG_GET_DIR_ID = 7;
constexpr int MSG_GET_FILE_ID = 8;
constexpr int MSG_GET_DIRS = 9;
constexpr int MSG_GET_FILES = 10;
constexpr int MSG_MOVE_DIR = 11;
constexpr int MSG_MOVE_FILE = 12;
constexpr int MSG_DELETE_DIR = 13;
constexpr int MSG_DELETE_FILE = 14;
constexpr int MSG_GET_FILE_INFO = 15;
constexpr int MSG_DOWNLOAD_BLOCK = 16;
constexpr int MSG_COPY_FILE = 17;
constexpr int MSG_COPY_DIR = 18;
constexpr int MSG_RENAME_FILE = 19;
constexpr int MSG_RENAME_DIR = 20;

// ע�� ��¼
struct LoginMessage
{
  LoginMessage() {}
  LoginMessage(const char* msg) {
    memcpy(this, msg, sizeof(LoginMessage));
  }
  char username[NAME_SIZE];
  char password[NAME_SIZE];
};

struct LoginResponse
{
  LoginResponse() {}
  LoginResponse(const char* msg) {
    memcpy(this, msg, sizeof(LoginResponse));
  }
  enum flags{
    success = 0,
    failed = 1,
    user_exist = 2,
    passwd_error = 3,
    dir_exist = 4,
    file_exist = 5,
  };
  int status;
  int dir; // ��Ŀ¼���
};

// �ļ�
struct UploadFileMessage
{
  UploadFileMessage() {}
  UploadFileMessage(const char* msg) {
    memcpy(this, msg, sizeof(UploadFileMessage));
  }
  char md5[33];  
  int file_size; // �ֽ���
  int block_num; // ǰ�˼����ܿ���
  int block_id;  // ��0��ʼ���
};

// ������Ҫ���ļ���
struct UploadFileResponse
{
  UploadFileResponse() {}
  UploadFileResponse(const char* msg) {
    memcpy(this, msg, sizeof(UploadFileResponse));
  }
  bool checked; // �Ƿ���֤ͨ�� true������һ������
  int block_id;
};

// �ϴ��ļ���
struct UploadBlockMessage
{
  UploadBlockMessage() {}
  UploadBlockMessage(const char* msg) {
    memcpy(this, msg, sizeof(UploadBlockMessage));
  }
  char md5[33];
  int block_id;
  int size; // �ֽ���
  char block_data[BLOCK_SIZE];
};

// �����ϴ����
struct UploadBlockResponse
{
  UploadBlockResponse() {}
  UploadBlockResponse(const char* msg) {
    memcpy(this, msg, sizeof(UploadBlockResponse));
  }
  int next_block_id; // ��һ�����id -1��ʾ�ϴ����
};

// ����Ŀ¼
struct CreateDirMessage
{
  CreateDirMessage() {}
  CreateDirMessage(const char* msg) {
    memcpy(this, msg, sizeof(CreateDirMessage));
  }
  char username[NAME_SIZE];
  char dirname[NAME_SIZE];
  int pid; // ��Ŀ¼���
};

typedef LoginResponse CreateDirResponse;

// ��ȡĿ¼id
struct GetDirIDMessage
{
  GetDirIDMessage() {}
  GetDirIDMessage(const char* msg) {
    memcpy(this, msg, sizeof(GetDirIDMessage));
  }
  char username[NAME_SIZE];
  char dirname[NAME_SIZE];
  int pid; // ��Ŀ¼���
};

typedef LoginResponse GetDirIDResponse;

// ��ȡ�ļ�id
struct GetFileIDMessage
{
  GetFileIDMessage() {}
  GetFileIDMessage(const char* msg) {
    memcpy(this, msg, sizeof(GetFileIDMessage));
  }
  char username[NAME_SIZE];
  char filename[NAME_SIZE];
  int pid; // ��Ŀ¼���
};

typedef LoginResponse GetFileIDResponse;

// �����ļ�Ŀ¼
struct CreateFileDirMessage
{
  CreateFileDirMessage() {}
  CreateFileDirMessage(const char* msg) {
    memcpy(this, msg, sizeof(CreateFileDirMessage));
  }
  char username[NAME_SIZE];
  int pid; // ��Ŀ¼���
  char md5[33]; // �ļ���md5
  char filename[NAME_SIZE];
};

typedef LoginResponse CreateFileDirResponse;

// �ļ�����
struct CopyFileMessage
{
  CopyFileMessage() {}
  CopyFileMessage(const char* msg) {
    memcpy(this, msg, sizeof(CopyFileMessage));
  }
  char username[NAME_SIZE];
  int pid; // ��Ŀ¼���
  int fid; // Դ�ļ����
  char filename[NAME_SIZE];
};

typedef LoginResponse CopyFileResponse;


// ��ȡ�ļ�����Ŀ¼
struct GetDirsMessage
{
  GetDirsMessage() {}
  GetDirsMessage(const char* msg) {
    memcpy(this, msg, sizeof(GetDirsMessage));
  }
  char username[NAME_SIZE];
  int pid; // ��Ŀ¼���
};

struct GetDirsResponse
{
  GetDirsResponse() {}
  GetDirsResponse(const char* msg) {
    memcpy(this, msg, sizeof(GetDirsResponse));
  }
  int num; // Ŀ¼����
  char dirname[FILE_NUM][NAME_SIZE]; // Ŀ¼��
};

// ��ȡ�ļ������ļ�
typedef GetDirsMessage GetFilesMessage;
typedef GetDirsResponse GetFilesResponse;

// �����ļ���λ��
struct MoveDirMessage
{
  MoveDirMessage() {}
  MoveDirMessage(const char* msg) {
    memcpy(this, msg, sizeof(MoveDirMessage));
  }
  int id; // Ŀ¼���
  int src; // Դ��Ŀ¼���
  int dst; // Ŀ�길Ŀ¼���
};

typedef LoginResponse MoveDirResponse;

typedef MoveDirMessage MoveFileMessage;
typedef MoveDirResponse MoveFileResponse;

// ɾ��Ŀ¼
struct DeleteDirMessage
{
  DeleteDirMessage() {}
  DeleteDirMessage(const char* msg) {
    memcpy(this, msg, sizeof(DeleteDirMessage));
  }
  int id; // Ŀ¼���
  int did; // ɾ���ļ�ʱ��Ŀ¼���
};

typedef LoginResponse DeleteDirResponse;

typedef DeleteDirMessage DeleteFileMessage;
typedef DeleteDirResponse DeleteFileResponse;


// ��ȡ�ļ���Ϣ
struct GetFileInfoMessage
{
  GetFileInfoMessage() {}
  GetFileInfoMessage(const char* msg) {
    memcpy(this, msg, sizeof(GetFileInfoMessage));
  }
  int id; // �ļ����
};

struct GetFileInfoResponse
{
  GetFileInfoResponse() {}
  GetFileInfoResponse(const char* msg) {
    memcpy(this, msg, sizeof(GetFileInfoResponse));
  }
  int file_size; // �ֽ���
};


// �����ļ���
struct DownloadBlockMessage
{
  DownloadBlockMessage() {}
  DownloadBlockMessage(const char* msg) {
    memcpy(this, msg, sizeof(DownloadBlockMessage));
  }
  int id;
  int block_id; // ������ ��1��ʼ���
  int size; // �ֽ���
};

struct DownloadBlockResponse
{
  DownloadBlockResponse() {}
  DownloadBlockResponse(const char* msg) {
    memcpy(this, msg, sizeof(DownloadBlockResponse));
  }
  char block_data[BLOCK_SIZE];
};

struct CopyDirMessage
{
  CopyDirMessage() {}
  CopyDirMessage(const char* msg) {
    memcpy(this, msg, sizeof(CopyDirMessage));
  }
  char username[NAME_SIZE];
  int src; // ԴĿ¼���
  int dst; // Ŀ�길Ŀ¼���
};

typedef LoginResponse CopyDirResponse;

// ����
struct RenameDirMessage
{
  RenameDirMessage() {}
  RenameDirMessage(const char* msg) {
    memcpy(this, msg, sizeof(RenameDirMessage));
  }
  int id; // Ŀ¼���
  int did; // �޸��ļ�ʱ��Ŀ¼���
  char newname[NAME_SIZE]; // ������
};

typedef LoginResponse RenameDirResponse;

typedef RenameDirMessage RenameFileMessage;
typedef RenameDirResponse RenameFileResponse;
