#!/bin/bash
#echo "Ploting"
var=$(ls *.res 2> /dev/null)

for file in ${var[@]}
do
	if [ -e $file ]
	then
		# echo $file
		mv $file ./fig/
	fi
done

cd ./fig/
pwd


var=$(ls *.dat 2> /dev/null)
for file in ${var[@]}
do
	if [ -f $file ]
	then
		# echo $file
		mv *$file ~/.local/share/Trash/files
	fi
done


if [ ! -d data ]
then
	echo "File data does not exist! Making it!"
	mkdir data
fi


# array=(Read)
array=(Read Write)
# array=(Write)
part=".res"

for i in ${array[@]}
do
	out=$i".dat"

	for j in {0..2}
	do
		name=$i$j$part
		tmp=$i"."$j".tmp"
		# echo $name

		if [ -e $name ]
		then
			if [ "$j" -eq 0 ]
			then 
				# awk '{print $2" "$3}' $name
				awk '{print $1" "$2" "$3}' $name > "$tmp"
				# cat $tmp
			elif [ -e $name ]
			then
				# awk '{print $3}' $name
				awk '{print $3}' $name > "$tmp"
			fi
			mv $name ./data
		fi 		
		# cat $i.dat		
	done
	
	res=$(ls *.tmp 2> /dev/null | wc -l)

	if [ $res -ne 0 ] 
	then
		paste -d " " *.tmp > $out
		gnuplot -c plot.sh $i
		mv $out ./data
	fi

	xdg-open $i".eps"

done


var=$(ls *.tmp 2> /dev/null)

for file in ${var[@]}
do
	if [ -f $file ]
	then
		echo $file
		mv *$file ~/.local/share/Trash/files
	fi
done


#rm *.tmp