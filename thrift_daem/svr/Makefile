# chenbaihu@didichuxing.com

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

DEBUG_FLAGS=-g -DDAsyncMapCli
DEBUG_FLAGS=-g 
CFLAGS=$(DEBUG_FLAGS) -O2 -fPIC -Wshadow -Wcast-qual -Wcast-align           \
	   -Wwrite-strings -Wsign-compare -Winvalid-pch -fms-extensions -Wall 
CPPFLAGS=$(CFLAGS) -Woverloaded-virtual -Wsign-promo -fno-gnu-keywords 
	   
INCLUDES=-I. -I../gen-cpp -I/usr/local/include/ 

#LDFLAGS=-L/usr/local/lib -lthrift -lthrift_c_glib -lthriftnb -lthriftz -levent -lpthread
#LDFLAGS=-L/usr/local/lib -lthrift -lthriftnb -lthriftz -levent -lpthread
LDFLAGS=-L/usr/local/lib -lthrift -lthriftnb -levent -lpthread

DIRNAME=$(shell basename `pwd`)

ifeq ($(PLATFORM), Linux)
RPATH=-Wl,-rpath
else
RPATH=-rpath
endif

RTFLAGS=$(RPATH)=. \
		$(RPATH)=/usr/local/lib \
		$(RPATH)=./usr_local_lib \

DIRS := . ../gen-cpp
SRCS := $(foreach d, $(DIRS), $(wildcard $(d)/*.cpp))
SRCS := $(filter-out ../gen-cpp/MapService_server.skeleton.cpp, $(SRCS))
SRCS := $(filter-out ../gen-cpp/MapService_async_server.skeleton.cpp, $(SRCS))
#SRCS := $(wildcard *.cpp) 
OBJS := $(patsubst %.cpp, %.o, $(SRCS))
DEPS := $(patsubst %.o, %.d, $(OBJS))

TARGET=$(PARENT_DIRNAME)_$(DIRNAME)

all : 
	$(MAKE) prepare
	$(MAKE) $(TARGET)
	$(MAKE) install

prepare:
	sh -x ../idl.sh

$(TARGET) : $(OBJS)
	$(CXX) $(CPPFLAGS) $(OBJS) $(LDFLAGS) $(RTFLAGS) -o $@

check : $(TARGET)
	./$^

-include $(DEPS)

%.o : %.cc
	$(CXX) -c $(CPPFLAGS) $< -o $@ $(INCLUDES)

%.o : %.cpp
	$(CXX) -c $(CPPFLAGS) $< -o $@ $(INCLUDES)


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

