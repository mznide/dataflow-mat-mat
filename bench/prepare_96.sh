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


for p in Basic; do
	prepare_vec_lmem $p 96
	prepare_vec_lmem $p 192
	prepare_vec_lmem $p 384
done



exit 0
