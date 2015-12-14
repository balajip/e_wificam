#!/bin/sh

# Get current epoch time.
current_time=`date +%s`

# Get snapshot.
/usr/bin/msgctrl -f /tmp/venc/c0/command.fifo -h encoder0 -c GetSnapshot > /dev/null 2>&1

# Wait a while.
sleep 2

# Sort files by modification time.
file_list=`ls -lt /tmp/snapshot*.jpg 2> /dev/null | awk '{print$9}'`

pre_file=''

# Search the file whose modification time is large than current time and the difference between them is smallest.
for file in ${file_list}
do
	file_time=`stat -c %Y ${file}`
	diff=$((file_time - current_time))

	if [ ${diff} -lt 0 ]; then
		break
	fi

	pre_file=${file}
done

if [ "${pre_file}" != '' ]; then
	echo -n ${pre_file}
fi

exit 0

