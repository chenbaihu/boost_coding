//dep: git@github.com:osrf/bzip2_cmake.git

#include "bzlib.h"

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

int main(int argc, char** argv)
{
	if (argc<2) { 
		fprintf(stderr, "Usage:%s filename\n", argv[0]);
        exit(1);
    }

	std::stringstream ss;

    ss << "bzip2||";

    std::string filename(argv[1]); 
    std::string compressfilename = filename + std::string(".compress.bzip2");

    std::string filedata;
    if (!FileGetContents(filename, filedata)) {
        fprintf(stderr, "FileGetContents %s failed\n", argv[1]);
        exit(1);
    }
	if (filedata.size()==0) {
        //fprintf(stderr, "FileGetContents %s fileempty\n", argv[1]);
		exit(0);
	}

	struct timeval start, end;    

	std::string compressdata;   
	compressdata.resize(filedata.size());
	unsigned int compressdatalen=filedata.size();
	
	gettimeofday(&start, NULL);
	if(BZ2_bzBuffToBuffCompress(
			&(compressdata[0]), 			//压缩后的数据
			&compressdatalen, 
            &(filedata[0]), 				//文件原始数据
			(unsigned int)filedata.size(), 
			9,    // blockSize100k arameter blockSize100k specifies the block size to be used for compression. It should be a value between 1 and 9 inclusive, and the actual block size used is 100000 x this figure. 9 gives the best compression but takes most memory.
			0,    // Parameter verbosity should be set to a number between 0 and 4 inclusive. 0 is silent, and greater numbers give increasingly verbose monitoring/debugging output. If the library has been compiled with -DBZ_NO_STDIO, no such output will appear for any verbosity setting.
            0)!=BZ_OK)    // 0==30是默认值
	{    
        fprintf(stderr, "BZ2_bzBuffToBuffCompress failed\n");
		return 1;
	}
	gettimeofday(&end, NULL); 
    
    int ctime = time_diff_millisecond(start, end);
    ss  << "filesize="    << filedata.size() << "\t" 
        << "compresslen=" << compressdatalen << "\t"  
        << "percent="     << compressdatalen*100.0/filedata.size() << "\t" 
        << "ctime="       << ctime           << "\t";

    FilePutContents(compressfilename, (const char*)compressdata.data(), (size_t)compressdatalen);    // Recode

	std::string uncompressdata;   
	uncompressdata.resize(filedata.size());
	unsigned int uncompressdatalen=filedata.size();
	gettimeofday(&start, NULL);
	if (BZ2_bzBuffToBuffDecompress(
			&uncompressdata[0],        //解压缩后的数据
			&uncompressdatalen, 
			&(compressdata[0]),        //压缩后的数据
			compressdatalen,
			0,         
			0)!=BZ_OK) {
        fprintf(stderr, "BZ2_bzBuffToBuffDecompress failed\n");
		return 1;
	}
    gettimeofday(&end, NULL); 

    int uctime = time_diff_millisecond(start, end); 
    ss << "uctime=" << uctime << "\t"; 	

	if (memcmp(uncompressdata.data(), filedata.data(), filedata.size())==0) {
		ss << "cu=1" << "\t";
    } else { 
        ss << "cu=0" << "\t";
    }
	//std::cout << ss.str() << std::endl;
	std::cout << ss.str();
}

