cd $CPU2006_ROOT
source ./shrc
for b in ${CPU2006_BENCHMARKS}; do
	CPU2006_BEFORE=
	for s in ${CPU2006_DATA}; do
		CPU2006_RUNBASE=/benchspec/CPU2006/$b/run/run_base_${s}_${ARCH}.0000
		if [ -d ${CPU2006_ROOT}${CPU2006_RUNBASE} ]; then
			(
			cd ${CPU2006_ROOT}${CPU2006_RUNBASE}
			echo "Running ${s} ${b}..."
			files=`ls`
			for f in $files; do
				CPU2006_BEFORE="${CPU2006_BEFORE} ${CPU2006_ROOT}${CPU2006_RUNBASE}/${f}"
			done
			)
		fi
	done
	runspec --config ${ARCH}.cfg --action onlyrun ${b}
	CPU2006_AFTER=
	for s in ${CPU2006_DATA}; do
		CPU2006_RUNBASE=/benchspec/CPU2006/$b/run/run_base_${s}_${ARCH}.0000
		if [ -d ${CPU2006_ROOT}${CPU2006_RUNBASE} ]; then
			(
			cd ${CPU2006_ROOT}${CPU2006_RUNBASE}
			echo "Running ${s} ${b}..."
			files=`ls`
			for f in $files; do
				CPU2006_AFTER="${CPU2006_AFTER} ${CPU2006_ROOT}${CPU2006_RUNBASE}/${f}"
			done
			)
		fi
	done
	for a in ${CPU2006_AFTER}; do
		found=no
		for b in ${CPU2006_BEFORE}; do
			if [ "x$a" = "x$b" ]; then
				found=yes
				break
			fi
		done
		if [ "x$found" = "xno" ]; then
			echo "cpu2006: $a begin"
			cat $a
			echo "cpu2006: $a end"
		fi
	done
done
