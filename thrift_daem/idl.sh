#!/bin/bash

basepath=$(cd `dirname $0`; pwd)
cd $basepath

#/usr/local/bin/thrift -r -gen cpp idl/map.thrift  
/usr/local/bin/thrift -r -strict  --gen cpp:cob_style idl/map.thrift

cd -
