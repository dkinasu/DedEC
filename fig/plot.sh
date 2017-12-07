#!/usr/bin/gnuplot -persist -e 

#if [ ! -f ./R.dat ]
#then
#	echo ".dat file does not exist!"
#	../merge.sh
#fi

#echo $test
fn=ARG1.".eps"
data=ARG1.".dat"
t=ARG1." request latency under different placement" 


# fname=system("echo $fn")
# test=system("echo $t")
set terminal postscript eps enhanced color font 'Helvetica,24'
set output fn 

# set style data histogram
# set style histogram cluster gap 1

# set style fill solid
set title t

# set style fill solid border rgb "black"
# set auto x
# # set yrange [0:*]
# plot data using 2:xtic(1) title col, \
#         '' using 3:xtic(1) title col

set grid
#
#  Set axis labels.
#
set xlabel "Request ID" 
set ylabel "Lacency" 
set xtics 1

set xrange [0:*]
# set logscale y

set style line 1 lt 2 lc rgb "red" lw 3
set style line 2 lt 2 lc rgb "blue" lw 2
set style line 3 lt 2 lc rgb "yellow" lw 3
set style line 4 lt 2 lc rgb "green" lw 2

# set yrange [1e7:1e9]

# plot data using 1:3 title "Baseline" with line , \
# 	data using 1:4 title "Random" with line, \
# 	data using 1:5 title "DualAware" with line


# plot data using 1:3 title "Baseline" with linespoints ls 1, \
# 	data using 1:4 title "Random" with linespoints ls 2, \
# 	data using 1:5 title "DualAware" with linespoints ls 4

plot data using 1:3 title "Baseline" with points , \
	data using 1:4 title "Random" with points , \
	data using 1:5 title "DualAware" with points



# points pointtype 5

#
#  Terminate.
#
quit




















#set xrange [  : 2 ] noreverse nowriteback
#set yrange [ 0 : 1 ] noreverse nowriteback
#set mxtics 5.000000
#set mytics 5.000000
#set xtics border mirror norotate 1
#set ytics border mirror norotate 0.5
# plot data using 1:3:xtic(2) title "Baseline" title col ,\
#      data using 1:4:xtic(2) title "Random" title col
     # data using 1:5 title "DA" with points ps 2 pt 5 


#    EOF
