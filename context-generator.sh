#!/bin/bash
#generates input for gpt-2
#takes a context file, and generates a gtp2-input using starscape markov 

#or chose randomly
if [ -z "$1" ]
  then
	rContext=$(ls text/ | sort -R | head -n 1);
	context=$(cat text/$rContext)
  else
	context=$(cat $1)
fi

#generate markov token
token=$(./a.out words sentencePieces)

echo $context $'\n' $token

