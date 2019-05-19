#include <iostream>
#include <iomanip>
#include <fstream>

#include <cryptopp/cryptlib.h>
#include <cryptopp/filters.h>
#include <cryptopp/files.h>
#include <cryptopp/hrtimer.h>
#include <cryptopp/hex.h>
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <cryptopp/aes.h>
#include <cryptopp/md5.h>
#include <cryptopp/sha.h>
#include <cryptopp/sha3.h>
#include <cryptopp/blake2.h>
#include <cryptopp/modes.h>
#include <cryptopp/osrng.h>

const double cpuFreq = 2.7 * 1000 * 1000 * 1000;
const unsigned int max_aes_file_size_bytes = 1024 * 1024 * 2;

/** Hack to use AES as hash function **/
class AES_HASH: public CryptoPP::HashTransformation {
private:
    CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption* encryptor;

public:
    AES_HASH() {
        CryptoPP::AutoSeededRandomPool rnd;

        CryptoPP::SecByteBlock key(0x00, CryptoPP::AES::DEFAULT_KEYLENGTH);
        rnd.GenerateBlock(key, key.size());

        CryptoPP::SecByteBlock iv(CryptoPP::AES::BLOCKSIZE);
        rnd.GenerateBlock(iv, iv.size());

        encryptor = new CryptoPP::CFB_Mode<CryptoPP::AES>::Encryption(key, key.size(), iv);
    }

    std::string AlgorithmName() const {
        return "AES Davies-Meyer";
    }

    unsigned int DigestSize() const {
        return max_aes_file_size_bytes;
    }

    void CalculateDigest(CryptoPP::byte *digest, const CryptoPP::byte *input, size_t length) {
        encryptor->ProcessData(digest, input, length);
    }

    void Update(const CryptoPP::byte *input, size_t length) {}

    void TruncatedFinal(CryptoPP::byte *digest, size_t digestSize) {}
};

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
 * @param colorsPerHash Color for each hash function (must be same size as hashList)
 */
void generateGnuplotDataFile(std::vector<CryptoPP::HashTransformation *> &hashList, std::vector<std::string> inputFileNames, std::vector<std::string> colorsPerHash) {
    double timeForOneHash[inputFileNames.size()][hashList.size()];
    size_t dataSizeInBytes[inputFileNames.size()];

    for (int i = 0; i < inputFileNames.size(); i++) {
        CryptoPP::byte *fileData;
        dataSizeInBytes[i] = readFileIntoArray(inputFileNames[i], fileData);

        std::cout << "reading " << dataSizeInBytes[i] << " bytes from " << inputFileNames[i] << std::endl;

        for (int j = 0; j < hashList.size(); j++) {
            double hashTimeInSeconds = bench(*hashList[j], fileData, dataSizeInBytes[i], 1);
            timeForOneHash[i][j] = hashTimeInSeconds;
        }
        delete[] fileData;
    }

    // write hashes per second, all in one file
    std::ofstream oneFile;
    std::string oneFileName = "../gnuplot/datafiles/onefile.dat";
    oneFile.open(oneFileName);
    std::cout << "writing to " << oneFileName << std::endl;

    for (int i = 0; i < hashList.size(); i++) {
        oneFile << "# " << hashList[i]->AlgorithmName() << " (index " << i << ")" << std::endl;
        oneFile << "# bytes hashesPerSecond" << std::endl;
        for (int j = 0; j < inputFileNames.size(); j++) {
            oneFile << dataSizeInBytes[j] << " " << 1 / timeForOneHash[j][i] << std::endl;
        }
        oneFile << std::endl << std::endl;
    }
    oneFile.close();

    // write hashes per second, bar chart files
    for (int i = 0; i < inputFileNames.size(); i++) {
        std::string hpsFileName;
        std::string helperString = inputFileNames[i].substr(inputFileNames[i].find_last_of("\\/"), std::string::npos);
        helperString = helperString.substr(0, helperString.find_last_of('.'));
        hpsFileName = "../gnuplot/datafiles" + helperString + "_hps.dat";
        std::cout << "writing to " << hpsFileName << std::endl;

        std::ofstream hpsFile;
        hpsFile.open(hpsFileName);
        hpsFile << "# name hashesPerSecond color" << std::endl;

        for (int j = 0; j < hashList.size(); j++) {
            hpsFile << "\"" << hashList[j]->AlgorithmName() << "\" " << 1 / timeForOneHash[i][j] << " " << colorsPerHash[j] << std::endl;
        }
        hpsFile.close();
    }

    // write cycles per byte, bar chart files
    for (int i = 0; i < inputFileNames.size(); i++) {
        std::string cpbFileName;
        std::string helperString = inputFileNames[i].substr(inputFileNames[i].find_last_of("\\/"), std::string::npos);
        helperString = helperString.substr(0, helperString.find_last_of('.'));
        cpbFileName = "../gnuplot/datafiles" + helperString + "_cpb.dat";
        std::cout << "writing to " << cpbFileName << std::endl;

        std::ofstream cpbFile;
        cpbFile.open(cpbFileName);
        cpbFile << "# name cyclesPerByte color" << std::endl;

        for (int j = 0; j < hashList.size(); j++) {
            double cpb = timeForOneHash[i][j] * cpuFreq / dataSizeInBytes[i];
            cpbFile << std::fixed << std::setprecision(2);
            cpbFile << "\"" << hashList[j]->AlgorithmName() << "\" " << cpb << " " << colorsPerHash[j] << std::endl;
        }
        cpbFile.close();
    }

}

int main() {
    std::vector<CryptoPP::HashTransformation*> hashList = {
        new AES_HASH(),
        new CryptoPP::Weak::MD5(),
        new CryptoPP::SHA1(),
        new CryptoPP::SHA256(),
        new CryptoPP::SHA512(),
        new CryptoPP::SHA3_256(),
        new CryptoPP::SHA3_512(),
        new CryptoPP::BLAKE2b(),
        new CryptoPP::BLAKE2s()
    };

    std::vector<std::string> colorsPerHash = {
        "0x3333cc",
        "0xf30323",
        "0xffe453",
        "0x00cc00",
        "0xff8342",
        "0xcc0099",
        "0xcc0000",
        "0x33bccc",
        "0x009933"
    };

    std::vector<std::string> fileNames;

    fileNames.push_back("../testfiles/short_text.txt");

    generateGnuplotDataFile(hashList, fileNames, colorsPerHash);
}
