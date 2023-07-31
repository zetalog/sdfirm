for b in ${CPU2006_BENCHMARKS}; do
	for s in ${CPU2006_DATA}; do
		CPU2006_RUNBASE=/benchspec/CPU2006/$b/run/run_base_${s}_${ARCH}.0000
		if [ -d ${CPU2006_ROOT}${CPU2006_RUNBASE} ]; then
			(
			cd ${CPU2006_ROOT}${CPU2006_RUNBASE}
			echo "Running ${s} ${b}..."
			if [ "x$CPU2006_DUMP_ALL_FILES" = "xyes" ]; then
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
			else
				./${b}.sh
				echo "cpu2006: ${CPU2006_ROOT}${CPU2006_RUNBASE}/speccmds.out begin"
				cat ${CPU2006_ROOT}${CPU2006_RUNBASE}/speccmds.out
				echo "cpu2006: ${CPU2006_ROOT}${CPU2006_RUNBASE}/speccmds.out end"
			fi
			)
		fi
	done
done
