set terminal pngcairo size 1024,720 font ",16"
set output 'images/cpb_final.png'

set title "Cycles per Byte"
set nokey
set yrange [0:33]
set ylabel "Cycles per Byte"
set xtics rotate by -45

set boxwidth 0.5
set grid ytics linestyle 0
set style fill solid
plot "datafiles/cpb_final.dat" using 0:2:3:xtic(1) with boxes lc rgb var ,\
     "datafiles/cpb_final.dat" using 0:($2+1):2 with labels