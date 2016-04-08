#ifndef _CRC32_H_
#define _CRC32_H_

namespace util {

class Crc32 {
public:
    static unsigned int crc32(unsigned char * buffer, unsigned int size, unsigned int crc=0xffffffff);  

private: 
    static void init_crc_table(void);  
    static unsigned int crc_table[256];  
};

} // end of namespace util 

#endif //#ifndef _CRC32_H_
