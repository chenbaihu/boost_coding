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
TOP_CFLAGS=-g 
CFLAGS=$(TOP_CFLAGS) -c -fPIC -Wshadow -Wcast-qual -Wcast-align           \
	   -Wwrite-strings -Wsign-compare -Winvalid-pch -fms-extensions -Wall \
	   -I../include -I../../common/include -I/usr/include/ 

CPPFLAGS=$(CFLAGS) -Woverloaded-virtual -Wsign-promo -fno-gnu-keywords 

LDFLAGS=-L../ -lcompress -L../../common -lcom ../../../thirdparty/huffman/lib/libhuffman.a ../../../thirdparty/bzip2/lib/libbz2.a -L/usr/lib64/ -lgtest -lpthread
#LDFLAGS=../libcompress.a -L/usr/lib64/ -lgtest -lpthread

DIRNAME=$(shell basename `pwd`)

ifeq ($(PLATFORM), Linux)
RPATH=-Wl,-rpath
else
RPATH=-rpath
endif

RTFLAGS=$(RPATH)=. \
		$(RPATH)=../ \
		$(RPATH)=../../common 

SRCS := $(wildcard *.cc)
OBJS := $(patsubst %.cc, %.o, $(SRCS))
DEPS := $(patsubst %.o, %.d, $(OBJS))

TARGET=$(PARENT_DIRNAME)_$(DIRNAME)

all : $(TARGET) 
	$(MAKE) install

check : $(TARGET)
	./$^

$(TARGET) : $(OBJS)
	$(CXX) $(OBJS) $(LDFLAGS) $(RTFLAGS) -o $@

-include $(DEPS)

%.o : %.cc
	$(CXX) $(CFLAGS) $(CPPFLAGS) $< -o $@

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

