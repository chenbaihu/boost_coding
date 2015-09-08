#ifndef _COMPRESSOR_H_
#define _COMPRESSOR_H_ 

#include <tr1/memory>
#include <string>

namespace util {

#define ZZ_OK            0
#define ZZ_STREAM_END    1
#define ZZ_NEED_DICT     2
#define ZZ_ERRNO        (-1)
#define ZZ_STREAM_ERROR (-2)
#define ZZ_DATA_ERROR   (-3)
#define ZZ_MEM_ERROR    (-4)
#define ZZ_BUF_ERROR    (-5)
#define ZZ_VERSION_ERROR (-6)

#define ZZ_BEST_SPEED             1
#define ZZ_BEST_COMPRESSION       9  

enum  CompressorType { 
    kNoCompress, 
    kHuffman,
    kBZIP2, 
    //kZlibCompress, 
    //kGZipCompress, 
};

class Compressor
{
public:
    virtual ~Compressor() {}

    virtual bool Compress(const void* data, size_t data_len, std::string& compresed_data) = 0;
    virtual bool Uncompress(const void* data, size_t data_len, std::string& uncompresed_data, size_t uncompresed_bound=0) = 0;
};
typedef std::tr1::shared_ptr<Compressor> CompressorPtr;

class FakeCompressor : public Compressor
{
public:
    virtual bool Compress(const void* data, size_t data_len, std::string& compresed_data) {
        compresed_data.assign((const char*)data, data_len);
        return true;
    }
    virtual bool Uncompress(const void* data, size_t data_len, std::string& uncompresed_data, size_t uncompresed_bound=0) {
        uncompresed_data.assign((const char*)data, data_len);
        return true;
    }
};

class CompressorFactory
{
public:
    static CompressorPtr CreateCompressor(int compress_method=kNoCompress);
    //static size_t GetCompressBound(int compress_method, size_t original_data_len);
};

} // end of namespace util

#endif



