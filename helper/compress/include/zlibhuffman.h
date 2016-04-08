#ifndef _ZLIB_HUFFMANCODE_H_
#define _ZLIB_HUFFMANCODE_H_ 

#include "compressor.h"

namespace util { 

#define HM_MAX_DATA_LEN   4*1024*1024 // 4M

#define HM_OK             0
#define HM_PARAM_ERROR    (-2) 
#define HM_DATA_ERROR     (-4)
#define HM_OUTBUFF_FULL   (-8)  
#define HM_ZLIB_ERROR     (-12)  

class ZlibHuffManCode : public Compressor {
    virtual bool Compress(const void* data, size_t data_len, std::string& compress_data);
    virtual bool Uncompress(const void* data, size_t data_len, std::string& uncompresed_data, size_t uncompresed_bound=0);

public:
    static int Compress(const void* source, size_t sourceLen, void* dest, size_t* destLen);
    static int Uncompress(const void* source, size_t sourceLen, void* dest, size_t* destLen);
}; 

} // end of namespace util

#endif
