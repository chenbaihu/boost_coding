# chenbaihu@didichuxing.com 

CC=gcc
CXX=g++
AR=ar
ARFLAGS=cru
CURRENT_DIR=$(shell echo `pwd`)
SOURCE_DIR=$(CURRENT_DIR)/src
PREFIX=${CURRENT_DIR}/../../install
#TOP_CFLAGS=-g -D_DEBUG
TOP_CFLAGS=-g  
CFLAGS=$(TOP_CFLAGS) -c -fPIC -Wall -Wno-unused-parameter   \
	   -Wno-unused-function -Wunused-variable -Wunused-value   \
	   -Wcast-qual -Wcast-align -Wwrite-strings -Wsign-compare \
	   -Winvalid-pch -fms-extensions -Wfloat-equal -Wextra     \
	   -I./include \
	   -I../../thirdparty/huffman/include \
	   -I../../thirdparty/bzip2/include 
CPPFLAGS= $(CFLAGS) -Woverloaded-virtual -Wsign-promo -fno-gnu-keywords
LDFLAGS=$(TOP_LDFLAGS) ../../thirdparty/huffman/lib/libhuffman.a ../../thirdparty/bzip2/lib/libbz2.a  
LIBNAME=compress

SRCS := $(wildcard src/*.cc) 
OBJS := $(patsubst src/%.cc, src/%.o, $(SRCS)) $(CCOBJS)
DEPS := $(patsubst %.o, %.d, $(OBJS))

TARGET_SO=lib$(LIBNAME).so
TARGET_A=lib$(LIBNAME).a

all : prepare $(TARGET_SO) $(TARGET_A)
	#$(MAKE) install

$(TARGET_A) : $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS) 
	ranlib $@			   

$(TARGET_SO) : $(OBJS)
	$(CXX) $(LDFLAGS)  $(OBJS) -shared -o $@
	
check : prepare $(TARGET_A) $(TARGET_SO)	
	#$(MAKE) install
	$(MAKE) check -C ut

test : prepare $(TARGET_A) $(TARGET_SO)	
	$(MAKE) -C ut

src/%.o : src/%.cc include/%.h
	$(CXX) $(CPPFLAGS) $< -o $@

prepare: 

-include $(DEPS)

t:
	@echo "LIBNAME:" $(LIBNAME)
	@echo "SRCS:" $(SRCS)
	@echo "OBJS:" $(OBJS)
	@echo "CFLAGS:" $(CFLAGS)
	@echo "CPPFLAGS:" $(CPPFLAGS)

#install: $(TARGET_A) $(TARGET_SO) 
#	cp -f $(TARGET_A) $(TARGET_SO) $(PREFIX)/lib

clean:
	rm -rf src/*.o src/*.d $(OBJS) $(DEPS) $(TARGET_SO) $(TARGET_A)
	$(MAKE) clean -C ut 
