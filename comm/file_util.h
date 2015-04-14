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

#include <errno.h>
#include <unistd.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <fcntl.h> 
#include <dirent.h>     
#include <stdlib.h>
#include <stdint.h>

#include <fstream>
#include <vector>
#include <string>
#include <map>
#include <iostream>

class FileUtil {
public:
    static bool GetFileLines(std::string& filename, std::vector<std::string>& file_lines);   
    static bool GetFileLines(const char* filename, std::vector<std::string>& file_lines);   

    static bool CopyFile(const std::string& src, const std::string& dst, std::string& errinfo); 
    static bool CopyFile(const char * src, const char * dst, char * errbuf);

    static bool FilePutContent(const std::string& filename, void * buf, unsigned int len, std::string& errbuf);
    static bool FilePutContent(const char * filename, void * buf, unsigned int len, char * errbuf);

    /* Read "n" bytes from a descriptor. */
    static ssize_t Readn(int fd, void *vptr, size_t n); 

    /* write "n" bytes to a descriptor. */
    static ssize_t Writen(int fd, const void *vptr, size_t n, int sleeptime=1000/*us*/);

    /* get file info */
    static time_t GetFileTime(const char * filename);

    static ino_t GetFileInode(const char * filename); 

    static ino_t GetFileInode(int fd);

    static off_t GetFileSize(const char * filename); 

    static off_t GetFilePos(int fd);
};

#endif


