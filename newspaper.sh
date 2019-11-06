#!/bin/bash

#read input into a file
cat > /tmp/text

#chose a 'space' picture at random
#and then calculate some numbers on where to place overlays
space=$(ls space/ | sort -R | head -n 1);
IFS='x' read -ra spaceSize <<< "$space"
spaceW=${spaceSize[0]}
spaceH=${spaceSize[1]}
middleSpaceW=$(( $spaceW /2 ))
middleSpaceH=$(( $spaceH /2 ))


#start setting up a spaceComposite 
cp space/$space spaceComposite.png
numOverlays=$(shuf -i 1-4 -n 1)

#add a random number of 'overlays' to the image, basically just increasing entropy of our newspaper
#little stars and scales that we can print over any space picture to make it more interesting
for ((n=0; n < $numOverlays; n++)); 
do
	overlay=$(ls overlay/ | sort -R | head -n 1);
	randomLocation=+$(shuf -i 0-$middleSpaceW -n 1)+$(shuf -i 0-$middleSpaceH -n 1)
	convert spaceComposite.png overlay/$overlay -gravity NorthWest -geometry $randomLocation -composite spaceComposite.png
done

#strip newlines from the input text
noLineText="$(cat /tmp/text | sed 's/--.*//g' | tr '\n' ' ')"
text=$(cat /tmp/text)

#declare some layout
#associative arrays that hold info one where to put all the elements
#text on left, right, or top images horizontal or verticle etc
declare -A layoutOne
layoutOne[textSize]=520x860
layoutOne[textLocation]=+500+200
layoutOne[spaceLayout]=+$((480 - $spaceW))+200
layoutOne[lalaLayoutWidth]=+$((480 - $spaceW))
layoutOne[lalaLayoutHeight]=+$((220 + $spaceH))
layoutOne[lalaScale]=$spaceW
layoutOne[numLala]=1

declare -A layoutTwo
layoutTwo[textSize]=1100x900
layoutTwo[textLocation]=+40+600
layoutTwo[spaceLayout]=+40+$((600 - $spaceH))
layoutTwo[lalaLayoutWidth]=+$((60 + $spaceW))
layoutTwo[lalaLayoutHeight]=+$((600 - $spaceH))
layoutTwo[lalaScale]=x$spaceH
layoutTwo[numLala]=2

declare -A layoutThree
layoutThree[textSize]=520x860
layoutThree[textLocation]=+40+200
layoutThree[spaceLayout]=+570+200
layoutThree[lalaLayoutWidth]=+570 
layoutThree[lalaLayoutHeight]=+$((220 + $spaceH))
layoutThree[lalaScale]=x$spaceH
layoutThree[numLala]=2

sel=$(shuf -i 0-2 -n 1)        

#randomly select a layout to use 
case $sel in
	0)
		declare -n layout=layoutOne
	;;
	1)
		declare -n layout=layoutTwo
	;;
	2)
		declare -n layout=layoutThree
	;;
	*)
		declare -n layout=layoutOne
	;;
esac



#randomly select a background texture for the paper
background=$(ls background/ | sort -R | head -n 1);

#randomly select a header image
title=$(ls title/ | sort -R | head -n 1);

#generate font based on title
case $title in
	5.png)
		font="KAWAIITEGAKIMOJI"
	;;
	6.png|4.png|3.png)
		font="arial"
	;;
	*)
		font="/usr/share/fonts/TTF/times.ttf"
	;;
esac


#imagemagick pipeline
convert background/$background  title/$title -background transparent -fill black -gravity NorthWest -compose LinearBurn -geometry +10+10 -composite $1 
convert -background transparent -fill black -font $font  -pointsize 23   -size ${layout[textSize]} caption:"$token$noLineText" miff:- |\
convert $1 - -gravity NorthWest -compose LinearBurn -geometry ${layout[textLocation]} -composite miff:- |\
convert - spaceComposite.png -gravity NorthWest -compose LinearBurn -rotate $(( $(shuf -i 1-3 -n 1)  - 2)) -geometry ${layout[spaceLayout]} -composite $1 

lalaStart=0

lala=$(ls lala/ | sort -R | head -n 1);
rm scaledLala.png
convert lala/$lala -scale ${layout[lalaScale]} scaledLala.png
convert $1 scaledLala.png	 -gravity NorthWest -compose LinearBurn -rotate $(( $(shuf -i 1-3 -n 1)  - 2)) -geometry +${layout[lalaLayoutWidth]}+${layout[lalaLayoutHeight]} -composite $1 


