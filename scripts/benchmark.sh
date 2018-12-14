#!/bin/sh

set -e

clean_cache=false
testdir=test/chr22_meth_example/
bamfile=$testdir/reads.sorted.bam
ref=$testdir/humangenome.fa
reads=$testdir/reads.fastq
batchsize=4096
disable_cuda=yes
if command -v nproc > /dev/null
then
	threads=$(nproc --all)
else
	threads=8
fi

# terminate script
die() {
	echo "test.sh: $1" >&2
	exit 1
}

help_msg() {
	echo "Benchmark script for f5c."
	echo "Usage: f5c_dir/script/benchmark.sh [-c] [-b bam file] [-g reference genome] [-r fastq/fasta read] [f5c path] [nanopolish path]"
	echo
	echo "-c                   Clean file system cache between every f5c and nanopolish call."
	echo "-b [bam file]        Same as f5c -b."
	echo "-r [read file]       Same as f5c -r."
	echo "-g [ref genome]      Same as f5c -g."
	echo "-K [n]               Same as f5c -K."
	echo "-C                   Same as f5c --disable-cuda=no."
	echo "-t [n]               Maximum number of threads. Minimum is 8."
	echo "-h                   Show this help message."
}

while getopts b:g:r:t:K:ch opt
do
	case $opt in
		b) bamfile="$OPTARG";;
		g) ref="$OPTARG";;
		r) reads="$OPTARG";;
		t) if [ "$OPTARG" -lt 8 ]
		   then
			die "Thread size must be >= 8"
		   fi
		   threads="$OPTARG";;
		K) batchsize="$OPTARG";;
		c) clean_cache=true;;
		C) disable_cuda=no;;
		h) help_msg
		   exit 0;;
		?) printf "Usage: %s [-c] [-h] [f5c path] [nanopolish path]" $0
		   exit 2;;
	esac
done

shift $(($OPTIND - 1))
f5c_path=$(echo "$*" | awk '{print $1}')
nanopolish_path=$(echo "$*" | awk '{print $2}')

for file in ${bamfile} ${ref} ${reads}
do
	[ -f ${file} ] || die "${file} not found."
done

# run benchmark
t=8
while [ $t -le $threads ]
do
	/usr/bin/time -v ${f5c_path} call-methylation -b ${bamfile} -g ${ref} -r ${reads} -t $t --secondary=yes --min-mapq=0 --print-scaling=yes -K$batchsize --disable_cuda=$disable_cuda > result.txt 2> f5c_$t.log
	if [ $clean_cache = true ]
	then
		sync; echo 3 | sudo tee /proc/sys/vm/drop_caches
	fi
	/usr/bin/time -v ${nanopolish_path} call-methylation -b ${bamfile} -g ${ref} -r ${reads} -t $t -K$batchsize > result.txt 2> nano_$t.log
	if [ $clean_cache = true ]
	then
		sync; echo 3 | sudo tee /proc/sys/vm/drop_caches
	fi
	t=$(( t + 8 ))
done
