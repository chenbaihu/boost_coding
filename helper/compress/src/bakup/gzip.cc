#include "gzip.h"

#include <zlib/zlib.h>

namespace util {
    
bool GZip::Compress(const void* data, size_t data_len, std::string& compresed_data)
{
    size_t dest_len_inner = GetCompressBound(data_len);
    compresed_data.resize(dest_len_inner);
    bool r = Compress(data, data_len, &compresed_data[0], &dest_len_inner);
    if (!r) {
        return false;
    }
    assert(dest_len_inner <= compresed_data.size());
    compresed_data.resize(dest_len_inner);
    return true;
}

bool GZip::Uncompress(const void* data, size_t data_len, std::string& uncompresed_data)
{
    size_t dest_len_inner = GetUncompressBound(data, data_len);
    uncompresed_data.resize(dest_len_inner);
    bool r = Uncompress(data, data_len, &uncompresed_data[0], &dest_len_inner);
    if (!r) {
        return false;
    }
    assert(dest_len_inner <= uncompresed_data.size());
    uncompresed_data.resize(dest_len_inner);
    return true;
}

bool GZip::Compress(const void* source, size_t sourceLen, void* dest, size_t* destLen)
{
    return ZZ_OK == gz_compress((const uint8_t*)source, sourceLen, (uint8_t*)dest, destLen);
}

bool GZip::Uncompress( const void* source, size_t sourceLen, void* dest, size_t* destLen )
{
    return ZZ_OK == gz_uncompress((const uint8_t*)source, sourceLen, (uint8_t*)dest, destLen);
}

size_t GZip::GetCompressBound( size_t sourceLen )
{
    return gz_compress_bound(sourceLen);
}

size_t GZip::GetUncompressBound(const void* compressed_data, size_t compressed_data_len)
{
    size_t r = gz_uncompress_bound((const uint8_t*)compressed_data, compressed_data_len);
    if (r > H_MAX_PACKAGE_SIZE) {
        r = H_MAX_PACKAGE_SIZE;
    }
    return r;
}

} // end of namespace util

static void write_u32(uint32_t x, Bytef* dest)
{
    for(int n = 0; n < 4; n++) {
        unsigned char c=(unsigned char)(x & 0xff);
        dest[n] = c;
        x >>= 8;
    }
}

static uint32_t read_u32(const Bytef* source)
{
    return source[0] + (source[1] << 8) + (source[2] << 16) + (source[3] << 24);
}

static int gz_magic[2] = {0x1f, 0x8b}; /* gzip magic header */
#define ASCII_FLAG   0x01 /* bit 0 set: file probably ascii text */
#define HEAD_CRC     0x02 /* bit 1 set: header CRC present */
#define EXTRA_FIELD  0x04 /* bit 2 set: extra field present */
#define ORIG_NAME    0x08 /* bit 3 set: original file name present */
#define COMMENT      0x10 /* bit 4 set: file comment present */
#define RESERVED     0xE0 /* bits 5..7: reserved */

bool gz_check_header(const uint8_t* source, size_t source_len, uint32_t* have_read_count)
{
#define get_byte() (source[readn++])
    int method; /* method byte */
    int flags;  /* flags byte */
    int len;
    int c;

    uint32_t readn = 0;

    if (source_len < 10) {
        return false;
    }

    /* Check the gzip magic header */
    for (len = 0; len < 2; len++) {
        c = source[readn++];
        if (c != gz_magic[len]) {
            //                     if (len != 0) m_zstream.avail_in++, m_zstream.next_in--;
            //                     if (c != EOF) {
            //                         m_zstream.avail_in++, m_zstream.next_in--;
            //                         m_transparent = 1;
            //                     }
            //                     m_z_err =m_zstream.avail_in != 0 ? Z_OK : Z_STREAM_END;
            return false;
        }
    }

    method = source[readn++];
    flags = source[readn++];
    if (method != Z_DEFLATED || (flags & RESERVED) != 0) {
        return false;
    }

    /* Discard time, xflags and OS code: */
    //for (len = 0; len < 6; len++) ;
    readn += 6;
    assert(readn == 10);

    //TODO more check of the length

    if ((flags & EXTRA_FIELD) != 0) { /* skip the extra field */
        len  =  (uInt)get_byte();
        len += ((uInt)get_byte())<<8;
        /* len is garbage if EOF but the loop below will quit anyway */
        //while (len-- != 0 && get_byte() != EOF) ;
        while (len-- != 0)  (void)get_byte();
    }
    if ((flags & ORIG_NAME) != 0) { /* skip the original file name */
        while ((c = get_byte()) != 0 && c != EOF) ;
    }
    if ((flags & COMMENT) != 0) {   /* skip the .gz file comment */
        while ((c = get_byte()) != 0 && c != EOF) ;
    }
    if ((flags & HEAD_CRC) != 0) {  /* skip the header crc */
        for (len = 0; len < 2; len++) (void)get_byte();
    }

    *have_read_count = readn;
    return true;
}

