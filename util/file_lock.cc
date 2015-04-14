#include "file_lock.h"

#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdio.h>

int LockReg(int fd, int cmd, int lock_type, off_t file_offset, int whence, off_t len)
{
    struct flock st_lock;

    st_lock.l_type=lock_type;       /* F_RDLCK, F_WRLCK, F_UNLCK */
    st_lock.l_start=file_offset;    /* byte offset, relative to whence */
    st_lock.l_whence=whence;        /* SEEK_SET, SEEK_CUR, SEEK_END */
    st_lock.l_len=len;              /* bytes, 0 means to EOF */

    return(fcntl(fd, cmd, &st_lock));
}


int LockFile(const char * file, char * errbuf)
{
    int fd = -1;
    fd = open(file, O_CREAT|O_WRONLY, 0664);
    if (-1 == fd) {
        snprintf(errbuf,1024, "open and create lock file[%s]fail[%s],so return", file, strerror(errno));
        return fd ;
    }

    if (WriteLock(fd) < 0 ) {
       snprintf(errbuf,1024, "lock file[%s] WriteLock fail[%s]\n", file, strerror(errno));
       close(fd);
       return -1;
    }
    return fd;
}


#ifdef _DEBUG_

int main(int argc, char* argv[])
{
    char err[1024];
    int fd = LockFile("./.lock", err);
    if (fd==-1) { 
        printf("err:%s\n", err);
	return 1;
    }
    while (true) {
        sleep(1000);
	continue;
    }
    return 0;
}

#endif
