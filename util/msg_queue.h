#ifndef __MSG_QUEUE_H
#define __MSG_QUEUE_H

#include <deque>
#include "util_lock.h"

namespace util {

const int g_maxqueuesize = 300000;

template <typename MTYPE, typename MUTEX=util::NullMutex >
class MsgQueue {
private:
    std::deque<MTYPE* > queue_;
    MUTEX               lock_;

public:
    MsgQueue() {}
    
    ~MsgQueue() {
    	util::Guard<MUTEX> guard(lock_);
    	while(queue_.size() > 0) {
    		MTYPE* pvalue = queue_.front();
    		if (pvalue != NULL) {
    			delete pvalue;
    			queue_.pop_front();
    		} else {
    			break;
    		}
    	}
    }
    
    bool post_msg(MTYPE* msg) {
    	util::Guard<MUTEX> guard(lock_);
    	if(queue_.size() > g_maxqueuesize) {
    		return false;
    	}
    	queue_.push_back(msg);
    	return true;
    }
    
    MTYPE* get_msg() {
    	util::Guard<MUTEX> guard(lock_);
    	
    	if(queue_.size() <= 0)
    		return NULL;
    	
    	MTYPE* pvalue = queue_.front();
    	if(pvalue != NULL) {
    		queue_.pop_front();
    		return pvalue;
    	} else {
    		return NULL;
    	}	
    }
    
    int size(){
    	util::Guard<MUTEX> guard(lock_);
    	return queue_.size();
    }
};

template <typename MTYPE, typename MUTEX=util::NullMutex  >
class MsgQueueCond {
private:
    std::deque<MTYPE* > queue_;
	
    MUTEX lock_;
    util::Condition<util::Mutex> cond_;

public:
    MsgQueueCond():cond_(lock_) {}
    
    ~MsgQueueCond() {	
    	util::Guard<MUTEX> guard(lock_);
    	while (queue_.size() > 0) {
    		MTYPE* pvalue = queue_.front();
    		if(pvalue != NULL) {
    			delete pvalue;
    			queue_.pop_front();
    		} else {
    			break;
    		}
    	}
    }
    
    bool post_msg(MTYPE* msg) {
    	util::Guard<MUTEX> guard(lock_);	
    if (queue_.size() >(size_t) g_maxqueuesize) {
    		//printf("full [%d]\n",g_maxqueuesize);
    		return false;
    	}
    
    	queue_.push_back(msg);
    	cond_.signal();
    	return true;
    }
    
    MTYPE* get_msg() {
    	util::Guard<MUTEX> guard(lock_);
    	
    	while (queue_.size() <= 0) {
    		cond_.wait();
    	}
    	
    	MTYPE* pvalue = queue_.front();
    	if (pvalue != NULL) {
    		queue_.pop_front();
    		return pvalue;
    	} else {
    		return NULL;
    	}	
    }
    
    MTYPE* get_msg(const struct timespec* time) {	
    	util::Guard<MUTEX> guard(lock_);
    	
    	if (queue_.size() <= 0) {
    		cond_.wait(time);
    		if(queue_.size() <= 0) {
    			return NULL;
    		}
    	}
    	
    	MTYPE* pvalue = queue_.front();
    	if (pvalue != NULL) {
    		queue_.pop_front();
    		return pvalue;
    	} else {
    		return NULL;
    	}	
    }
    
    int size() {
    	util::Guard<MUTEX> guard(lock_);  
    	return queue_.size();
    }
};

}

#endif  

