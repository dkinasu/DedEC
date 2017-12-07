#!/bin/bash
#  running simulators

#echo "Start"
if [[ $# -lt 2 ]]
then 
	echo "Usage: 1.sh trace_num [Independent run 0|1] [policy num 0|1|2]"
	exit
else
	#filedir="/home/dkinasu/Documents/Research/Tracefile/"
        filedir="/home/dkinasu/Documents/Research/New_DedupEC/test/"
        #filedir="/home/dkinasu/Documents/Research/fordk/1/"
        #filedir="/home/dkinasu/NetBeansProjects/SSD/F_Dedup/F_Dedup/"
        output="1.txt"
        node_num=6
        max_blk_num=5000
        maxFp_num=200000
        cachesize=500000
	
	make
	echo "$2"
	if [ $2 -eq 0 ]
	then
		for i in {0..2}
		do
			echo "Policy is: $i"
			echo $(($1+1))
			~/Documents/Research/New_DedupEC/a.out $filedir $1 $(($1+1)) $output $node_num $max_blk_num $maxFp_num $cachesize $i
		done
		./merge.sh
	else
		~/Documents/Research/New_DedupEC/a.out $filedir $1 $(($1+1)) $output $node_num $max_blk_num $maxFp_num $cachesize $3 
	fi

fi
