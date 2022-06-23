#pragma once

#include <fstream>
#include <openssl/md5.h>
#include <string>
#include <cstring>
using std::string;

int get_file_md5(const std::string &file_name, std::string &md5_value);