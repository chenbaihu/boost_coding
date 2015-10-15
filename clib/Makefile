CC=gcc
CXX=g++
AR=ar
ARFLAGS=cru
CURRENT_DIR=$(shell echo `pwd`)
SOURCE_DIR=$(CURRENT_DIR)/src

TOP_CFLAGS=-g -O3 -D_DEBUG
CFLAGS=$(TOP_CFLAGS) -fPIC -Wall -std=c++0x 
CPPFLAGS= $(CFLAGS) -Woverloaded-virtual -Wsign-promo -fno-gnu-keywords

LDFLAGS=$(TOP_CFLAGS)  

INCLUDE=-I/usr/local/include/event2/ -I/usr/include/

LIBNAME=clib

SRCS := $(filter-out httpclient.*, $(wildcard *.cpp))
OBJS := $(patsubst %.cpp, .obj/%.o, $(SRCS)) $(CCOBJS)
DEPS := $(patsubst .obj/%.o, %.d, $(OBJS))

VERSION=$(shell svnversion)
TARGET_SO=lib$(LIBNAME).so
TARGET_A=lib$(LIBNAME).a

all : prepare $(TARGET_SO) $(TARGET_A)
	@ln -sf $(TARGET_SO).1.0.$(VERSION) $(TARGET_SO).1
	@ln -sf $(TARGET_SO).1              $(TARGET_SO)
	@ln -sf $(TARGET_A).1.0.$(VERSION)  $(TARGET_A).1
	@ln -sf $(TARGET_A).1               $(TARGET_A)


$(TARGET_A) : $(OBJS)
	$(AR) $(ARFLAGS) $@ $(OBJS) 
	ranlib $@			  
	mv $@ $@.1.0.$(VERSION)

$(TARGET_SO) : $(OBJS)
	$(CXX) $(CPPFLAGS) $(LDFLAGS)  $(OBJS) -shared -o $@.1.0.$(VERSION)
	
check : prepare $(TARGET_A) $(TARGET_SO)	
	$(MAKE) check -C ut

test : prepare $(TARGET_A) $(TARGET_SO)	
	$(MAKE) -C ut

.obj/%.o : %.cpp 
	$(CXX) -c $(CPPFLAGS) $< -o $@ $(INCLUDE)

prepare: 
	@mkdir .obj

-include $(DEPS)

clean:
	rm -rf .obj $(OBJS) $(DEPS) *.so* *.a*
