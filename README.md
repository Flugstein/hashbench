# hashbench
Benchmarking of different cryptographic hash functions using the [Crypto++](https://www.cryptopp.com "Crypto++ Homepage") library 

## How to run
- Change CPU powersave mode to performance with `governer.sh`  
- Optionally generate Files with `generateFiles.sh`
- Modify `main()` in `hashbench.cpp` to include the Hashes and Filenames you need
- Run hashbench
- Modify `.plt` files under `gnuplot/` and change input and output filenames
- Run `gnuplot <plotfilename>.plt`
- Look at the generated images under `gnuplot/images/`

### Crypto++ Links:
https://www.cryptopp.com/  
https://www.cryptopp.com/wiki/  
https://www.cryptopp.com/docs/ref/  

https://www.cryptopp.com/wiki/Hash_Functions  
https://www.cryptopp.com/wiki/Benchmarks  

https://www.cryptopp.com/wiki/Category:User_Guide  
https://www.cryptopp.com/wiki/User_Guide:_Header_Files  
https://www.cryptopp.com/wiki/User_Guide:_filters.h  
