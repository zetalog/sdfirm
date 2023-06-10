CPU2006_ROOT=/opt/cpu2006
CPU2006_BENCHMARKS=`ls ${CPU2006_ROOT}/benchspec/CPU2006}`
CPU2006_DATA="ref test train"

if [ ! -f /usr/bin/perl ]; then
	mkdir /usr/bin
	ln -s $CPU2006_ROOT/bin/specperl /usr/bin/perl
fi

for b in ${CPU2006_BENCHMARKS}; do
	for s in ${CPU2006_DATA}; do
		CPU2006_RUNBASE=/benchspec/CPU2006/$b/run/run_base_${s}_${ARCH}.0000
		if [ -d ${CPU2006_ROOT}${CPU2006_RUNBASE} ]; then
			(
			cd ${CPU2006_ROOT}${CPU2006_RUNBASE}
			echo "Running ${s} ${b}..."
			CPU2006_BEFORE=`ls`
			./${b}.sh
			CPU2006_AFTER=`ls`
			for a in ${CPU2006_AFTER}; do
				found=no
				for b in ${CPU2006_BEFORE}; do
					if [ "x$a" = "x$b" ]; then
						found=yes
						break
					fi
				done
				if [ "x$found" = "xno" ]; then
					echo "cpu2006: ${CPU2006_ROOT}${CPU2006_RUNBASE}/$a begin"
					cat ${CPU2006_ROOT}${CPU2006_RUNBASE}/$a
					echo "cpu2006: ${CPU2006_ROOT}${CPU2006_RUNBASE}/$a end"
				fi
			done
			)
		fi
	done
done
