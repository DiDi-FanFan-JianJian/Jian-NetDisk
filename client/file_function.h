#ifndef FILE_FUNCTION_H
#define FILE_FUNCTION_H

#include <QDebug>
#include <QFile>
#include <QString>
#include <QTextStream>
#include "md5.h"

// �ж�ĳ��·�����ļ������ļ���
bool isDir(const QString &path);
bool isFile(const QString &path);

// �ж�ĳ��·�����ļ����Ƿ����
bool isDirExist(const QString &dir_path);

// �ж�ĳ��·�����ļ��Ƿ����
bool isFileExist(const QString &file_path);

// ��ȡĳ��·���������ļ���
QStringList getDirList(const QString &dir_path);

// ��ȡĳ��·���������ļ�
QStringList getFileList(const QString &dir_path);

// ��ĳ��·���´����ļ���
bool createDir(const QString &dir_path, const QString &dir_name);

// ��ĳ��·���´����ļ�
bool createFile(const QString &file_path);

// ��ȡ�ļ���С
int getFileSize(QString file_name);

// ��ȡ�ļ���
string getFileName(string path);

// ��ĳ���ļ�ָ��λ��д�����ݣ�-1��β��0��ͷ��������ָ��λ�ã�
bool writeFile(const QString &file_path, const QString &content, int pos = -1);

// ��ĳ���ļ���ȡָ��λ�õ�ָ������������
QString readFile(const QString &file_path, int pos, int size, QString &content);

// ���ļ��е�'/'�滻Ϊ'\'
QString slash2backslash(QString file_name);

#endif // FILE_FUNCTION_H
