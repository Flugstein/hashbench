#include <iostream>
#include <fstream>

#include <cryptopp/cryptlib.h>
#include <cryptopp/filters.h>
#include <cryptopp/files.h>
#include <cryptopp/hrtimer.h>
#include <cryptopp/hex.h>
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <cryptopp/md4.h>
#include <cryptopp/md5.h>
#include <cryptopp/sha.h>
#include <cryptopp/sha3.h>
#include <cryptopp/blake2.h>

/**
 * Read file into byte array
 * @param filename
 * @param data Array to read into
 * @return Size of data array
 */
size_t readFileIntoArray(std::string filename, CryptoPP::byte* &data) {
    FILE* f = fopen(filename.c_str(), "r");

    fseek(f, 0, SEEK_END);
    size_t size = ftell(f);

    data = new CryptoPP::byte[size];

    rewind(f);
    fread(data, sizeof(CryptoPP::byte), size, f);

    fclose(f);

    return size;
}

/**
 * Measure speed of hash functions
 * @param hash Hash function to apply
 * @param data Pointer to input data array
 * @param runTimeInSeconds Approximate runtime of function (defines accuracy of benchmark)
 * @return Time for one hash in seconds
 */
double bench(CryptoPP::HashTransformation& hash, CryptoPP::byte* inputData, size_t inputDataSize, double runTimeInSeconds) {
    CryptoPP::byte digest[hash.DigestSize()];

    double totalTime = 0;
    double currentHashTime = 0;
    double bestHashTime = std::numeric_limits<double>::max();
    const double minimumHashTime = (double) 100 / 1000 / 1000; // 100 microseconds

    do {
        CryptoPP::Timer timer;
        timer.StartTimer();

        hash.CalculateDigest(digest, inputData, inputDataSize);

        currentHashTime = timer.ElapsedTimeAsDouble();

        // calculate average of more hashes if time for 1 hash is too short for accurate measurement
        if (currentHashTime < minimumHashTime) {
            long numberOfHashes = 0;
            double moreHashesTime = 0;
            while (moreHashesTime < minimumHashTime * 10) {
                hash.CalculateDigest(digest, inputData, inputDataSize);
                numberOfHashes++;
                moreHashesTime = timer.ElapsedTimeAsDouble();
            }
            currentHashTime = moreHashesTime / numberOfHashes;
            totalTime += moreHashesTime;
        } else {
            totalTime += currentHashTime;
        }

        if (currentHashTime < bestHashTime)
            bestHashTime = currentHashTime;

    } while (totalTime < runTimeInSeconds);

    /** DEBUG
    std::string hexDigest;
    CryptoPP::ArraySource(digest, sizeof(digest), true, new CryptoPP::HexEncoder(new CryptoPP::StringSink(hexDigest)));
    std::cout << "Digest: " << hexDigest << std::endl;
    **/

    return bestHashTime;
}

/**
 * Generate data files for plotting with gnuplot
 * @param hashList List of hash functions to plot
 * @param inputFileName Filename of data to be hashed
 */
void generateGnuplotDataFile(std::vector<CryptoPP::HashTransformation *> &hashList, std::string &inputFileName) {
    CryptoPP::byte* fileData;
    size_t dataSizeInBytes = readFileIntoArray(inputFileName, fileData);

    std::cout << dataSizeInBytes << " bytes (B)" << std::endl;

    std::string helperString = inputFileName.substr(inputFileName.find_last_of("\\/"), std::string::npos);
    helperString = helperString.substr(0, helperString.find_last_of('.')) + ".dat";
    std::string outputFileName = "../gnuplot/datafiles" + helperString;
    std::cout << "writing to " << outputFileName << std::endl;

    std::ofstream outputFile;
    outputFile.open(outputFileName);

    for (int i = 0; i < hashList.size(); i++) {
        double hashesPerSecond = 1 / bench(*hashList[i], fileData, dataSizeInBytes, 2);

        outputFile << i << " \"" << hashList[i]->AlgorithmName() << "\" " << hashesPerSecond << std::endl;
    }
    outputFile.close();
}

int main() {
    std::vector<CryptoPP::HashTransformation*> hashList = {
        new CryptoPP::Weak::MD4(),
        new CryptoPP::Weak::MD5(),
        new CryptoPP::SHA1(),
        new CryptoPP::SHA256(),
        new CryptoPP::SHA512(),
        new CryptoPP::SHA3_256(),
        new CryptoPP::SHA3_512(),
        new CryptoPP::BLAKE2b(),
        new CryptoPP::BLAKE2s()
    };

    std::string filename = "../testfiles/short_text.txt";
    generateGnuplotDataFile(hashList, filename);


    /**
    double hashTime[hashList.size()];

    CryptoPP::byte* fileData;
    size_t dataSizeInBytes = readFileIntoArray(filename, fileData);

    std::cout << dataSizeInBytes << " bytes (B)" << std::endl;
    std::cout << std::endl;

    for (int i = 0; i < hashList.size(); i++) {
        hashTime[i] = bench(*hashList[i], fileData, dataSizeInBytes, 5);

        std::cout << hashList[i]->AlgorithmName() << ":" << std::endl;
        std::cout << hashTime[i] * 1000 * 1000 << " µs per hash" << std::endl;
        std::cout <<  1 / hashTime[i] << " hashes per s" << std::endl;
        std::cout <<  ((double) dataSizeInBytes / 1024 / 1024) / hashTime[i] << " MiB/s" << std::endl;
        std::cout << std::endl;
    }

    delete[] fileData;
    for (int i = 0; i < hashList.size(); i++) {
        delete hashList[i];
    }
    **/
}