#ifndef __FILE_LOCK_H
#define __FILE_LOCK_H

#include <sys/types.h>

int LockReg(int fd, int cmd, int lock_type, off_t file_offset, int whence, off_t len);

#define WriteLock(fd)     LockReg(fd, F_SETLK,  F_WRLCK, 0, SEEK_SET, 0)
#define WriteLockWait(fd) LockReg(fd, F_SETLKW, F_WRLCK, 0, SEEK_SET, 0)
#define ReadLock(fd)      LockReg(fd, F_SETLK,  F_RDLCK, 0, SEEK_SET, 0)
#define ReadLockWait(fd)  LockReg(fd, F_SETLKW, F_RDLCK, 0, SEEK_SET, 0)
#define UnLock(fd)        LockReg(fd, F_SETLK,  F_UNLCK, 0, SEEK_SET, 0)

int LockFile(const char * file, char * errbuf);

#endif
