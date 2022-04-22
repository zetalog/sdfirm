#!/bin/sh

SCRIPT=`(cd \`dirname $0\`; pwd)`
SRCTREE=`(cd ${SCRIPT}/..; pwd)`
MODCONFIG=${SRCTREE}/scripts/config
VERBOSE=no

fatal()
{
	echo "FATAL: $1"
	exit 1
}

modify_kconfig()
{
	if [ "x${VERBOSE}" = "xyes" ]; then
		if [ "x$1" =  "x--disable" ]; then
			echo "Disabling CONFIG_$2"
		else
			echo "Enabling CONFIG_$2"
		fi
	fi
	# Use --keep-case|-k to allow litmus configurations to be
	# automatically adopted.
	eval $MODCONFIG -k $@
	cfg=`cat ./.config | grep ^CONFIG_$2=`
	if [ "x$1" =  "x--disable" ]; then
		if [ "x$cfg" != "x" ]; then
			fatal "Disable CONFIG_$2 failure, please check dependencies."
		fi
	else
		if [ "x$cfg" = "x" ]; then
			fatal "Enable CONFIG_$2 failure, please check dependencies."
		fi
	fi
}

parse_kconfig()
{
	echo $@ | awk 'NR==1{						\
		s=" ";							\
		p="CONFIG_";						\
		if (match($0, /^CONFIG_/)) {				\
			rem=substr($0, RLENGTH+1);			\
			if (match(rem, /[A-Za-z0-9_]+=/)) {		\
				cfg=substr(rem, 0, RLENGTH-1);		\
				val=substr(rem, RLENGTH+1);		\
				if (match(val, /^y$/)) {		\
					opt="--enable";		 	\
					cmds=opt""s""cfg;		\
				} else if (match(val, /^n$/)) {		\
					opt="--disable";		\
					cmds=opt""s""cfg;		\
				} else if (match(val, /^m$/)) {		\
					opt="--module";			\
					cmds=opt""s""cfg;		\
				} else if (match(val, /^[0-9]+$/)) {	\
					opt="--set-val";		\
					cmds=opt""s""cfg""s""val;	\
				} else if (match(val, /^\".*\"$/)) {	\
					str=substr(val, 1, RLENGTH);	\
					opt="--set-str";		\
					cmds=opt""s""cfg""s""str;	\
				} else {				\
					opt="--set-val";		\
					cmds=opt""s""cfg""s""val;	\
				}					\
				print cmds;				\
			}						\
		}							\
	}'
}

apply_kconfig()
{
	param=`parse_kconfig $@`
	modify_kconfig $param
}

apply_kconfig_file()
{
	while read line
	do 
	 	apply_kconfig $line
	done < $1
}

usage()
{
	echo "Usage:"
	echo "`basename $0` [-v] defconfig modconfig [mode]"
	echo "Where:"
	echo "      [-v]: specify verbosity of defconfig"
	echo " defconfig: specify default configuration"
	echo " modconfig: specify configuration modification file"
	echo " mode: old    - oldconfig after configuration"
	echo "       allno  - allnoconfig after configuration (default)"
	echo "       allyes - allyesconfig after configuration"
	exit $1
}

fatal_usage()
{
	echo "FATAL: $1"
	usage 1
}

while getopts "v" opt
do
	case $opt in
	v) VERBOSE=yes;;
	?) echo "Invalid argument $opt"
	   fatal_usage;;
	esac
done
shift $(($OPTIND - 1))

if [ -z $1 ]; then
	fatal_usage "Missing defconfig"
fi
if [ -z $2 ]; then
	fatal_usage "Missing modconfig"
fi

if [ "x${VERBOSE}" = "xyes" ]; then
	make $1
else
	make $1 > /dev/null
fi
apply_kconfig_file $2

if [ -z $3 ]; then
	echo "Configuration modification done."
else
	MODMODE=$3
	make ${MODMODE}config
	echo "Configuration ${MODMODE} modification done."
fi
