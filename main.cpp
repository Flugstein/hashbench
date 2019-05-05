#include <iostream>
#include <cryptopp/cryptlib.h>
#include <cryptopp/filters.h>
#include <cryptopp/files.h>
#include <cryptopp/hrtimer.h>
#include <cryptopp/hex.h>
#include <cryptopp/sha.h>
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <cryptopp/md5.h>

using namespace std;
using namespace CryptoPP;

/**
 * Read file into byte array
 * @param filename
 * @param data Array to read into
 * @return Size of data array
 */
size_t readFile(string filename, CryptoPP::byte* &data) {
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
 * @param inputFileName Name of file getting hashed
 * @return Time spent for hashing data
 */
double bench(HashTransformation& hash, string inputFileName) {
    CryptoPP::byte* data;
    size_t sizeOfData = readFile(inputFileName, data);

    cout << "Bytes: " << sizeOfData << endl;

    CryptoPP::byte digest[hash.DigestSize()];

    Timer timer(TimerBase::MILLISECONDS);
    double elapsedTimeInMillis;

    timer.StartTimer();
    hash.CalculateDigest(digest, data, sizeOfData);
    elapsedTimeInMillis = timer.ElapsedTimeAsDouble();

    string hexDigest;
    ArraySource(digest, sizeof(digest), true, new HexEncoder(new StringSink(hexDigest)));
    cout << "Digest: " << hexDigest << endl;

    delete[] data;

    return elapsedTimeInMillis;
}

int main() {
    string testFile = "../testfiles/short_text.txt";
    SHA256 hash;
    double timeNeeded = bench(hash, testFile);
    cout << "Time for " << hash.AlgorithmName() << ": " << timeNeeded << "ms" << endl;
}