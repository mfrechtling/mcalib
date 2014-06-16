#! /bin/bash
echo "Type, N, t, x" > ./test.res
COUNTER=10
END=10
make clean; make test
while [ $COUNTER -le $END ]; do
	echo "Running MCA at N = $COUNTER"
	./test 24 $COUNTER >> ./test.res
	if [ $COUNTER -lt 10000 ]; then
		let COUNTER=COUNTER+10
	else
		let COUNTER=COUNTER+10
	fi
done