CPU2006_ROOT=/opt/cpu2006

cpu2006_elfname()
{
	b=$1
	selfn=${b##*.}
	if [ $b == "482.sphinx3" ]; then
		selfn=sphinx_livepretend
	fi
	if [ $b == "483.xalancbmk" ]; then
		selfn=Xalan
	fi
	echo $selfn
}

for b in ${CPU2006_BENCHMARKS}; do
	CPU2006_ELF=`cpu2006_elfname $b`
	(
		cd ${CPU2006_ROOT}/${b}
		cat ../commands/${b}.${CPU2006_DATA}.cmd | while read line
		do
			if [ ${line:0:1} != '#' ]; then
				echo "Running ${b}..."
				eval ${CPU2006_ELF}_base.${ARCH} ${line}
			fi
		done
	)
done
