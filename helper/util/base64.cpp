#include "base64.h" 

namespace util {

/*{{{*/
static const char base64_table[] =
{ 'A', 'B', 'C', 'D', 'E', 'F', 'G', 'H', 'I', 'J', 'K', 'L', 'M',
    'N', 'O', 'P', 'Q', 'R', 'S', 'T', 'U', 'V', 'W', 'X', 'Y', 'Z',
    'a', 'b', 'c', 'd', 'e', 'f', 'g', 'h', 'i', 'j', 'k', 'l', 'm',
    'n', 'o', 'p', 'q', 'r', 's', 't', 'u', 'v', 'w', 'x', 'y', 'z',
    '0', '1', '2', '3', '4', '5', '6', '7', '8', '9', '+', '/', '\0'
};

static const char base64_pad = '=';

static const short base64_reverse_table[256] = {
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -1, -1, -2, -2, -1, -2, -2,
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
    -1, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, 62, -2, -2, -2, 63,
    52, 53, 54, 55, 56, 57, 58, 59, 60, 61, -2, -2, -2, -2, -2, -2,
    -2,  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14,
    15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, -2, -2, -2, -2, -2,
    -2, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40,
    41, 42, 43, 44, 45, 46, 47, 48, 49, 50, 51, -2, -2, -2, -2, -2,
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2,
    -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2, -2
};
/*}}}*/

bool Base64::encode( const void* pSrcData, size_t inlen, void* outbuf, size_t& outlen )
{
    const unsigned char *current = reinterpret_cast<const unsigned char*>(pSrcData);
    size_t length = inlen;

    if (outlen < ((length + 2) / 3) * 4) {
        return false;
    }

    unsigned char *p = reinterpret_cast<unsigned char*>(outbuf);

    while (length > 2) { /* keep going until we have less than 24 bits */
        *p++ = base64_table[current[0] >> 2];
        *p++ = base64_table[((current[0] & 0x03) << 4) | (current[1] >> 4)];
        *p++ = base64_table[((current[1] & 0x0f) << 2) | (current[2] >> 6)];
        *p++ = base64_table[current[2] & 0x3f];

        current += 3;
        length -= 3; /* we just handle 3 octets of data */
    }

    /* now deal with the tail end of things */
    if (length != 0) {
        *p++ = base64_table[current[0] >> 2];
        if (length > 1) {
            *p++ = base64_table[((current[0] & 0x03) << 4) | (current[1] >> 4)];
            *p++ = base64_table[(current[1] & 0x0f) << 2];
            *p++ = base64_pad;
        } else {
            *p++ = base64_table[(current[0] & 0x03) << 4];
            *p++ = base64_pad;
            *p++ = base64_pad;
        }
    }

    outlen = p - reinterpret_cast<const unsigned char*>(outbuf);
    return true;
}

bool Base64::decode( const void* enc, size_t inlen, void* outbuf, size_t& outlen )
{
    if (outlen < ((inlen + (inlen << 1)) >> 2))
    {
        return false;
    }

    const unsigned char *current = reinterpret_cast<const unsigned char*>(enc);
    int ch = 0, i = 0, j = 0, k = 0;

    /* this sucks for threaded environments */
    unsigned char *result = reinterpret_cast<unsigned char*>(outbuf);

    /* run through the whole string, converting as we go */
    while (inlen-- > 0) {
        ch = *current++;
        if (ch == base64_pad) {
            if (inlen > 0 && *current != '=' && (i % 4) == 1) {
                return false;
            }
            continue;
        }

        ch = base64_reverse_table[ch];
        if (ch < 0) { 
            return false;
        }

        switch(i % 4) {
            case 0:
                result[j] = ch << 2;
                break;
            case 1:
                result[j++] |= ch >> 4;
                result[j] = (ch & 0x0f) << 4;
                break;
            case 2:
                result[j++] |= ch >>2;
                result[j] = (ch & 0x03) << 6;
                break;
            case 3:
                result[j++] |= ch;
                break;
            default:
                break;
        }
        i++;
    }

    k = j;

    /* mop things up if we ended on a boundary */
    if (ch == base64_pad) {
        switch(i % 4) {
            case 1:
                return false;
            case 2:
                k++;
            case 3:
                result[k++] = 0;
        }
    }

    assert(outlen >= size_t(j));
    outlen = j;
    return true;
}

} // end of namespace util 

/////////////////////////////////////////////////////////////

namespace util {
    void Base64::encodeblock( const unsigned char in[3], unsigned char out[4], int len )
    {
        static const char* cb64 = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
        out[0] = cb64[ in[0] >> 2 ];
        out[1] = cb64[ ( ( in[0] & 0x03 ) << 4 ) | ( ( in[1] & 0xf0 ) >> 4 ) ];
        out[2] = (uint8_t) ( len > 1 ? cb64[ ( ( in[1] & 0x0f ) << 2 ) | ( ( in[2] & 0xc0 ) >> 6 ) ] : '=' );
        out[3] = (uint8_t) ( len > 2 ? cb64[ in[2] & 0x3f ] : '=' );
    }

    /*
    ** decodeblock
    ** decode 4 '6-bit' characters into 3 '8-bit' binary bytes
    */
    void Base64::decodeblock( const unsigned char in[4], unsigned char out[3] )
    {
        out[ 0 ] = ( unsigned char ) ( in[0] << 2 | in[1] >> 4 );
        out[ 1 ] = ( unsigned char ) ( in[1] << 4 | in[2] >> 2 );
        out[ 2 ] = ( unsigned char ) ( ( ( in[2] << 6 ) & 0xc0 ) | in[3] );
    } 

    bool Base64::encode( const string& srcData, string& encodedData )
    {
        return encode(srcData.data(), srcData.size(), encodedData);
    }

    bool Base64::decode( const string& srcData, string& decodedData )
    {
        return decode(srcData.data(), srcData.size(), decodedData);
    }

} // end of namespace util 