int gz_uncompress( const uint8_t*source, uint32_t sourceLen, uint8_t*dest, size_t *destLen )
{
    z_stream stream;
    int err;

    uint32_t have_read_count = 0;
    if (!gz_check_header(source, sourceLen, &have_read_count)) {
        return Z_DATA_ERROR;
    }

    //4 for crc
    //4 for orginal length
    if (sourceLen < have_read_count + 4 + 4) {
        return Z_DATA_ERROR;
    }

    stream.next_in  = const_cast<Bytef*>(source) + have_read_count;
    stream.avail_in = (uInt)sourceLen - have_read_count - 4 - 4;

    /* Check for source > 64K on 16-bit machine: */
    //if ((uLong)stream.avail_in != sourceLen) return Z_BUF_ERROR;

    stream.next_out = (Bytef*)dest;
    stream.avail_out = (uInt)*destLen;
    //if ((uLong)stream.avail_out != *destLen) return Z_BUF_ERROR;

    stream.zalloc = (alloc_func)0;
    stream.zfree = (free_func)0;

    err = inflateInit2(&stream, -MAX_WBITS);
    if (err != Z_OK) return err;

    err = inflate(&stream, Z_FINISH);
    if (err != Z_STREAM_END) {
        inflateEnd(&stream);
        if (err == Z_NEED_DICT || (err == Z_BUF_ERROR && stream.avail_in == 0))
            return Z_DATA_ERROR;
        return err;
    }
    *destLen = stream.total_out;

    //CRC check
    uint32_t m_crc = crc32(0L, Z_NULL, 0);
    m_crc = crc32(m_crc, dest, *destLen);
    uint32_t icrc32 = read_u32(source + sourceLen - 8);
    if (m_crc != icrc32)
    {
        return Z_DATA_ERROR;
    }

    //Data length check
    uint32_t orignal_len = read_u32(source + sourceLen - 4);
    if (stream.total_out != orignal_len) {
        return Z_DATA_ERROR;
    }

    err = inflateEnd(&stream);
    return err;
}

int gz_compress( const uint8_t*source, uint32_t sourceLen, uint8_t*dest, size_t *destLen )
{
    z_stream stream;
    int err;

    uint8_t header[10]={0x1f,0x8b,Z_DEFLATED, 0 /*flags*/, 0,0,0,0 /*time*/, 0 /*xflags*/, OS_CODE};
    memcpy(dest, header, sizeof(header));
    uint32_t writen = sizeof(header);

    stream.next_in = const_cast<Bytef*>(source);
    stream.avail_in = (uInt)sourceLen;
#ifdef MAXSEG_64K
    /* Check for source > 64K on 16-bit machine: */
    if ((uLong)stream.avail_in != sourceLen) return Z_BUF_ERROR;
#endif
    stream.next_out = (Bytef*)dest + writen;
    stream.avail_out = (uInt)*destLen - writen;

    //if ((uLong)stream.avail_out != *destLen) return Z_BUF_ERROR;

    stream.zalloc = (alloc_func)0;
    stream.zfree = (free_func)0;
    stream.opaque = (voidpf)0;
 
    int strategy = Z_HUFFMAN_ONLY;
    int mem_level = 9;
    //---------------------------------------------------------
    //&d_stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, DEFAULT_WINDOWSIZE, DEFAULT_MEMLEVEL,  Z_DEFAULT_STRATEGY
    uint32_t m_crc = crc32(0L, Z_NULL, 0);
    err = deflateInit2(&stream, Z_DEFAULT_COMPRESSION, Z_DEFLATED, -MAX_WBITS, mem_level, strategy);
    if (err != Z_OK) return err;

    err = deflate(&stream, Z_FINISH);
    if (err != Z_STREAM_END) {
        deflateEnd(&stream);
        return err == Z_OK ? Z_BUF_ERROR : err;
    }

    writen += stream.total_out;

    m_crc = crc32(m_crc, (const Bytef *)source, sourceLen);
    write_u32(m_crc, (Bytef*)dest + writen);
#ifdef _DEBUG
    uint32_t x = read_u32((Bytef*)dest + writen);
    assert(x == m_crc);
#endif
    writen += 4;


    write_u32(stream.total_in, (Bytef*)dest + writen);
#ifdef _DEBUG
    x = read_u32((Bytef*)dest + writen);
    assert(x == stream.total_in);
#endif
    writen += 4;

    *destLen = writen;

    err = deflateEnd(&stream);
    return err;
}

size_t gz_compress_bound( size_t source_len )
{
    return compressBound(source_len) + 10 + 4 + 4;
}

size_t gz_uncompress_bound( const uint8_t *compressed_data, size_t compressed_source_len )
{
    if (compressed_source_len < + 10 + 4 + 4) {
        return 0;
    }

    return read_u32(compressed_data + compressed_source_len - 4);
}

#endif



