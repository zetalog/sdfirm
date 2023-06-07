CPU2006_ROOT=/opt/cpu2006

for b in ${CPU2006_BENCHMARKS}; do
	(
		CPU2006_RUNBASE=/benchspec/CPU2006/$b/run/run_base_${CPU2006_DATA}_${ARCH}.0000
		cd ${CPU2006_ROOT}${CPU2006_RUNBASE}
		echo "Running ${b}..."
		./${b}.sh
	)
done
