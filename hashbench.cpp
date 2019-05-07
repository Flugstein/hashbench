#include <iostream>
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

#define CPU_FREQ 2.7 * 1000 * 1000 * 1000

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
    CryptoPP::Timer timer;
    double elapsedTimeInSeconds;
    long numberOfHashes = 0;

    // warm up
    for (int i = 0; i < 1000; ++i) {
        hash.CalculateDigest(digest, inputData, inputDataSize);
    }

    timer.StartTimer();
    do {
        hash.CalculateDigest(digest, inputData, inputDataSize);
        numberOfHashes++;
        elapsedTimeInSeconds = timer.ElapsedTimeAsDouble();
    } while (elapsedTimeInSeconds < runTimeInSeconds);

    /** DEBUG
    std::string hexDigest;
    CryptoPP::ArraySource(digest, sizeof(digest), true, new CryptoPP::HexEncoder(new CryptoPP::StringSink(hexDigest)));
    std::cout << "Digest: " << hexDigest << std::endl;
    **/

    return elapsedTimeInSeconds / numberOfHashes;
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
    double hashTime[hashList.size()];

    std::string filename = "../testfiles/short_text.txt";
    
    CryptoPP::byte* fileData;
    size_t dataSizeInBytes = readFileIntoArray(filename, fileData);

    std::cout << "CPU Freq: " << CPU_FREQ / 1000 / 1000 / 1000 << " GHz" << std::endl;
    std::cout << dataSizeInBytes << " bytes (B)" << std::endl;
    std::cout << std::endl;

    for (int i = 0; i < hashList.size(); i++) {
        hashTime[i] = bench(*hashList[i], fileData, dataSizeInBytes, 3);

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
}