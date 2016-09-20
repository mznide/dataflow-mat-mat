#!/bin/bash

function print_settings {
#	egrep -r "int nMax = " *
	egrep -r "int tileSize = " *
}

function prepare {

	n=448
	src=PagingAlg
	while (( n <= 448 )); do
		dst=PagingAlg$n
		cp -r $src $dst
		f=$(egrep -r "tileSize = " $dst | cut -d: -f1)
		{
			rm "$f"
			sed -E -e "s/tileSize = [[:digit:]]+/tileSize = $n/" >"$f"
		} <"$f"	
		(( n += 64 ))
	done
}


cd ..

echo

prepare



exit 0
