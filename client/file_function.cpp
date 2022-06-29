#include "file_function.h"

#include <QFileInfo>
#include <QDir>
#include <iostream>

using namespace std;

// �ж�ĳ��·�����ļ������ļ���
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

// �ж�ĳ�ļ����Ƿ����
bool isDirExist(const QString &dir_path)
{
    QDir dir(dir_path);
    return dir.exists();
}

// �ж�ĳ���ļ��Ƿ����
bool isFileExist(const QString &file_path)
{
    QFile file(file_path);
    return file.exists();
}

// ��ȡĳ��·���������ļ��У��������ļ���
QStringList getDirList(const QString &dir_path)
{
    QDir dir(dir_path);
    // ��ȡ�ļ����б��������ļ�
    QStringList dir_list = dir.entryList(QDir::Dirs);
    return dir_list;
}

// ��ȡĳ��·���������ļ����������ļ��У�
QStringList getFileList(const QString &dir_path)
{
    QDir dir(dir_path);
    // ��ȡ�ļ��б��������ļ���
    QStringList file_list = dir.entryList(QDir::Files);
    return file_list;
}

// ��ĳ��·���´����ļ���
bool createDir(const QString &dir_path, const QString &dir_name)
{
    if (!isDirExist(dir_path))
    {
        QDir dir(dir_path);
        // �����ļ���
        return dir.mkdir(dir_name);
    }
    return false;
}

// ��ĳ��·���´����ļ�
bool createFile(const QString &file_path)
{
    if (!isFileExist(file_path))
    {
        QFile file(file_path);
        // �����ļ���ָ����дȨ��
        return file.open(QIODevice::ReadWrite);
    }
    return false;
}

// ��ȡ�ļ���С
int getFileSize(QString file_name)
{
    QFile file(file_name);
    if (!file.open(QIODevice::ReadOnly))
    {
        return -1;
    }
    return file.size();
}

// ��ȡ�ļ���
string getFileName(string path)
{
    QString file_name = QString::fromStdString(path);
    QFileInfo file_info(file_name);
    return file_info.fileName().toStdString();
}

// ��ĳ���ļ�ָ��λ��д�����ݣ�-1��β��0��ͷ��������ָ��λ�ã�
bool writeFile(const QString &file_path, const QString &content, int pos)
{
    // �ж��ļ��Ƿ����
    if (!isFileExist(file_path))
    {
        return false;
    }
    // �Զ����Ʒ�ʽ���ļ����ƶ��ļ�ָ�뵽ָ��λ��
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
    // ���ļ�д�����ݣ��ж��Ƿ�д��ɹ�
    QTextStream out(&file);
    out << content;
    if (file.error() != QFile::NoError)
    {
        return false;
    }
    file.close();
    return true;
}

// ��ĳ���ļ���ȡָ��λ�õ�ָ������������
QString readFile(const QString &file_path, int pos, int size, QString &content)
{
    // �ж��ļ��Ƿ����
    if (!isFileExist(file_path))
    {
        return "";
    }
    // �Զ����Ʒ�ʽ���ļ����ƶ��ļ�ָ�뵽ָ��λ��
    QFile file(file_path);
    if (!file.open(QIODevice::ReadOnly))
    {
        return "";
    }
    file.seek(pos);
    // ��ȡָ�����������ݣ��ж��Ƿ��ȡ�ɹ�
    QTextStream in(&file);
    content = in.read(size);
    if (file.error() != QFile::NoError)
    {
        return "";
    }
    file.close();
    return content;
}

// ���ļ�·���е�'/'�滻Ϊ'\'
QString slash2backslash(QString file_path)
{
    return file_path.replace('/', '\\');
}
