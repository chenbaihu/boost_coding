#!/bin/bash

#�������IO
sudo dd if=/dev/sda1 of=/dev/null

#����һ��blkio�����豸IO����cgroup
#mkdir /sys/fs/cgroup/blkio/chenbaihu

#�Ѷ�IO���Ƶ�1MB/s������ǰ���Ǹ�dd�����pid�Ž�ȥ��ע��8:0 ���豸�ţ������ͨ��ls -l /dev/sda1��ã���
#echo '8:0 1048576'  > /sys/fs/cgroup/blkio/chenbaihu/blkio.throttle.read_bps_device
#��pid��ӵ����������������
#echo [pid] > /sys/fs/cgroup/blkio/chenbaihu/tasks

#ʹ��iotop���Կ������̵Ķ������ٶȱ����Ƶ�1MB/s
