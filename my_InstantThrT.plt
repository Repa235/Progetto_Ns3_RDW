reset
clear

set xrange [0:30]
set xtics 3,3
#set term png size 800,500
#set ytics 6000, 0.1.
set autoscale
set samples 20
 
set grid
set terminal jpeg font "Arial, 18"
set key top left Left reverse font "Arial, 14"
#set key outside
#set key at 10., 50.
set key box linewidth 1
set xlabel "Tempo [s]" 
set ylabel "Troughtput istantaneo [Mbit/s]" 
set output "Sim2.jpeg"

#set pointsize 3

plot "instant_ThroughputTCP_OnlyTCP.txt"  using 1:2 title "Solo TCP" with linespoint lt 1 lw 1 pt 1 lc rgb "black",\
"instant_ThroughputTCP.txt"  using 1:2 title "TCP con UDP" with linespoint lt 1 lw 1 pt 2 lc rgb "red"



