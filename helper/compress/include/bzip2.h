#ifndef _BZIP2_H_
#define _BZIP2_H_ 

#include "compressor.h"

namespace util { 

#define BZ2_BEST_SPEED   9 
#define BZ2_MAX_DATA_LEN 4*1024*1024   // 4M

//bzlib.h
#define BZ_RUN               0
#define BZ_FLUSH             1
#define BZ_FINISH            2

#define BZ_OK                0
#define BZ_RUN_OK            1
#define BZ_FLUSH_OK          2
#define BZ_FINISH_OK         3
#define BZ_STREAM_END        4
#define BZ_SEQUENCE_ERROR    (-1)
#define BZ_PARAM_ERROR       (-2)
#define BZ_MEM_ERROR         (-3)
#define BZ_DATA_ERROR        (-4)
#define BZ_DATA_ERROR_MAGIC  (-5)
#define BZ_IO_ERROR          (-6)
#define BZ_UNEXPECTED_EOF    (-7)
#define BZ_OUTBUFF_FULL      (-8)
#define BZ_CONFIG_ERROR      (-9)

class Bzip2 : public Compressor {
    virtual bool Compress(const void* data, size_t data_len, std::string& compress_data);
    virtual bool Uncompress(const void* data, size_t data_len, std::string& uncompresed_data, size_t uncompresed_bound=0);

public:    
    static int Compress(const void* source, size_t sourceLen, void* dest, size_t* destLen, int level = BZ2_BEST_SPEED); 
    static int Uncompress(const void* source, size_t sourceLen, void* dest, size_t* destLen);
}; 

} // end of namespace util 

#endif
