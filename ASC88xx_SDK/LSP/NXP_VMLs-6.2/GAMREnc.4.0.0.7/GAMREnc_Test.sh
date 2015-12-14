export GAMRENC_BIN=./gamrenc
export CTRLFILE=amr_encoder.ctl

echo Input file name = T21.IND			> $CTRLFILE
echo Output file name = T21.amr			>> $CTRLFILE
echo Mode rate file = T21.MOD			>> $CTRLFILE
echo Output Format = 1					>> $CTRLFILE
echo Compliance file name = T21.PAK		>> $CTRLFILE
./$GAMRENC_BIN