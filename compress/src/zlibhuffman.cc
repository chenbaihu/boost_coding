#include "zlibhuffman.h" 

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "zlib.h" 

namespace util { 

int ZlibHuffManCode::Compress(const void* source, size_t sourceLen, void* dest, size_t* destLen) 
{
    if (source==NULL || dest==NULL || destLen==NULL || *destLen==0) {
    #ifdef _DEBUG  
        fprintf(stderr, "%s:%d HM_PARAM_ERROR\n", __FILE__, __LINE__);
    #endif
        return HM_PARAM_ERROR;
    }
    if (sourceLen==0) { 
        *((char*)dest)=0x0;
        *destLen = 0;
        return Z_OK;
    }

    z_stream c_stream; 
    //memset(&c_stream, 0x0, sizeof(c_stream));
    c_stream.zalloc = Z_NULL;
    c_stream.zfree  = Z_NULL;
    c_stream.opaque = Z_NULL; 
    int err = Z_OK; 

    //err = deflateInit2(&c_stream, -1, 8, 15 + 16, 8, 0);
    err = deflateInit2(&c_stream, 
            Z_BEST_SPEED,     // 压缩级别:Z_NO_COMPRESSION 0不压缩; Z_BEST_SPEED 1压缩速度最快; Z_BEST_COMPRESSION 9压缩率最高
            Z_DEFLATED,       // 压缩方式，目前只支持一种；
            15,               // windowBits can also be –8..–15 for raw deflate. In this case, -windowBits determines the window size. deflate() will then generate raw deflate data with no zlib header or trailer, and will not compute an adler32 check value.
            8,                // The memLevel parameter specifies how much memory should be allocated for the internal compression state. memLevel=1 uses minimum memory but is slow and reduces compression ratio; memLevel=9 uses maximum memory for optimal speed. The default value is 8. See zconf.h for total memory usage as a function of windowBits and memLevel.
            Z_HUFFMAN_ONLY);  // Z_DEFAULT_STRATEGY);
    if (err!=Z_OK) {
    #ifdef _DEBUG  
        fprintf(stderr, "%s:%d deflateInit2 err=%d\n", __FILE__, __LINE__, err);
    #endif
        return err;
    }

    ////////smart buff
    //c_stream.next_in  = (z_const unsigned char *)source;
    //c_stream.next_out = dest; 
    //while (c_stream.total_in != (uLong)sourceLen && c_stream.total_out < (uLong)*destLen) {
    //    c_stream.avail_in = c_stream.avail_out = 1; /* force small buffers */
    //    err = deflate(&c_stream, Z_NO_FLUSH); 
    //    if (err!=Z_OK) { 
    //        return err;
    //    }
    //}

    ///* Finish the stream, still forcing small buffers: */
    //for (;;) {
    //    c_stream.avail_out = 1;
    //    int err = deflate(&c_stream, Z_FINISH);
    //    if (err == Z_STREAM_END) break;                                         
    //    if (err!=Z_OK) { 
    //        return err;
    //    }
    //}   
    //err = deflateEnd(&c_stream); 
    //if (err!=Z_OK) {
    //    return err;
    //}
    //*comprLen = (size_t)c_stream.total_out;

    ////////large buf
    c_stream.next_in   = (z_const Byte*)source;;
    c_stream.avail_in  = (uInt)sourceLen;

    c_stream.next_out  = (Byte*)dest;
    c_stream.avail_out = (uInt)*destLen;                                              

    err = deflate(&c_stream, Z_NO_FLUSH); 
    if (err!=Z_OK) {
    #ifdef _DEBUG  
        fprintf(stderr, "%s:%d deflate err=%d\n", __FILE__, __LINE__, err);
    #endif
        return err;
    }
    
    if (c_stream.avail_in!=0) { 
    #ifdef _DEBUG  
        fprintf(stderr, "%s:%d avail_in=%d\n", __FILE__, __LINE__, c_stream.avail_in);
    #endif
        return HM_ZLIB_ERROR;
    }
    
    err = deflate(&c_stream, Z_FINISH);                            
    if (err != Z_STREAM_END) {  // 内存不够
    #ifdef _DEBUG  
        fprintf(stderr, "%s:%d deflate Z_FINISH err=%d\n", __FILE__, __LINE__, err);
    #endif
        return Z_BUF_ERROR;
    }                                                              

    err = deflateEnd(&c_stream);
    if (err!=Z_OK) {
    #ifdef _DEBUG  
        fprintf(stderr, "%s:%d deflateEnd err=%d\n", __FILE__, __LINE__, err);
    #endif
        return err;
    }

    #ifdef _DEBUG  
        fprintf(stdout, "%s:%d\t"
                "sourceLen=%lu\t"
                "avail_in=%d\t" 
                "total_out=%lu\t"
                "avail_out=%d\n", 
                __FILE__, __LINE__, 
                sourceLen, 
                c_stream.avail_in, 
                c_stream.total_out, 
                c_stream.avail_out);
    #endif
    *destLen = (size_t)c_stream.total_out;
    return Z_OK; 
}

bool ZlibHuffManCode::Compress(const void* data, size_t data_len, std::string& compress_data) 
{
    if (data==NULL || data_len==0) {
        compress_data.clear();
        return true;
    }

    if (data_len>HM_MAX_DATA_LEN) { 
    #ifdef _DEBUG  
        fprintf(stderr, "%s:%d data_len=%lu too large\n", __FILE__, __LINE__, data_len);
    #endif
        compress_data = std::string((const char*)data, data_len);
        return false;
    }

    //size_t compress_datalen = data_len+HM_MAX_DATA_LEN;
    size_t compress_datalen = data_len;
    compress_data.resize(compress_datalen);   // TODO 优化, 另外，如果数据有膨胀，会失败

    int ret = Compress(data, data_len, &(compress_data[0]), &compress_datalen);
    if (ret!=Z_OK) { 
    #ifdef _DEBUG  
        fprintf(stderr, "%s:%d ret=%d\n", __FILE__, __LINE__, ret);
    #endif
        return false;
    }
    compress_data.resize(compress_datalen);     
    #ifdef _DEBUG  
        fprintf(stdout, "%s:%d\t"
                "compress_datalen=%lu\t" 
                "compress_data.size=%lu\n", 
                __FILE__, __LINE__, 
                compress_datalen, 
                compress_data.size());
    #endif
    return true;
}

int ZlibHuffManCode::Uncompress(const void* source, size_t sourceLen, void* dest, size_t* destLen) 
{
    if (source==NULL || dest==NULL || destLen==NULL || (*destLen)==0 || sourceLen>HM_MAX_DATA_LEN) { 
    #ifdef _DEBUG 
        fprintf(stderr, "%s:%d HM_PARAM_ERROR\n", __FILE__, __LINE__);
    #endif
        return HM_PARAM_ERROR;
    }

    int err;
    z_stream d_stream; /* decompression stream */                               
    memset(&d_stream, 0x0, sizeof(d_stream)); 
 
    d_stream.zalloc = Z_NULL;
    d_stream.zfree  = Z_NULL;
    d_stream.opaque = Z_NULL;
    
    d_stream.next_in  = (z_const unsigned char *)source;
    d_stream.avail_in = (uInt)sourceLen;
    
    err = inflateInit(&d_stream); 
    if (err!=Z_OK) {
    #ifdef _DEBUG 
        fprintf(stderr, "%s:%d inflateInit failed, err=%d\n", __FILE__, __LINE__, err);
    #endif
        return err;
    }

    for (;;) {
        d_stream.next_out  = (Byte*)dest;            /* discard the output */
        d_stream.avail_out = (uInt)*destLen;
        err = inflate(&d_stream, Z_NO_FLUSH);
        if (err == Z_STREAM_END) break; 
        if (err!=Z_OK) {
        #ifdef _DEBUG  
            fprintf(stderr, "%s:%d inflate failed, err=%d\n", __FILE__, __LINE__, err);
        #endif
            return err;
        }
    }   
    
    err = inflateEnd(&d_stream); 
    if (err!=Z_OK) {
    #ifdef _DEBUG 
        fprintf(stderr, "%s:%d inflateInit failed, err=%d\n", __FILE__, __LINE__, err);
    #endif 
        return err;
    }
    #ifdef _DEBUG 
        fprintf(stdout, "%s:%d total_out=%lu\n", __FILE__, __LINE__, d_stream.total_out);
    #endif 
    *destLen = d_stream.total_out;
    return Z_OK; 
}

bool ZlibHuffManCode::Uncompress(const void* data, size_t data_len, std::string& uncompresed_data, size_t uncompresed_bound/*=0*/) 
{
    if (data==NULL || data_len>HM_MAX_DATA_LEN) {
        return false;
    }
    if (data_len==0) {
        uncompresed_data.clear();
        return true;
    }

    size_t uncompressdatalen = HM_MAX_DATA_LEN; //TODO优化  
    if (uncompresed_bound!=0) {  
        uncompressdatalen = uncompresed_bound+1;
    }
    if (data_len>uncompressdatalen) {   
        uncompressdatalen = data_len + HM_MAX_DATA_LEN;
    }
    uncompresed_data.resize(uncompressdatalen);

    int ret = Uncompress(data, data_len, &(uncompresed_data[0]), &uncompressdatalen);
    if (ret!=Z_OK) {
    #ifdef _DEBUG 
        fprintf(stderr, "%s:%d ret=%d\n", __FILE__, __LINE__, ret);
    #endif
        return false;
    }
    uncompresed_data.resize(uncompressdatalen);  
    return true;
}

} // endof namespace util 
