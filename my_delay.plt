reset
clear

set xrange [1:16]
set yrange [0:0.5]
set xtics 2,2

#set ytics 6000, 0.1

 
set grid
set terminal jpeg font "Arial, 18"
set key top left Left reverse font "Arial, 14"
#set key outside
#set key at 10., 50.
set key box linewidth 1
set xlabel "Flussi WiFi" 
set ylabel "Ritardo [s]" 
set output "delay.jpeg"

set pointsize 2

plot "80211a/avg_delay.txt" using 1:2 title "a" with linespoint lt 1 lw 2.5 pt 1 lc rgb "black",\
"80211g/avg_delay.txt" using 1:2 title "g" with linespoint lt 1 lw 2.5 pt 2 lc rgb "red",\
"80211ac/avg_delay.txt" using 1:2 title "ac" with linespoint lt 1 lw 2.5 pt 4 lc rgb "#0000FF"

