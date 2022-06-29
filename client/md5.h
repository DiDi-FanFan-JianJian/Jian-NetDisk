#ifndef MD5_H
#define MD5_H

#include <QString>
#include <QByteArray>
#include <QCryptographicHash>
#include <QFile>
#include <QDebug>
#include <string>

using namespace std;

string getFileMd5(string path);

#endif // MD5_H
