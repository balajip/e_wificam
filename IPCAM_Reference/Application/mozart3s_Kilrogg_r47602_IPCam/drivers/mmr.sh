#!/bin/sh -e

# mmr.sh
# Read back predefined regions of registers.
#
# Usage: mmr.sh [path of Godshand.ko]
# If path of Godshand.ko is not specified, default path is /drivers/Godshand.ko
#

#=======================================================================
# define the register regions to be read:
#    MMR="${MMR} <start of mmr address> <end of mmr address>"
#
MMR=""
### System Controller
MMR="${MMR} 0x4f800000 0x4f80005c"
### PUB
MMR="${MMR} 0x50000000 0x500002b0"
### DDR32SDMC 0
MMR="${MMR} 0xc4000000 0xc4000044"
### DDR32SDMC 1
MMR="${MMR} 0xc5000000 0xc5000044"



#=======================================================================
GODSHAND_DRIVER=/drivers/Godshand.ko
[ $# -ge 1 ] && GODSHAND_DRIVER=$1

if [ "`lsmod | grep Godshand | cut -d ' ' -f 1`" != "Godshand" ]; then
	if [ -e $GODSHAND_DRIVER ]; then
		insmod $GODSHAND_DRIVER
	else
		echo "cannot find $GODSHAND_DRIVER" && exit 1
	fi
	[ $? -eq 0 ] || { echo "cannot install Godshand driver" && exit 1; }
fi

GODSHAND=`dirname $GODSHAND_DRIVER`/Godshand.bin


set -- $MMR

while [ $# -gt 0 ] ; do
	start=`printf %u $1`
	end=`printf %u $2`
	range=`expr $end - $start`
	shift 2

	offset=0
	while [ $offset -le $range ] ; do
		$GODSHAND -r -a $(printf 0x%x $(expr $start + $offset)) | cut -s -d ' ' -f 5,7 | awk '{print "0x"$1" -> 0x"$2}'
		offset=`expr $offset + 4`
	done
done
