#include "huffmancode.h" 

#include <stdlib.h>
#include <string.h>

#include "huffman.h" 

namespace util { 

#define HuffManClear(buf, bufsizep) { \
    if (buf!=NULL) {                  \
        free(buf);                    \
        buf = NULL;                   \
    }                                 \
    if (bufsizep!=NULL) {             \
        *bufsizep = 0;                \
    }                                 \
}

#define HuffManClearBuf(buf) {        \
    if (buf!=NULL) {                  \
        free(buf);                    \
        buf = NULL;                   \
    }                                 \
}

int HuffManCode::Compress(const void* source, size_t sourceLen, void* dest, size_t* destLen) 
{
    if (source==NULL || dest==NULL || destLen==NULL) {
        return HM_PARAM_ERROR;
    }
    if (sourceLen==0) { 
        *((char*)dest)=0x0;
        *destLen = 0;
        return HM_OK;
    }

    unsigned char* cbufout    = NULL;   
    unsigned int   cbufoutlen = 0;

    if(huffman_encode_memory((const unsigned char*)source, (unsigned int)sourceLen, &cbufout, &cbufoutlen)!=0) {        
        HuffManClear(cbufout, destLen);
        return HM_DATA_ERROR;
    }

    if (cbufoutlen>(unsigned int)*destLen) {  
#ifdef _DEBUG 
        fprintf(stderr, "HuffManCode::Compress cbufoutlen=%u\tdestLen=%ld\n", cbufoutlen, *destLen);
#endif
        HuffManClear(cbufout, destLen);
        return HM_OUTBUFF_FULL;
    }

    memmove(dest, cbufout, cbufoutlen);  // TODO优化
    *destLen = cbufoutlen;
    HuffManClearBuf(cbufout);
    return HM_OK; 
}

bool HuffManCode::Compress(const void* data, size_t data_len, std::string& compress_data) 
{
    if (data==NULL || data_len>HM_MAX_DATA_LEN) { 
        return false;
    }

    if (data_len==0) {
        compress_data.clear();
        return true;
    } 

    //size_t compress_data_len = HM_MAX_DATA_LEN; 
    //compress_data.resize(compress_data_len); 
    //int ret = Compress(data, data_len, &(compress_data[0]), &compress_data_len);
    //if (ret!=HM_OK) { 
    //#ifdef _DEBUG  
    //    fprintf(stderr, "HuffManCode::Compress ret=%d\n", ret);
    //#endif
    //    return false;
    //}
    //compress_data.resize(cbufoutlen); 
    
    unsigned char* cbufout    = NULL;   
    unsigned int   cbufoutlen = 0;

    if(huffman_encode_memory((const unsigned char*)data, (unsigned int)data_len, &cbufout, &cbufoutlen)!=0) {        
        HuffManClearBuf(cbufout);
        return false;
    } 
    if (cbufoutlen>HM_MAX_DATA_LEN) {
        HuffManClearBuf(cbufout);
        return false;
    } 
    compress_data.resize(cbufoutlen); 
    memmove(&(compress_data[0]), cbufout, cbufoutlen);  // TODO优化
    HuffManClearBuf(cbufout);
    return true;
}

int HuffManCode::Uncompress(const void* source, size_t sourceLen, void* dest, size_t* destLen) 
{
    if (source==NULL || dest==NULL || destLen==NULL || (*destLen)==0 || sourceLen>HM_MAX_DATA_LEN) { 
        return HM_PARAM_ERROR;
    }

    unsigned char* ucbufout    = NULL;
    unsigned int   ucbufoutlen = 0;

    if(huffman_decode_memory((const unsigned char*)source, sourceLen, &ucbufout, &ucbufoutlen)!=0) { 
        HuffManClear(ucbufout, destLen);
        return HM_DATA_ERROR;
    }

    if (ucbufoutlen>(unsigned int)*destLen) { 
        HuffManClear(ucbufout, destLen);
#ifdef _DEBUG 
        fprintf(stderr, "HuffManCode::Uncompress ucbufoutlen=%u\tdestLen=%ld\n", ucbufoutlen,*destLen);
#endif
        return HM_OUTBUFF_FULL;
    }

    memmove(dest, ucbufout, ucbufoutlen);  // TODO优化
    *destLen = ucbufoutlen;
    HuffManClearBuf(ucbufout);
    return HM_OK; 
}

bool HuffManCode::Uncompress(const void* data, size_t data_len, std::string& uncompresed_data, size_t uncompresed_bound/*=0*/) 
{
    if (data==NULL || data_len>HM_MAX_DATA_LEN) {
        return false;
    }
    if (data_len==0) {
        uncompresed_data.clear();
        return true;
    }

    //size_t uncompresed_data_len = HM_MAX_DATA_LEN;
    //uncompresed_data.resize(uncompresed_data_len);
    //int ret = Uncompress(data, data_len, &(uncompresed_data[0]), &uncompresed_data_len);
    //if (ret!=HM_OK) {
    //#ifdef _DEBUG 
    //    fprintf(stderr, "HuffManCode::Uncompress ret=%d\n", ret);
    //#endif
    //    return false;
    //}
    //uncompresed_data.resize(uncompresed_data_len);  

    unsigned char* ucbufout    = NULL;
    unsigned int   ucbufoutlen = 0;

    if(huffman_decode_memory((const unsigned char*)data, data_len, &ucbufout, &ucbufoutlen)!=0) { 
        HuffManClearBuf(ucbufout);
        return false;
    }

    if (ucbufoutlen>HM_MAX_DATA_LEN) { 
        HuffManClearBuf(ucbufout);
        return false;
    }

    uncompresed_data.resize(ucbufoutlen); 
    memmove(&(uncompresed_data[0]), ucbufout, ucbufoutlen);  // TODO优化
    HuffManClearBuf(ucbufout);
    return true;
}

} // endof namespace util 
