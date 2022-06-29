#ifndef FILE_FUNCTION_H
#define FILE_FUNCTION_H

#include <QDebug>
#include <QFile>
#include <QString>
#include <QTextStream>
#include "md5.h"

// 判断某个路径是文件还是文件夹
bool isDir(const QString &path);
bool isFile(const QString &path);

// 判断某个路径下文件夹是否存在
bool isDirExist(const QString &dir_path);

// 判断某个路径下文件是否存在
bool isFileExist(const QString &file_path);

// 获取某个路径下所有文件夹
QStringList getDirList(const QString &dir_path);

// 获取某个路径下所有文件
QStringList getFileList(const QString &dir_path);

// 在某个路径下创建文件夹
bool createDir(const QString &dir_path, const QString &dir_name);

// 在某个路径下创建文件
bool createFile(const QString &file_path);

// 获取文件大小
int getFileSize(QString file_name);

// 获取文件名
string getFileName(string path);

// 向某个文件指定位置写入内容（-1：尾，0：头，其他：指定位置）
bool writeFile(const QString &file_path, const QString &content, int pos = -1);

// 从某个文件读取指定位置的指定数量的内容
QString readFile(const QString &file_path, int pos, int size, QString &content);

// 将文件中的'/'替换为'\'
QString slash2backslash(QString file_name);

#endif // FILE_FUNCTION_H
