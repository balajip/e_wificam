#/bin/sh

if [ -z $1 ] || [ -z $2 ] || [ -z $3 ] || [ -z $4 ];then
	echo "/usr/sbin/venc_receiver [istream_no(0/1/2/3/4)] [frame_num] [path] [bDump](0/1/2)"
	exit
fi

echo "Begin to output bistream to $3..."
/usr/sbin/venc_receiver $1 $2 $3 $4
