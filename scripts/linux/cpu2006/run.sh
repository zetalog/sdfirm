for b in ${CPU2006_BENCHMARKS}; do
	for s in ${CPU2006_DATA}; do
		CPU2006_RUNBASE=/benchspec/CPU2006/$b/run/run_base_${s}_${ARCH}.0000
		if [ -d ${CPU2006_ROOT}${CPU2006_RUNBASE} ]; then
			(
			cd ${CPU2006_ROOT}${CPU2006_RUNBASE}
			echo "Running ${s} ${b}..."
			./${b}.sh
			echo "cpu2006: ${CPU2006_ROOT}${CPU2006_RUNBASE}/speccmds.out begin"
			cat ${CPU2006_ROOT}${CPU2006_RUNBASE}/speccmds.out
			echo "cpu2006: ${CPU2006_ROOT}${CPU2006_RUNBASE}/speccmds.out end"
			)
		fi
	done
done
