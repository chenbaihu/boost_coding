IDC_NAME=`hostname | awk -F'.' '{print $3}'`
case ${IDC_NAME} in
	idc1)
	ln -sf ${ROOTPATH}/etc.latest/zookeeper_conf/idc1_zookeeper.conf ${ROOTPATH}/etc.latest/zookeeper.conf 
	;;
	idc2)
	ln -sf ${ROOTPATH}/etc.latest/zookeeper_conf/idc2_ookeeper.conf  ${ROOTPATH}/etc.latest/zookeeper.conf 
	;;
	idc3)
	ln -sf ${ROOTPATH}/etc.latest/zookeeper_conf/idc3_zookeeper.conf ${ROOTPATH}/etc.latest/zookeeper.conf 
	;;
	idc5)
	ln -sf ${ROOTPATH}/etc.latest/zookeeper_conf/idc4_zookeeper.conf ${ROOTPATH}/etc.latest/zookeeper.conf 
	;;
	*)
	ln -sf ${ROOTPATH}/etc.latest/zookeeper_conf/idc5_zookeeper.conf ${ROOTPATH}/etc.latest/zookeeper.conf 
	;;
esac
if [ $? -ne 0 ] 
then
	echo "ln -sf ${ROOTPATH}/etc.latest/zookeeper_conf/${IDC_NAME}_zookeeper.conf ${ROOTPATH}/etc/_zookeeper.conf failed"
	exit 4
fi

