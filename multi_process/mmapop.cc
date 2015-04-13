#include "mmapop.h"

#include <unistd.h>
#include <sys/fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/stat.h>
#include <iostream>
#include <string>

namespace util { 

bool MMapOP::Init(const char* filename, size_t size, bool& create, const std::string & ip, const std::string & version) {
	have_head_ = true;
	size += sizeof(MMapContext);

    if (!Init(filename, size, create)) {
        return false;
    }

    //check version and ip
    MMapContext * head = (MMapContext*)ptr_;
    if (head->magic != 0xac82df46) {
        create = true;
        head->magic = 0xac82df46;
    }

    if (0 != strcmp(ip.c_str(), head->ip)) {
        create = true;
        strncpy(head->ip, ip.c_str(), sizeof(head->ip));
    }

    if (0 != strcmp(version.c_str(), head->version)) {
        create = true;
        strncpy(head->version, version.c_str(), sizeof(head->version));
    }

    if (create) {
        bzero(head->reserve, sizeof(head->reserve));
        bzero(((char*)ptr_)+sizeof(MMapContext), size- sizeof(MMapContext));
    }
	return true;
}

bool MMapOP::Init(const char* filename,size_t size, bool& create) {
	create = false;

	int fd = open(filename, O_RDWR|O_CREAT, 0644);
	if (fd < 0) {
		snprintf(err_buf_, sizeof(err_buf_), "open file[%s] for read/write error:%s",
			filename, strerror(errno));
		return false;
	}

	struct stat filestat;
	if (0 != fstat(fd, &filestat)) {
		snprintf(err_buf_, sizeof(err_buf_), "fstat file[%s] for read/write error:%s",
			filename, strerror(errno) );
		close(fd);
		return false;
	}

	if (size != (size_t)(filestat.st_size)) {
		if (0 !=  ftruncate(fd, size)) {
			snprintf(err_buf_, sizeof(err_buf_), "ftruncate file[%s] for read/write error:%s",
				filename, strerror(errno));
			close(fd);
			return false;
		}
		create = true;  //说明是个新文件
		//std::cout << "ftruncate:" << size << std::endl;
	}

	size_ = size;
	ptr_  = mmap(NULL, size, PROT_READ|PROT_WRITE, MAP_SHARED, fd, 0);
	if (MAP_FAILED == ptr_) {			
		snprintf(err_buf_, sizeof(err_buf_), "ftruncate file[%s] for read/write error:%s", 
				filename, strerror(errno));
		return false;
	}
	close(fd);	
	return true;
}

} // end of namespace util

#ifdef _DEBUG_

struct FileOffset {
    size_t file_offset;
    ino_t  file_inode;
};

int main(int argc, char* argv[])
{
    bool create = false;
    util::MMapOP mmap_op;
    if (!mmap_op.Init("./.sync_file_offset.txt", sizeof(FileOffset), create)) {
        fprintf(stderr, "mmap_op Init failed, errinfo[%s]\n", mmap_op.GetErr());
        return 1;
    }
    FileOffset* file_offset = (FileOffset*)mmap_op.GetPtr();
    // TODO use file_offset recode sync file offset info
    return 0;
}

#endif
