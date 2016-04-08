#include "file_helper.h"

bool FileHelper::GetFileLines(std::string& filename, std::vector<std::string>& file_lines)
{
    return FileHelper::GetFileLines(filename.c_str(), file_lines);
}

bool FileHelper::GetFileLines(const char* filename, std::vector<std::string>& file_lines)
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

bool FileHelper::CopyFile(const std::string& src, const std::string& dst, std::string& errinfo) 
{
    errinfo.resize(1024);
    return CopyFile(src.c_str(), dst.c_str(), &errinfo[0]);
}

bool FileHelper::CopyFile(const char * src, const char * dst, char * errbuf)
{
    char tmp_dst[256]={0};
    snprintf(tmp_dst, 253,"%s.tmp",dst);

    time_t r_file_time = GetFileTime(src);
    if ( 0 == r_file_time ) {
        snprintf(errbuf,1024,"GetFileTime file[%s] error[%d:%s]", src, errno, strerror(errno));
        return false;
    }

    struct timeval tval[2];
    tval[0].tv_sec = r_file_time;
    tval[0].tv_usec = 0 ;
    tval[1].tv_sec = r_file_time;
    tval[1].tv_usec = 0 ;

    int rfd = open(src, O_RDONLY, 0644);
    if ( rfd < 0 ) {   
        snprintf(errbuf, 1024, "open file[%s] for read error[%d:%s]", src, errno, strerror(errno));
        return false;  
    }

    int wfd = open(tmp_dst, O_CREAT|O_TRUNC|O_WRONLY|O_LARGEFILE, 0744);
    if ( wfd < 0) {   
        snprintf(errbuf, 1024, "open file[%s] for write error[%d:%s]", tmp_dst, errno, strerror(errno));
        close(rfd);
        return false;  
    }

    char buf[4096]={0};
    int rd_len = -1;
    int wd_len = -1;
    bool ret = true;

    while ( (rd_len = Readn(rfd, buf, sizeof(buf))) > 0 ) {
        wd_len = Writen(wfd, buf, rd_len, 0);
        if( rd_len != wd_len ) {
            ret = false;
            snprintf(errbuf, 1024, "writen file [%s] error[%d:%s]", tmp_dst, errno, strerror(errno));
            break;
        }
    }

    if ( -1 == rd_len ) {
        ret = false;
        snprintf(errbuf, 1024, "read file [%s] error[%d:%s]", src, errno, strerror(errno));
    }

    close(rfd);
    close(wfd);

    if ( ret ) {
        if( 0 != utimes(tmp_dst,tval) ) {
            snprintf(errbuf, 1024, "utimes file[%s] error[%d:%s]\n", tmp_dst, errno, strerror(errno));
            return false;
        }

        if( 0 != rename(tmp_dst,dst)) {
            snprintf(errbuf,1024,"rename file[%s->%s] error[%d:%s]\n",tmp_dst,dst,errno,strerror(errno));
            return false;
        } else {
            return true;
        } 
    }

    return ret;
}

bool FileHelper::FilePutContent(const std::string& filename, void * buf, unsigned int len, std::string& errbuf)
{
    errbuf.resize(1024);
    return FilePutContent(filename.c_str(), buf, len, &errbuf[0]);
}

bool FileHelper::FilePutContent(const char * filename, void * buf, unsigned int len, char * errbuf)
{
    int wfd = open(filename, O_CREAT|O_TRUNC|O_WRONLY|O_LARGEFILE,0644);
    if( wfd < 0) {   
        snprintf(errbuf, 1024, "open file[%s] for write error[%d:%s]\n", filename, errno, strerror(errno));
        return false;  
    }
    ssize_t wsize = Writen(wfd, buf, len);  
    if ( wsize < len ) {
        snprintf(errbuf, 1024, "write file[%s] num[%zd<%u] fail[%d:%s]\n", filename, wsize, len, errno, strerror(errno));

        close(wfd);

        unlink(filename);
        return false;  
    }

    close(wfd);
    return true;
}

bool FileHelper::Rename(std::string oldname, std::string newname)
{
    int ret = rename(oldname.c_str(), newname.c_str());
    if (ret!=0) {
        return false;
    }
    return true;
}

bool Mkdir(std::string path) 
{
    size_t cur_split_offset = 0, next_split_offset = 0;

    int iret = 0;
    do {
        // get current dir name.
        cur_split_offset  = path.find_first_of((std::string::value_type)'/', next_split_offset);
        next_split_offset = path.find_first_of((std::string::value_type)'/', cur_split_offset+1);

        // current dir
        iret = ::mkdir(path.substr(0, next_split_offset).c_str(), 0755);

        // next dir
        cur_split_offset = next_split_offset + 1;
    } while (next_split_offset != std::string::npos);
    return true;
}

ssize_t FileHelper::Readn(int fd, void *vptr, size_t n) 
{
	size_t	nleft;
	ssize_t	nread;
	char	*ptr;

	ptr = (char*)vptr;
	nleft = n;
	while (nleft > 0) {
		if ( (nread = read(fd, ptr, nleft)) < 0) {
			if (errno == EINTR || errno == EAGAIN) {
				nread = 0;		/* and call read() again */
			} else {
				return(-1);
			}
		} else if (nread == 0) {
			break;				/* EOF */
		}

		nleft -= nread;
		ptr   += nread;
	}
	return(n - nleft);		/* return >= 0 */
}

ssize_t FileHelper::Writen(int fd, const void *vptr, size_t n,int sleeptime/*=1000*/) 
{
    size_t	nleft;
    ssize_t	nread;
    char	*ptr;

    ptr = (char*)vptr;
    nleft = n;
    while (nleft > 0) {
        if ( (nread = write(fd, ptr, nleft)) <= 0) {
            if (errno == EINTR||errno==EAGAIN) {
                nread = 0;		/* and call read() again */
                if ( sleeptime ) {
                    usleep(sleeptime);
                }
            } else {
                return(-1);
            }
        } 

        nleft -= nread;
        ptr   += nread;
    }
    return(n - nleft);		/* return >= 0 */
}

time_t FileHelper::GetFileTime(const char * filename)
{
    if (filename==NULL) {
        return 0;
    }

    struct stat statbuf;
    if( 0 == stat(filename,&statbuf) ) {
        return  statbuf.st_mtime;
    }
    return 0 ;
}

ino_t FileHelper::GetFileInode(const char * filename)
{
    struct stat buf;
retry:
    if (0 != stat(filename,&buf) )
    {   
        if ( errno == EINTR)
        {
            goto retry;
        }
        return -1;
    }

    return buf.st_ino;
}

ino_t FileHelper::GetFileInode(int fd)
{
    struct stat buf;
retry:
    if (0 != fstat(fd,&buf) )
    {    
        if ( errno == EINTR)
        {
            goto retry;
        }
        return -1;
    }

    return buf.st_ino;
}

off_t FileHelper::GetFileSize(const char * filename)
{
    struct stat statbuf;

    if ( 0 == stat(filename,&statbuf))
    {
        return statbuf.st_size;
    }
    else
    {
        return 0;
    }
}

off_t FileHelper::GetFilePos(int fd)
{
    off_t ret;
retry:
    ret = lseek(fd,0,SEEK_CUR);
    if ( (off_t)-1==ret )
    {   
        if ( errno == EINTR)
        {
            goto retry;
        }
        return ret;
    }

    return ret;
} 


#ifdef _TEST_

int main(int argc, char*argv[]) {
    std::vector<std::string> file_lines;

    if (!FileHelper::GetFileLines("./file_util.cc", file_lines)) {
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
