#include "crc32.h"

namespace util {

unsigned int Crc32::crc_table[256];

//初始化crc表,生成32位大小的crc表,也可以直接定义出crc表,直接查表, 但总共有256个,看着眼花,用生成的比较方便. 
void Crc32::init_crc_table(void)  
{  
    unsigned int c;  
    unsigned int i, j;  

    for (i = 0; i < 256; i++) {  
        c = (unsigned int)i;  
        for (j = 0; j < 8; j++) {  
            if (c & 1)  
                c = 0xedb88320L ^ (c >> 1);  
            else  
                c = c >> 1;  
        }  
        crc_table[i] = c;  
    }  
}

unsigned int Crc32::crc32(unsigned char * buffer, unsigned int size, unsigned int crc/*=0xffffffff*/)
{
    static bool init_table = true;
    if (init_table) {
        init_crc_table();
        init_table = false;
    }

    for (unsigned int i = 0; i < size; i++) {  
        crc = crc_table[(crc ^ buffer[i]) & 0xff] ^ (crc >> 8);  
    } 

    return crc ;  
}

} // end of namespace util

#ifdef _TEST_

#include <stdio.h> 
#include <string.h>

int main(int argc, char **argv)  
{  
    unsigned char src[] = {"hello crc32"};

    printf("The crc of %s is:%u\n", src, util::Crc32::crc32(src, strlen((const char*)src)));  

    return 0;  
}

#endif
