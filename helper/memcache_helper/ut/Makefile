CC=gcc
CXX=g++
AR=ar
ARFLAGS=cru
PLATFORM=$(shell uname)
CURRENT_DIR=$(shell echo `pwd`)
PARENT_DIRNAME=$(shell basename $(shell dirname $(CURRENT_DIR)))
SOURCE_DIR=$(CURRENT_DIR)/src
#PREFIX=${CURRENT_DIR}/../../../install
PREFIX=${CURRENT_DIR}/../
DEBUG_CFLAGS=-g 
CFLAGS=$(DEBUG_CFLAGS) -c -fPIC -Wshadow -Wcast-qual -Wcast-align         \
	   -Wwrite-strings -Wsign-compare -Winvalid-pch -fms-extensions -Wall \

CPPFLAGS=$(CFLAGS) -Woverloaded-virtual -Wsign-promo -fno-gnu-keywords 

INCLUDES=-I../ \
		 -I../../util/ \
		 -I/usr/include/  

LDFLAGS=../libmemcache_cli_pool.a \
		../../util/libutil.a \
		../../../thirdparty/nodemgr/lib/libNodeManager.a \
		../../../thirdparty/libuuid/lib/libuuid.a \
		../../../thirdparty/memcache/lib/libmemcached.a \
		-L../../../thirdparty/config/lib/ -lconfig++ \
		-L/usr/lib64/ -lgtest -lpthread

DIRNAME=$(shell basename `pwd`)

ifeq ($(PLATFORM), Linux)
RPATH=-Wl,-rpath
else
RPATH=-rpath
endif

RTFLAGS=$(RPATH)=. \
		$(RPATH)=../../../thirdparty/config/lib/ 

SRCS := $(wildcard *.cpp)
OBJS := $(patsubst %.cpp, %.o, $(SRCS))
DEPS := $(patsubst %.o, %.d, $(OBJS))

TARGET=$(PARENT_DIRNAME)_$(DIRNAME)

all : $(TARGET) 
	#$(MAKE) install

check : $(TARGET)
	./$^

$(TARGET) : $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) $(RTFLAGS) -o $@ 

%.o : %.cpp
	$(CXX) $(CFLAGS) $(CPPFLAGS) $< -o $@ $(INCLUDES)

-include $(DEPS)

t:
	@echo "DIRNAME:" $(DIRNAME)
	@echo "PARENT_DIRNAME:" $(PARENT_DIRNAME)
	@echo "SRCS:" $(SRCS)
	@echo "OBJS:" $(OBJS)
	@echo "CFLAGS:" $(CFLAGS)
	@echo "CPPFLAGS:" $(CPPFLAGS)

install: $(TARGET) 
	cp -f $(TARGET) $(PREFIX)/

clean:
	rm -rf *.o *.d $(OBJS) $(DEPS) $(TARGET) 

