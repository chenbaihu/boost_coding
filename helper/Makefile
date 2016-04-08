all :
	make -C util
	make -C php_helper       
	make -C memcache_helper  
	make -C redis_helper 
	make -C event_thread_pool  

clean: 
	make -C util clean
	make -C php_helper clean  
	make -C memcache_helper clean  
	make -C redis_helper clean 
	make -C event_thread_pool clean   
	make -C util/ut clean
	make -C php_helper/ut clean  
	make -C memcache_helper/ut clean  
	make -C redis_helper/ut clean 
	make -C event_thread_pool/ut clean   

check:
	make -C util/ut
	make -C php_helper/ut 
	make -C memcache_helper/ut  
	make -C redis_helper/ut 
	make -C event_thread_pool/ut  

