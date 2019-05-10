set title "Hashes per Second for 16 Byte Password"
set nokey
set ylabel "Hashes per Second"
set xtics rotate by -45

set boxwidth 0.5
set grid ytics linestyle 0
set style fill solid
plot "datafiles/password_final.dat" using 0:2:3:xtic(1) with boxes lc rgb var