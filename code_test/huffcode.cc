#include "huffman.h"

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

    ss << "huffman||";

    std::string filename(argv[1]); 
    std::string compressfilename = filename + std::string(".compress.huffman");

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

	unsigned char * cbufout = NULL;   
    unsigned int    cbufoutlen = 0;
	
	gettimeofday(&start, NULL);
	if(huffman_encode_memory((const unsigned char*)filedata.data(), (unsigned int)filedata.size(), &cbufout, &cbufoutlen)) {    
        fprintf(stderr, "huffman_encode_memory failed\n");
		return 1;
	}
	gettimeofday(&end, NULL); 
    
    int ctime = time_diff_millisecond(start, end);
    ss << "filesize="    << filedata.size() << "\t" 
        << "compresslen=" << cbufoutlen      << "\t"  
        << "percent="     << cbufoutlen*100.0/filedata.size() << "\t" 
        << "ctime="       << ctime           << "\t";

    FilePutContents(compressfilename, (const char*)cbufout, (size_t)cbufoutlen);    // Recode

	unsigned char * ucbufout = NULL;   
    unsigned int    ucbufoutlen = 0;

	gettimeofday(&start, NULL);
	if(huffman_decode_memory(cbufout, cbufoutlen, &ucbufout, &ucbufoutlen)) {
        fprintf(stderr, "huffman_decode_memory failed\n");
		return 2;
	}
    gettimeofday(&end, NULL); 

    int uctime = time_diff_millisecond(start, end); 
    ss << "uctime=" << uctime << "\t"; 	

	std::string uncompressdata((const char*)ucbufout, (size_t)ucbufoutlen);

	if (uncompressdata==filedata) {
		ss << "cu=1" << "\t";
    } else { 
        ss << "cu=0" << "\t";
    }
	//std::cout << ss.str() << std::endl;
	std::cout << ss.str();
}

//static int
//memory_encode_file(FILE *in, FILE *out)
//{
//	unsigned char *buf = NULL, *bufout = NULL;
//	unsigned int len = 0, cur = 0, inc = 1024, bufoutlen = 0;
//
//	assert(in && out);
//
//	/* Read the file into memory. */
//	while(!feof(in))
//	{
//		unsigned char *tmp;
//		len += inc;
//		tmp = (unsigned char*)realloc(buf, len);
//		if(!tmp)
//		{
//			if(buf)
//				free(buf);
//			return 1;
//		}
//
//		buf = tmp;
//		cur += fread(buf + cur, 1, inc, in);
//	}
//
//	if(!buf)
//		return 1;
//
//	struct timeval start, end;    
//	gettimeofday(&start, NULL);
//
//	/* Encode the memory. */
//	if(huffman_encode_memory(buf, cur, &bufout, &bufoutlen))
//	{
//		free(buf);
//		return 1;
//	}
//
//	gettimeofday(&end, NULL); 
//    int ctime = time_diff_millisecond(start, end);
//	fprintf(stdout, "huffolen=%d\thuffelen=%d\thuffpercent=%f\thuffctime=%d\n", cur, bufoutlen, bufoutlen*100.0/cur, ctime);
//
//	free(buf);
//
//	/* Write the memory to the file. */
//	if(fwrite(bufout, 1, bufoutlen, out) != bufoutlen)
//	{
//		free(bufout);
//		return 1;
//	}
//
//	free(bufout);
//
//	return 0;
//}
//
//static int
//memory_decode_file(FILE *in, FILE *out)
//{
//	unsigned char *buf = NULL, *bufout = NULL;
//	unsigned int len = 0, cur = 0, inc = 1024, bufoutlen = 0;
//	assert(in && out);
//
//	/* Read the file into memory. */
//	while(!feof(in))
//	{
//		unsigned char *tmp;
//		len += inc;
//		tmp = (unsigned char*)realloc(buf, len);
//		if(!tmp)
//		{
//			if(buf)
//				free(buf);
//			return 1;
//		}
//
//		buf = tmp;
//		cur += fread(buf + cur, 1, inc, in);
//	}
//
//	if(!buf)
//		return 1;
//
//    struct timeval start, end;    
//	gettimeofday(&start, NULL);
//
//	/* Decode the memory. */
//	if(huffman_decode_memory(buf, cur, &bufout, &bufoutlen))
//	{
//		free(buf);
//		return 1;
//	}
//    gettimeofday(&end, NULL); 
//    int uctime = time_diff_millisecond(start, end); 
//
//	fprintf(stdout, "decode\tdlen=%d\tolen=%d\tuctime=%d\n", cur, bufoutlen, uctime);
//
//	free(buf);
//
//	/* Write the memory to the file. */
//	if(fwrite(bufout, 1, bufoutlen, out) != bufoutlen)
//	{
//		free(bufout);
//		return 1;
//	}
//
//	free(bufout);
//
//	return 0;
//}
