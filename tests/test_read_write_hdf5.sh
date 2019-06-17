#!/usr/bin/bash
option=$1
parent_dir=$(dirname $PWD)
x=1
for FILENAME in 1-datalayout.yaml 2-datalayout.yaml 3-datalayout.yaml
do
        rm -rf "$parent_dir/python/$x-install"
	python2 "$parent_dir/python/podio_class_generator.py" $FILENAME "$parent_dir/python/$x-install" install
        python2 "$parent_dir/python/hdf5_class_generator.py" $FILENAME "$parent_dir/python/$x-install" install
        cd "$parent_dir/python/$x-install"
        cd src
        for i in *.cpp
        do
                h5c++ "$i" -o "${i%.cpp}.out"
        done
        rm *.o
        for i in write_*.out
        do
                ./"$i" 10 >> "$x-input.txt"
        done
        for i in read_*.out
        do
                ./"$i" 10 >> "$x-output.txt"
        done
        cmp --silent "$x-input.txt" "$x-output.txt" && echo "Test Passed! ($x/3)" || echo 'Test Failed!'
        cd "$parent_dir/tests"
	if [ "$option" == "clean" ]
	then
		rm -rf "$parent_dir/python/$x-install"
	fi
	x=$((x+1))
done

