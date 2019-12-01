work_dir=./init/gem5

case_num=1
gem5_conf=$work_dir/simpoint_slice_$case_num.gem5
> $gem5_conf
echo "GEM5_SRC=../gem5" >> $gem5_conf
echo "SE_ARCH=arm64" >> $gem5_conf
echo "SE_PROG=cpu2006-mcf" >> $gem5_conf
echo "SIM_CHECKPOINT=18" >> $gem5_conf

case_num=2
gem5_conf=$work_dir/simpoint_slice_$case_num.gem5
> $gem5_conf
echo "GEM5_SRC=../gem5" >> $gem5_conf
echo "SE_ARCH=arm64" >> $gem5_conf
echo "SE_PROG=hello" >> $gem5_conf
echo "SIM_CHECKPOINT=2" >> $gem5_conf

case_num=3
gem5_conf=$work_dir/simpoint_slice_$case_num.gem5
> $gem5_conf
echo "GEM5_SRC=../gem5" >> $gem5_conf
echo "SE_ARCH=arm64" >> $gem5_conf
echo "SE_PROG=hannoi20" >> $gem5_conf
echo "SIM_CHECKPOINT=5" >> $gem5_conf

case_num=1
gem5_conf=$work_dir/simpoint_slice_$case_num.gem5

scripts/gem5sim.sh -s gem5sim -x Exec -g $gem5_conf > $work_dir/mkslice_$case_num.log 
cp $work_dir/simpoint_slice.S $work_dir/simpoint_slice_$case_num.S
cp $work_dir/simpoint_func.c $work_dir/simpoint_func_$case_num.c
cat $work_dir/mkslice_$case_num.log | grep @ | cut -d ":" -f 3 > $work_dir/st_$case_num-pos.txt
cat $work_dir/mkslice_$case_num.log | grep @ | cut -d ":" -f 4     | cut -d "/" -f 1 > $work_dir/st_$case_num-i.txt
cat $work_dir/mkslice_$case_num.log | grep @ | cut -d ":" -f 4,5,6 | cut -d "/" -f 1 > $work_dir/st_$case_num-ida.txt
cat $work_dir/st_$case_num-ida.txt | sed 's/A=.*//' > $work_dir/st_$case_num-id.txt

ls -l $work_dir/st_$case_num-*.txt
cat $gem5_conf
