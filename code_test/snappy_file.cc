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
#include <errno.h>

#include <string>
#include <sstream>
#include <iostream>

#include "snappy.h"

off_t GetFileSize(const char* filename)
{
    struct stat statbuf;

    if ( 0 == stat(filename, &statbuf)) {  
        return statbuf.st_size;
    } else {   
        return 0;
    }   
}

size_t FileGetContents(const std::string& filename, std::string& filedata) { 
    off_t filesize = GetFileSize(filename.c_str());
    if (filesize==(off_t)0) {
        //fprintf(stderr, "FileGetContents filesize=%lu\n", filesize);
        return -1;
    }

    //filedata.resize((size_t)filesize+(size_t)1);
    filedata.resize((size_t)filesize);

    FILE* fp = fopen(filename.c_str(), "r");
    if (fp==NULL) {
        fprintf(stderr, "FileGetContents fopen %s\n", filename.c_str());
        return -1;
    }

    while (!feof(fp)) {
        size_t ret_len = fread((void*)&(filedata[0]), filesize, 1, fp);
        if (ret_len==(size_t)filesize) {
            break;
        }
    }

    //if (read_len!=(size_t)filesize) {
    //    fclose(fp);
    //    fp=NULL;
    //    fprintf(stderr, "FileGetContents read_len=%ld\tfilesize=%lu\n", read_len, filesize);
    //    return false;
    //}
    fclose(fp);
    fp=NULL;
    //fprintf(stdout, "FileGetContents filesize=%lu\n", filesize);
    return filesize;
}

bool FilePutContents(const std::string& filename, const void* data, size_t len) {
    FILE* fp = fopen(filename.c_str(), "w");
    if (fp==NULL) {
        return false;
    }
    fwrite(data, len, 1, fp);
    fflush(fp);
    fclose(fp);
    fp = NULL;
    return true;
}

int time_diff_millisecond(const struct timeval& start, const struct timeval& end) {
    return (int)((end.tv_usec/1000ll+end.tv_sec*1000ll)-(end.tv_usec/1000ll+end.tv_sec*1000ll)); 
}

void SnappyCompress(const std::string& inputdata, std::string& outputdata, std::stringstream& ss) { 
    struct timeval start, end;

    gettimeofday(&start, NULL);
    size_t len = snappy::Compress(inputdata.c_str(), inputdata.size(), &outputdata);
    gettimeofday(&end, NULL); 

    int ctime = time_diff_millisecond(start, end);

    ss << "filesize="     << inputdata.size()             << "\t"
           "compresslen=" << len                          << "\t"
           "percent="     << (len*100.0/inputdata.size()) << "\t" 
           "ctime="       << ctime                        << "\t";
}

void SnappyUncompress(const std::string& inputdata, std::string& outputdata, std::stringstream& ss) { 
    struct timeval start, end;
 
    gettimeofday(&start, NULL);
    if (!snappy::Uncompress(inputdata.c_str(), inputdata.size(), &outputdata)) {
        fprintf(stderr, "SnappyTest Uncompress failed\n");
        return;
    }
    gettimeofday(&end, NULL); 

    int uctime = time_diff_millisecond(start, end);

    ss << "uctime=" << uctime << "\t";
}

int main(int argc, char* argv[]) {
    if (argc<2) { 
		fprintf(stderr, "Usage:%s filename\n", argv[0]);
        exit(1);
    }

    std::stringstream ss;

    ss << "snappy||";

    std::string filename(argv[1]); 
    std::string compressfilename = filename + std::string(".compress.snappy");

    std::string filedata;
    if (!FileGetContents(filename, filedata)) {
        fprintf(stderr, "FileGetContents %s failed\n", argv[1]);
        exit(1);
    }
	if (filedata.size()==0) {
        //fprintf(stderr, "FileGetContents %s fileempty\n", argv[1]);
		exit(0);
	}

    std::string compressdata;
    SnappyCompress(filedata, compressdata, ss);	   // Compress
    
    FilePutContents(compressfilename, compressdata.data(), compressdata.size());    // Recode

    std::string uncompressdata; 
    SnappyUncompress(compressdata, uncompressdata, ss);   // UnCompress 

    if (filedata==uncompressdata) {
		ss << "cu=" << "1" << "\t";
    } else { 
        ss << "cu=" << "0" << "\t";
    }
    //std::cout << ss.str() << std::endl;
    std::cout << ss.str();
    return 0;
}
