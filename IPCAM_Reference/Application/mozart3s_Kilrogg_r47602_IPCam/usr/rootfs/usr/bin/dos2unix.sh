#!/bin/sh

#echo $#

TMPFILE=`/usr/bin/mktemp.sh`
#echo "----->$?"

#echo "tmpfile:"$TMPFILE

if [ $# -eq 1 ];then
	if [ -f $1 ];then
		#echo "file $1"
		tr -d '\r' < $1  >  $TMPFILE
		mv $TMPFILE $1
	fi
else
	echo "Please enter a existing file name!"
fi

