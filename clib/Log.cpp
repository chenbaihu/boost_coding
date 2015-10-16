#include "Log.h"

#include <sys/file.h>
#include <stdarg.h>  
#include <time.h>    
#include <stdlib.h>  
#include <string.h>  
#include <errno.h>   
#include <unistd.h>  
#include <sys/stat.h>

//__thread char Log_Writer::m_buffer[_LOG_BUFFSIZE];  
static __thread uint64_t g_uuid ;                   
static __thread int g_pid = 0;                      

namespace clib {

void set_pid(int pid)                               
{                                                   
    g_pid = pid;                                    
}                                                   
                                                    
void set_uuid(uint64_t log_id)                      
{                                                   
    if( log_id == 0 ){                              
        //uuid_t src_uid;                             
        //uuid_generate( src_uid );                   
        //uint64_t* p_uid = (uint64_t*)src_uid;       
        //g_uuid = p_uid[0];                          
    }                                               
    else{                                           
        g_uuid = log_id;                            
    }                                               
}                                                   
                                                    
uint64_t get_uuid(){                                
    return g_uuid;                                  
}                                                   
                                                    
} // namespace clib

