CPU2006_ROOT=/opt/cpu2006

for b in ${CPU2006_BENCHMARKS}; do
	(
		CPU2006_RUNBASE=/benchspec/CPU2006/$b/run/run_base_${CPU2006_DATA}_${ARCH}.0000
		cd ${CPU2006_ROOT}${CPU2006_RUNBASE}
		echo "Running ${b}..."
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
done
