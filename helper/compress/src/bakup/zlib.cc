#include "zlib.h"

#include <zlib/zlib.h>

namespace util {

int ZLib::Compress(const void* source, size_t sourceLen, void* dest, size_t* destLen, int level/* = ZZ_BEST_SPEED*/)
{
    if (NULL == destLen || 
        NULL == dest || 
        *destLen < (GetCompressBound(sourceLen)) || 
        NULL == source) {
        return Z_MEM_ERROR;
    }

    *(uint32_t*)(dest) = htonl(sourceLen);

    uLongf dest_len_inner = *destLen - sizeof(uint32_t);

    int r = compress2((Bytef*)dest + sizeof(uint32_t), &dest_len_inner, (const Bytef*)source, sourceLen, level);

    //add the leading 4 bytes of original data length
    *destLen = dest_len_inner + sizeof(uint32_t);
    return r;
}

bool ZLib::Compress(const void* source, size_t sourceLen, std::string& dest)
{
    size_t dest_len_inner = GetCompressBound(sourceLen);
    dest.resize(dest_len_inner);
    int r = Compress(source, sourceLen, &dest[0], &dest_len_inner);
    if (r != Z_OK) {
        return false;
    }
    assert(dest_len_inner <= dest.size());
    dest.resize(dest_len_inner);
    return true;
}

size_t ZLib::GetCompressBound(size_t sourceLen)
{
    return 4 + compressBound(sourceLen);
}

int ZLib::Uncompress(const void* source, size_t sourceLen, void* dest, size_t* destLen)
{
    if (NULL == destLen || 
        NULL == dest || 
        *destLen < (GetUncompressBound(source, sourceLen)) || 
        NULL == source) {
        return Z_MEM_ERROR;
    }

    uLongf dest_len_inner = *destLen;
    int r = uncompress((Bytef*)dest, &dest_len_inner, (const Bytef *)source + sizeof(uint32_t), sourceLen - sizeof(uint32_t));
    *destLen = dest_len_inner;
    return r;
}

bool ZLib::Uncompress( const void* source, size_t sourceLen, std::string& dest )
{
    size_t dest_len_inner = GetUncompressBound(source, sourceLen);
    dest.resize(dest_len_inner);
    int r = Uncompress(source, sourceLen, &dest[0], &dest_len_inner);
    if (r != Z_OK) {
        return false;
    }
    assert(dest_len_inner <= dest.size());
    dest.resize(dest_len_inner);
    return r == Z_OK;
}

size_t ZLib::GetUncompressBound( const void* compressed_data, size_t compressed_data_len )
{
    (void)compressed_data_len;
    size_t r = ntohl(*(uint32_t*)compressed_data);
    if (r > H_MAX_DATA_SIZE) {
        r = H_MAX_DATA_SIZE;
    }
    return r;
}

} // end of namespace util

int zlib_uncompress(const void *in_data, size_t in_data_len,
            std::string& uncompressed_data)
{
    unsigned long out_buf_len = uncompressed_data.size();
    unsigned int factor=1, maxfactor=32;
    int status = Z_OK;
    void *in_buf = const_cast<void*>(in_data);

    do {
        if(0 == out_buf_len || status == Z_BUF_ERROR) {
            out_buf_len += (unsigned long)in_data_len * (1 << factor++);
            uncompressed_data.resize(out_buf_len);
        }
        status = uncompress((unsigned char*)(&uncompressed_data[0]), &out_buf_len, (Bytef*)in_buf, in_data_len);
    } while ((status == Z_BUF_ERROR) && (factor < maxfactor));

    if (status != Z_OK) {
        return status;
    }
    uncompressed_data.resize(out_buf_len);
    return Z_OK;
}

#endif



