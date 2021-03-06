#!/bin/bash

function print_settings {
#	egrep -r "int nMax = " *
	egrep -r "int vectorSize = " *
}

function prepare_vec {
	p=$1	
	vec=$2
	src=${p}Vec
	dst=${p}Vec$2
	cp -r $src $dst
	f=$(egrep -r "vectorSize = " $dst | cut -d: -f1)
	{
		rm "$f"
		sed -E -e "s/vectorSize = [[:digit:]]+/vectorSize = $vec/" >"$f"
	} <"$f"	
}

function prepare_vec_lmem {
	p=$1	
	vec=$2
	src=${p}VecLMem
	dst=${p}Vec$2LMem
	cp -r $src $dst
	f=$(egrep -r "vectorSize = " $dst | cut -d: -f1)
	{
		rm "$f"
		sed -E -e "s/vectorSize = [[:digit:]]+/vectorSize = $vec/" >"$f"
	} <"$f"	
}

cd ..

print_settings
echo


for p in Tiled Transposed; do
	prepare_vec $p 2
	prepare_vec $p 4
	prepare_vec $p 8
	prepare_vec $p 16
done

for p in Tiled Transposed; do
	prepare_vec_lmem $p 2
	prepare_vec_lmem $p 4
	prepare_vec_lmem $p 12
	prepare_vec_lmem $p 24
	prepare_vec_lmem $p 48
done

print_settings

exit 0
