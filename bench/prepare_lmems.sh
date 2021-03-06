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

echo


for p in Basic; do
	prepare_vec_lmem $p 12
	prepare_vec_lmem $p 24
	prepare_vec_lmem $p 48
done

for p in Tiled; do
	prepare_vec_lmem $p 72
	prepare_vec_lmem $p 96
	prepare_vec_lmem $p 120
done



exit 0
