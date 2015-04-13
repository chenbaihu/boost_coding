#ifndef __SEMOP_H
#define __SEMOP_H

#include <string.h>
#include <sys/sem.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

namespace util {

union semun
{
    int val;                           //<= value for SETVAL
    struct semid_ds *buf;              //<= buffer for IPC_STAT & IPC_SET
    unsigned short int *array;         //<= array for GETALL & SETALL
    struct seminfo *__buf;             //<= buffer for IPC_INFO
};

class SemOP {
public:
	SemOP (key_t key) {
		key_ = key;
		id_ = 0;
		bzero(errbuf_, sizeof(errbuf_));
	}
	bool Init();

	const char * GetErr() const { return errbuf_; }

	int Ctl(int semnum, int cmd, semun un);

	bool Lock();
	bool UnLock();

private:
	key_t key_;
	int   id_;
	char  errbuf_[1024];

	int   op(int num);
};

} // end of namespace util

#endif

