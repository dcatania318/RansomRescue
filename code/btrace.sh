#!/bin/sh

TRACEOPTS=""
PARSEOPTS="-b100000"
USAGE="Usage: btrace [-s] [-t] [-w N] [-n N] [-b N] [-a <trace>...] [-r <dbg mnt>] <dev>..."
DIRNAME=`dirname $0`

while getopts "a:w:n:b:r:sthv" c
do
	case $c in
	a)	TRACEOPTS=$TRACEOPTS" -a "$OPTARG" ";;
	w)	TRACEOPTS=$TRACEOPTS" -w "$OPTARG" ";;
	n)	TRACEOPTS=$TRACEOPTS" -n "$OPTARG" ";;
	b)	TRACEOPTS=$TRACEOPTS" -b "$OPTARG" ";;
	r)	TRACEOPTS=$TRACEOPTS" -r "$OPTARG" ";;
	s)	PARSEOPTS=$PARSEOPTS" -s";;
	t)	PARSEOPTS=$PARSEOPTS" -t";;
	h)	PARSEOPTS=$PARSEOPTS" -h";;
	v)	PARSEOPTS=$PARSEOPTS" -v";;
	\?)	echo $USAGE 1>&2
		exit 2
		;;
	esac
done

shift `expr $OPTIND - 1`
if [ $# -eq 0 ]; then
	echo $USAGE 1>&2
	exit 2
fi

blktrace ${TRACEOPTS} -o- $@ | blkparse -f "%M %m %2c %8s %5T.%9t %5p %2a %3d %S %n\n" ${PARSEOPTS} -i- | ${DIRNAME}/preprocessing.out
