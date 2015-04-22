#!/bin/bash

rm -rf ./log/*

#server_ip="106.39.202.170" 
		  
#server_ip="106.39.202.169"
server_ip=$1
server_port=53
#server_ip="106.120.162.178" 

./udp_test --thread_num=24              \
		   --udp_req_num=10000          \
		   --stderrthreshold=3          \
		   --server_ip="${server_ip}"   \
		   --server_port=${server_port} \
		   --log_dir=./log/             \
		   --data_len=1472              \
		   --total_count=0
