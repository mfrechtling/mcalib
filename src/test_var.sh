#! /bin/bash
echo "N, t, x" > ./test.res
COUNTER=100
END=100
make clean; make test
while [ $COUNTER -le $END ]; do
	echo "Running MCA at N = $COUNTER"
	./test $COUNTER 100 >> ./test.res
	let COUNTER=COUNTER+1
done