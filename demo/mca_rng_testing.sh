#!/bin/bash

COUNTER_n=1
COUNTER_t=1

END_n=200
END_t=53

make clean; make knuth_qmca
echo "n, Type, t, u, v" > DATA/knuth.res

while [ $COUNTER_n -le $END_n ]; do
	let COUNTER_t=1
	while [ $COUNTER_t -le $END_t ]; do
		echo "Running MCA at N = $COUNTER_n and t = $COUNTER_t"
		./knuth_mca $COUNTER_t $COUNTER_n >> DATA/knuth.res
		let COUNTER_t=COUNTER_t+1
	done
	#if [ $COUNTER_n -lt 10 ] 
	#	then
		let COUNTER_n=COUNTER_n+1
	#elif [ $COUNTER_n -lt 100 ] 
	#	then
	#	let COUNTER_n=COUNTER_n+10
	#else
	#	let COUNTER_n=COUNTER_n+100
	#fi
done
