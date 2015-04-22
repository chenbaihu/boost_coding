PWD := $(shell pwd)

PLATFORM=$(shell uname)

CXX := g++
#DEBUG=-O3
CXXFLAGS := -g3 $(DEBUG) -Wall -Werror -DMUDUO_STD_STRING -MMD \
	-I./ \
	-I/home/s/safe/include \
	-I/home/s/include \
	-I/usr/local/include \
	-I/usr/include/

LDFLAGS := -pthread \
	-L/home/s/safe/lib \
	-L/home/s/lib \
	-L/usr/local/lib/ 

RTFLAGS := \
	-Wl,-rpath=/home/s/safe/lib \
	-Wl,-rpath=/home/s/lib 

LIBS := -lglog \
	-lgflags \
    -lmuduo_net	\
	-lmuduo_base \
	-lpthread \
	-lz

DIR := .
SRC := $(foreach d, $(DIR), $(wildcard $(d)/*.cc))
OBJ := $(patsubst %.cc, %.o, $(SRC))
DEP := $(patsubst %.o, %.d, $(OBJ))

#TARGET := ../bin/udp_test
TARGET := ./udp_test

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CXX) $^ -o $@ $(RTFLAGS) $(LDFLAGS) $(LIBS)

target: $(TARGET)

%.o : %.cc
	$(CXX) -c $(CXXFLAGS) $< -o $@
%.o : %.cpp
	$(CXX) -c $(CXXFLAGS) $< -o $@

-include $(DEP)

clean:
	-rm -rf $(OBJ) $(TARGET) $(STATIC_LIB) $(DEP)
	-rm -rf *.pid *.log *.core

test: 
	$(MAKE) test -C ut

.PHONY: all target clean test static

