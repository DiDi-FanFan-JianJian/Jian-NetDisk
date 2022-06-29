#include "file_function.h"

#include <QFileInfo>
#include <QDir>
#include <iostream>

using namespace std;

// 判断某个路径是文件还是文件夹
bool isDir(const QString &path)
{
    QFileInfo file_info(path);
    return file_info.isDir();
}
bool isFile(const QString &path)
{
    QFileInfo file_info(path);
    return file_info.isFile();
}

// 判断某文件夹是否存在
bool isDirExist(const QString &dir_path)
{
    QDir dir(dir_path);
    return dir.exists();
}

// 判断某个文件是否存在
bool isFileExist(const QString &file_path)
{
    QFile file(file_path);
    return file.exists();
}

// 获取某个路径下所有文件夹（不包括文件）
QStringList getDirList(const QString &dir_path)
{
    QDir dir(dir_path);
    // 获取文件夹列表，不包括文件
    QStringList dir_list = dir.entryList(QDir::Dirs);
    return dir_list;
}

// 获取某个路径下所有文件（不包括文件夹）
QStringList getFileList(const QString &dir_path)
{
    QDir dir(dir_path);
    // 获取文件列表，不包括文件夹
    QStringList file_list = dir.entryList(QDir::Files);
    return file_list;
}

// 在某个路径下创建文件夹
bool createDir(const QString &dir_path, const QString &dir_name)
{
    if (!isDirExist(dir_path))
    {
        QDir dir(dir_path);
        // 创建文件夹
        return dir.mkdir(dir_name);
    }
    return false;
}

// 在某个路径下创建文件
bool createFile(const QString &file_path)
{
    if (!isFileExist(file_path))
    {
        QFile file(file_path);
        // 创建文件并指定读写权限
        return file.open(QIODevice::ReadWrite);
    }
    return false;
}

// 获取文件大小
int getFileSize(QString file_name)
{
    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly))
    {
        return -1;
    }
    return file.size();
}

// 获取文件名
string getFileName(string path)
{
    QString file_name = QString::fromStdString(path);
    QFileInfo file_info(file_name);
    return file_info.fileName().toStdString();
}

// 向某个文件指定位置写入内容（-1：尾，0：头，其他：指定位置）
bool writeFile(const QString &file_path, const QString &content, int pos)
{
    // 判断文件是否存在
    if (!isFileExist(file_path))
    {
        return false;
    }
    // 以二进制方式打开文件，移动文件指针到指定位置
    QFile file(file_path);
    if (!file.open(QIODevice::ReadWrite))
    {
        return false;
    }
    if (pos == -1)
    {
        file.seek(file.size());
    }
    else if (pos == 0)
    {
        file.seek(0);
    }
    else
    {
        file.seek(pos);
    }
    // 向文件写入内容，判断是否写入成功
    QTextStream out(&file);
    out << content;
    if (file.error() != QFile::NoError)
    {
        return false;
    }
    file.close();
    return true;
}

// 从某个文件读取指定位置的指定数量的内容
QString readFile(const QString &file_path, int pos, int size, QString &content)
{
    // 判断文件是否存在
    if (!isFileExist(file_path))
    {
        return "";
    }
    // 以二进制方式打开文件，移动文件指针到指定位置
    QFile file(file_path);
    if (!file.open(QIODevice::ReadOnly))
    {
        return "";
    }
    file.seek(pos);
    // 读取指定数量的内容，判断是否读取成功
    QTextStream in(&file);
    content = in.read(size);
    if (file.error() != QFile::NoError)
    {
        return "";
    }
    file.close();
    return content;
}

// 将文件路径中的'/'替换为'\'
QString slash2backslash(QString file_path)
{
    return file_path.replace('/', '\\');
}
