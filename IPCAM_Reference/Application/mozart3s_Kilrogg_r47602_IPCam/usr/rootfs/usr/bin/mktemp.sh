#!/bin/sh
# mktemp.sh - A portable and compliant mktemp replacement.
# version 0.2 by Wayne Pollock, Tampa Florida USA, 2007
# Usage: mktemp.sh [ base_file_name ]
#
# TODO: Make production quality: add options and parsing,
#       help, better error handling.
#
# (See also the POSIX definition of pathchk utility, the
# "rationale" section, for examples and a discussion of this.)

set -C  # turn on noclobber shell option

# The only standard utility that provides random numbers:

rand()
{
    awk 'BEGIN {srand();printf "%d\n", (rand() * 10^8);}'
}

umask 177

NAME="$1"
NAME="${NAME:=tmp}"

while :
do
   TMP=${TMPDIR:-/tmp}/$NAME-$$.$(rand)
   : > $TMP && break
done

printf "%s\n" "$TMP"
