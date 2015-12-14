#!/bin/sh

# If no parameter provided, notify user of usage.
if [ -z "$1" ]; then
	echo  - make_devs.sh
	echo  - "usage: `basename $0` path/to/dev/dir"
	exit 1
fi

if [ ! -e "$1" ]; then
	echo  - make_devs.sh
	echo  - ERROR: Directory \("$1"\) does not exist.
	echo  - Aborting operation.
	exit 1
fi

cd $1

#if [ -z "${PRJROOT}" ]; then
#	echo "Please source environment file first!"
#	exit 1
#fi
makedev () { 
        rm -f $1
        #echo mknod $1 $2 $3 $4
             mknod $1 $2 $3 $4
        chown root:disk $1
        chmod 660 $1
}

makedevs () {
        rm -f $1*
        makedev $1 b $2 $3
        for part in 1 2 3 4 5 6 7 8 9 10 11 12 13 14 15 16
        do
                makedev $1$part b $2 `expr $3 + $part`
        done 
}

#mknod -m 600 mem c 1 1
mknod -m 666 null c 1 3
#mknod -m 666 zero c 1 5
#mknod -m 644 random c 1 8
#mknod -m 644 urandom c 1 9
#mknod -m 666 ptmx c 5 2
#mknod -m 644 ptyp0 c 2 0
#mknod -m 644 ptyp1 c 2 1
#mknod -m 644 ptyp2 c 2 2
#mknod -m 644 ptyp3 c 2 3
#mknod -m 644 ptyp4 c 2 4
#mknod -m 644 ptyp5 c 2 5
#mknod -m 644 ptyp6 c 2 6
#mknod -m 644 ptyp7 c 2 7
#mknod -m 600 tty0 c 4 0
#mknod -m 600 tty1 c 4 1
#mknod -m 600 tty2 c 4 2
#mknod -m 600 tty3 c 4 3
#mknod -m 600 tty4 c 4 4
#mknod -m 600 tty5 c 4 5
#mknod -m 600 tty6 c 4 6
#mknod -m 600 tty7 c 4 7
mknod -m 600 ttyS0 c 4 64
mknod -m 600 ttyS1 c 4 65
mknod -m 600 ttyS2 c 4 66
mknod -m 600 ttyS3 c 4 67
ln -s ttyS1 ttyS
#mknod -m 666 tty c 5 0
mknod -m 600 console c 5 1
#mknod -m 600 ttyp0 c 3 0
#mknod -m 600 ttyp1 c 3 1
#mknod -m 600 ttyp2 c 3 2
#mknod -m 600 ttyp3 c 3 3
#mknod -m 600 ttyp4 c 3 4
#mknod -m 600 ttyp5 c 3 5
#mknod -m 600 ttyp6 c 3 6
#mknod -m 600 ttyp7 c 3 7
#mknod -m 600 ttyAM0 c 204 16
#mknod -m 600 ttyAM1 c 204 17
#mknod -m 600 ram0 b 1 0
#mknod -m 600 ram1 b 1 1
#mknod -m 600 ram2 b 1 2
#mknod -m 600 ram3 b 1 3
#mknod -m 600 ram4 b 1 4
#mknod -m 600 ram5 b 1 5
#mknod -m 600 ram6 b 1 6
#mknod -m 600 ram7 b 1 7
#mknod -m 600 rom0 b 31 0
#mknod -m 600 rom1 b 31 1
#mknod -m 600 rom2 b 31 2
#mknod -m 600 rom3 b 31 3
#mknod -m 600 audio c 14 4
#mknod -m 600 sda1 b 8 1
#mknod -m 600 dsp0 c 14 3
#mknod -m 600 dsp1 c 14 19
#ln -s dsp0 dsp
#mknod -m 600 mixer c 14 0
#mknod -m 600 sequencer c 14 1
#mknod -m 600 midi00 c 14 2
#makedevs hda  3 0
#makedevs hdb  3 64
#makedevs hdc 22 0
#makedevs hdd 22 64
#makedevs hde 33 0
#makedevs hdf 33 64
#makedevs hdg 34 0
#makedevs hdh 34 64
#makedevs hdi 56 0
#makedevs hdj 56 64
#makedevs hdk 57 0
#makedevs hdl 57 64
#makedevs hdm 88 0
#makedevs hdn 88 64
#makedevs hdo 89 0
#makedevs hdp 89 64
#makedevs hdq 90 0
#makedevs hdr 90 64
#makedevs hds 91 0
#makedevs hdt 91 64
#mknod -m 644 gpio_i2c c 243 0
#ln -s gpio_i2c i2c
#mknod -m 644 rtc c 242 0
#mknod -m 644 gpio0 c 244 0
#mknod -m 644 gpio1 c 244 1
#mknod -m 644 gpio2 c 244 2
#mknod -m 644 gpio3 c 244 3
#mknod -m 644 gpio4 c 244 4
#mknod -m 644 gpio5 c 244 5
#mknod -m 644 gpio6 c 244 6
#mknod -m 644 gpio7 c 244 7
#ln -s gpio0 gpio
#mknod -m 644 i2c_slave c 245 0
#mknod -m 644 vma_vpe c 246 0
#mknod -m 644 vma_me c 247 0
#mknod -m 644 vma_be c 248 0
#mknod -m 644 vma_ae c 249 0
#mknod -m 644 cmmap c 250 0
#mknod -m 644 mmap c 251 0
#mknod -m 644 i2s c 252 0
#mknod -m 644 tmr c 253 0
#ln -s tmr tmr1
#mknod -m 644 tmr2 c 253 1
#mknod -m 644 tracer c 254 0
#mknod -m 644 tracerU c 254 1
#mknod -m 644 mtd0 c 90 0
#mknod -m 644 mtd1 c 90 2
#mknod -m 644 mtdblock1 b 31 1
#mknod -m 644 wdt c 130 0
#ln -s wdt watchdog

#mknod -m 644 CF b 254 0

#ln -s ram0 ramdisk
#ln -s /proc/self/fd fd
#ln -s fd/0 stdin
#ln -s fd/1 stdout
#ln -s fd/2 stderr
