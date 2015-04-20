#!/bin/bash

#构造磁盘IO
sudo dd if=/dev/sda1 of=/dev/null

#创建一个blkio（块设备IO）的cgroup
#mkdir /sys/fs/cgroup/blkio/chenbaihu

#把读IO限制到1MB/s，并把前面那个dd命令的pid放进去（注：8:0 是设备号，你可以通过ls -l /dev/sda1获得）：
#echo '8:0 1048576'  > /sys/fs/cgroup/blkio/chenbaihu/blkio.throttle.read_bps_device
#将pid添加到创建的这个控制组
#echo [pid] > /sys/fs/cgroup/blkio/chenbaihu/tasks

#使用iotop可以看到进程的读磁盘速度被控制到1MB/s
