#include "bzip2.h" 

#include "bzlib.h"    //git@github.com:osrf/bzip2_cmake.git

namespace util { 
    
int Bzip2::Compress(const void* source, size_t sourceLen, void* dest, size_t* destLen, int level/*= BZ2_BEST_SPEED*/) 
{
    int ret = BZ2_bzBuffToBuffCompress(
            (char*)dest,     //压缩后的数据
            (unsigned int*)destLen,
            (char*)source,  //文件原始数据
            (unsigned int)sourceLen, 
            level, 
            0,    
            0);
    return ret; // BZ_OK || BZ_CONFIG_ERROR || BZ_PARAM_ERROR || BZ_MEM_ERROR
}

bool Bzip2::Compress(const void* data, size_t data_len, std::string& compress_data) 
{ 
    //unsigned int compress_datalen = BZ2_MAX_DATA_LEN;
    unsigned int compress_datalen = data_len;
    compress_data.resize(compress_datalen);   // TODO 优化, 另外，如果数据有膨胀，会失败
    int ret = Compress(data,
            data_len,
            &(compress_data[0]),    
            (size_t*)&compress_datalen,
            BZ2_BEST_SPEED); 

    if (ret!=BZ_OK) {    
#ifdef _DEBUG 
        fprintf(stderr, "Bzip2::Uncompress ret=%d\n", ret);
#endif
        return false; 
    }
    compress_data.resize(compress_datalen);
    return true;
}

int Bzip2::Uncompress(const void* source, size_t sourceLen, void* dest, size_t* destLen) 
{
    int ret = BZ2_bzBuffToBuffDecompress(
                (char*)dest,       //解压缩后的数据
                (unsigned int*)destLen, 
                (char*)source,     //压缩数据
                (unsigned int)sourceLen,
                0,         
                0);
    return ret; // BZ_CONFIG_ERROR || BZ_PARAM_ERROR || BZ_MEM_ERROR || BZ_OUTBUFF_FULL || BZ_DATA_ERROR || BZ_DATA_ERROR_MAGIC || BZ_UNEXPECTED_EOF || BZ_OK
}

bool Bzip2::Uncompress(const void* data, size_t data_len, std::string& uncompresed_data, size_t uncompresed_bound/*=0*/) 
{
    unsigned int uncompressdatalen = BZ2_MAX_DATA_LEN; //TODO优化
    if (uncompresed_bound!=0) {
        uncompressdatalen = uncompresed_bound+1;
    }
    if (data_len>uncompressdatalen) {
        uncompressdatalen = data_len + BZ2_MAX_DATA_LEN;
    }
    uncompresed_data.resize(uncompressdatalen);

    int ret = Uncompress(
            data,       
            data_len, 
            &(uncompresed_data[0]),  
            (size_t*)&uncompressdatalen);

    if (ret!=BZ_OK) {
#ifdef _DEBUG 
        fprintf(stderr, "Bzip2::Uncompress ret=%d\n", ret);
#endif
        return false;
    }
    uncompresed_data.resize(uncompressdatalen);
    return true; 
} 

} // endof namespace util 
