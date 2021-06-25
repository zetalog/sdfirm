#!/bin/sh
echo "Running a quick test"
./run.exe -st 1 -s 5k -r 20 > run.test.log
c=0
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 1 -s 50k -r 50 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 1 -s 500k -r 5 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 1 -s 10k -r 250 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 1 -s 100k -r 25 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 1 -s 5k -r 500 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 1 -s 1M -r 3 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 2 -s 50k -r 50 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 2 -s 500k -r 5 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 2 -s 10k -r 250 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 2 -s 100k -r 25 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 2 -s 5k -r 500 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 2 -s 1M -r 3 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 3 -s 50k -r 50 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 3 -s 500k -r 5 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 3 -s 10k -r 250 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 3 -s 100k -r 25 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 3 -s 5k -r 500 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 3 -s 1M -r 3 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 4 -s 50k -r 50 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 4 -s 500k -r 5 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 4 -s 10k -r 250 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 4 -s 100k -r 25 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 4 -s 5k -r 500 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 4 -s 1M -r 3 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 5 -s 50k -r 50 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 5 -s 500k -r 5 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 5 -s 10k -r 250 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 5 -s 100k -r 25 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 5 -s 5k -r 500 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 5 -s 1M -r 3 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 6 -s 50k -r 50 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 6 -s 500k -r 5 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 6 -s 10k -r 250 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 6 -s 100k -r 25 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 6 -s 5k -r 500 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 6 -s 1M -r 3 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 7 -s 50k -r 50 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 7 -s 500k -r 5 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 7 -s 10k -r 250 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 7 -s 100k -r 25 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 7 -s 5k -r 500 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 7 -s 1M -r 3 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 8 -s 50k -r 50 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 8 -s 500k -r 5 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 8 -s 10k -r 250 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 8 -s 100k -r 25 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 8 -s 5k -r 500 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 8 -s 1M -r 3 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 31 -s 50k -r 50 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 31 -s 500k -r 5 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 31 -s 10k -r 250 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 31 -s 100k -r 25 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 31 -s 5k -r 500 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 31 -s 1M -r 3 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 133 -s 50k -r 50 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 133 -s 500k -r 5 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 133 -s 10k -r 250 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 133 -s 100k -r 25 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 133 -s 5k -r 500 > "run.$c.log"
c=$(($c + 1))
echo "Running test $c out of 60"
./run.exe -st 133 -s 1M -r 3 > "run.$c.log"
touch done
echo "Done"
