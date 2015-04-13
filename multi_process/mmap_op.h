#ifndef __MMAPOP_H
#define __MMAPOP_H

#include <sys/mman.h>
#include <string>
#include <iostream>

namespace util {

struct MMapContext
{
	unsigned int magic;
	//int mmapSize;
	char ip[24];
	char version[16];
	char reserve[8];
};

class MMapOP {
public:
    MMapOP()
        :ptr_(NULL), have_head_(false) 
    {
    }
    
    ~MMapOP() 
    {
		if (MAP_FAILED != ptr_) {
			munmap(ptr_,size_);
			ptr_ = NULL;
		}
	}

	bool Init(const char * filename, size_t size, bool & create, const std::string & ip, const std::string & version);
	
    bool Init(const char * filename, size_t size, bool & create);

    void * GetPtr() { 
        if (MAP_FAILED==ptr_) {
            return NULL;
        }

        if ( have_head_ ) {
            return ((char*)ptr_)+sizeof(MMapContext);
        }
        return ptr_;
    }

	const char * GetErr() const { return err_buf_; } 

	int Msync() { //刷新页面到文件
		int ret = 0 ;
		if( MAP_FAILED != ptr_) {
			ret = ::msync(ptr_,size_,MS_SYNC);
		}
		return ret ;
	}
private:
	char   err_buf_[256];
	void*  ptr_;
	size_t size_;
	bool   have_head_;
};

} // end of namespace util

#endif
