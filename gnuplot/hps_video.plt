set terminal pngcairo size 1024,720 font ",16"
set output 'images/hps_video_final.png'

set title "Hashes per Second for 2GB Video File"
set nokey
set ylabel "Hashes per Second"
set xtics rotate by -45

set boxwidth 0.5
set grid ytics linestyle 0
set style fill solid
plot "datafiles/video_final.dat" using 0:2:3:xtic(1) with boxes lc rgb var