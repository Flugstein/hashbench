#include <iostream>
#include <cryptopp/cryptlib.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/sha.h>
#define CRYPTOPP_ENABLE_NAMESPACE_WEAK 1
#include <cryptopp/md5.h>

int main() {
    using namespace std;
    using namespace CryptoPP;
    string message = "abcdefghijklmnopqrstuvwxyz";
    string digest;
    SHA256 hash;

    StringSource s(message, true, new HashFilter(hash, new HexEncoder(new StringSink(digest))));

    cout << digest << endl;
}