/*
 *参考博客;
 *    http://technet.microsoft.com/zh-cn/library/44cs32f9(v=vs.80).aspx
 *
 * c++ file stream open type to c open file type
 *
 *r：
 *    ios_base::in becomes "r" (open existing file for reading).
 *w:
 *    ios_base::out or ios_base::out | ios_base::trunc becomes "w" (truncate existing file or create for writing).
 *a：
 *    ios_base::out | app becomes "a" (open existing file for appending all writes).
 *
 *r+:
 *    ios_base::in | ios_base::out becomes "r+" (open existing file for reading and writing).
 *
 *w+:
 *    ios_base::in | ios_base::out | ios_base::trunc becomes "w+" (truncate existing file or create for reading and writing).
 *
 *a+：
 *    ios_base::in | ios_base::out | ios_base::app becomes "a+" (open existing file for reading and for appending all writes).
 * */

#ifndef __FILE_UTIL_H
#define __FILE_UTIL_H

#include <fstream>
#include <vector>
#include <string>
#include <iostream>

class FileUtil {
public:
    static bool GetFileLines(std::string& filename, std::vector<std::string>& file_lines);   
    static bool GetFileLines(const char* filename, std::vector<std::string>& file_lines);   
};

#endif

bool FileUtil::GetFileLines(std::string& filename, std::vector<std::string>& file_lines)
{
    return FileUtil::GetFileLines(filename.c_str(), file_lines);
}

bool FileUtil::GetFileLines(const char* filename, std::vector<std::string>& file_lines)
{ 
    if (filename==NULL) {
        return false;
    } 
    std::ifstream ifs;

    ifs.open(filename, std::fstream::in);
    if (!ifs.good()) {
        return false;
    }

    while (!ifs.eof()) {
        std::string line;
        getline(ifs, line);
        if (ifs.fail() && !ifs.eof()) {
            ifs.close();
            return false;
        }    
        file_lines.push_back(line);
    }

    ifs.close();
    return true;
}

#ifdef _TEST_

int main(int argc, char*argv[]) {
    std::vector<std::string> file_lines;

    if (!FileUtil::GetFileLines("./file_util.cc", file_lines)) {
        std::cout << "GetFileLines failed" << std::endl;
        return 1;
    }

    std::vector<std::string>::const_iterator it = file_lines.begin();
    while (it != file_lines.end()) {
        std::cout << *it << std::endl;
        it++;
    }

    return 0;
}

#endif
