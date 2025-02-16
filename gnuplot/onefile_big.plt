set terminal pngcairo size 1024,720 font ",16"
set output 'images/onefile_big_final.png'

set style line 1 \
    linecolor rgb '#3333cc' \
    linetype 1 linewidth 2 \
    pointtype 1 pointsize 1.5
set style line 2 \
    linecolor rgb '#F30323' \
    linetype 1 linewidth 2 \
    pointtype 2 pointsize 1.5
set style line 3 \
    linecolor rgb '#ffe453' \
    linetype 1 linewidth 2 \
    pointtype 3 pointsize 1.5
set style line 4 \
    linecolor rgb '#00cc00' \
    linetype 1 linewidth 2 \
    pointtype 4 pointsize 1.5
set style line 5 \
    linecolor rgb '#ff8342' \
    linetype 1 linewidth 2 \
    pointtype 5 pointsize 1.5
set style line 6 \
    linecolor rgb '#cc0099' \
    linetype 1 linewidth 2 \
    pointtype 6 pointsize 1.5
set style line 7 \
    linecolor rgb '#cc0000' \
    linetype 1 linewidth 2 \
    pointtype 7 pointsize 1.5
set style line 8 \
    linecolor rgb '#33bccc' \
    linetype 1 linewidth 2 \
    pointtype 8 pointsize 1.5
set style line 9 \
    linecolor rgb '#009933' \
    linetype 1 linewidth 2 \
    pointtype 9 pointsize 1.5

set title "Hashes per Second over different File Sizes"

set ylabel "Hashes per Second"
set logscale y 10
set format y "10^{%T}"
set grid ytics linestyle 0

set xlabel "File Size in Bytes"
set logscale x 2
set xtics 4
set format x "2^{%L}"


plot 'datafiles/onefile_final.dat' index 0 title 'AES Davies-Meyer' with linespoints linestyle 1, \
     ''                      index 1 title 'MD5' with linespoints linestyle 2, \
     ''                      index 2 title 'SHA-1' with linespoints linestyle 3, \
     ''                      index 3 title 'SHA-256' with linespoints linestyle 4, \
     ''                      index 4 title 'SHA-512' with linespoints linestyle 5, \
     ''                      index 5 title 'SHA3-256' with linespoints linestyle 6, \
     ''                      index 6 title 'SHA3-512' with linespoints linestyle 7, \
     ''                      index 7 title 'BLAKE2b-512' with linespoints linestyle 8, \
     ''                      index 8 title 'BLAKE2s-256' with linespoints linestyle 9