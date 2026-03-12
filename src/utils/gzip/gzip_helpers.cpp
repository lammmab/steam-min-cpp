#include "utils/gzip/gzip_helpers.hpp"

using namespace Steam::Utils;
using vector = std::vector<uint8_t>;
using string = std::string;

#include <cryptopp/gzip.h>
#include <cryptopp/filters.h>

vector GZip::gzip_decompress(const vector& input, size_t expected_size) {
    std::string decompressed;
    vector output;
    try {
        CryptoPP::VectorSource vs(input, true,
            new CryptoPP::Gunzip(
                new CryptoPP::VectorSink(output)
            )
        );
    } catch (const CryptoPP::Exception& e) {
        throw std::runtime_error(e.what());
    }
    return output;
}