#/bin/sh
cd drivers/
sh vpl_dmac.sh
sh vma_h4ee.sh
sh vma_ibpe.sh
sh vpl_edmc.sh
sh vpl_tmrc.sh
#sh vpl_ddr2sdmc.sh

# enable WDTC
insmod wdt.ko
sh Godshand.sh

./Godshand.bin -r -a 0x30002000
./Godshand.bin -r -a 0x30002010
./Godshand.bin -r -a 0x30002018
./Godshand.bin -r -a 0x30002038
./Godshand.bin -r -a 0x30002050
./Godshand.bin -r -a 0x30002058
./Godshand.bin -r -a 0x30002078
./Godshand.bin -r -a 0x30002090
./Godshand.bin -r -a 0x30002098
./Godshand.bin -r -a 0x300020B8
./Godshand.bin -r -a 0x300020D0
./Godshand.bin -r -a 0x300020D8
./Godshand.bin -r -a 0x300020E8
