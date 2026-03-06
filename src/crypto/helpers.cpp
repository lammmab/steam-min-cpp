#include "crypto/helpers.hpp"

using namespace Steam::Crypto;

uint32_t Helpers::crc32_hash(const std::vector<uint8_t>& data)
{
    CryptoPP::CRC32 crc;
    crc.Update(data.data(), data.size());

    uint32_t result;
    crc.TruncatedFinal(reinterpret_cast<CryptoPP::byte*>(&result), sizeof(result));

    return result;
}

std::vector<uint8_t> Helpers::rsa_encrypt_oaep_sha1(
    const std::vector<uint8_t>& pubKeyDer,
    const std::vector<uint8_t>& plaintext,
    CryptoPP::AutoSeededRandomPool& rng
)
{
    CryptoPP::RSA::PublicKey key;
    CryptoPP::ByteQueue queue;

    queue.Put(pubKeyDer.data(), pubKeyDer.size());
    queue.MessageEnd();

    key.BERDecode(queue);

    CryptoPP::RSAES_OAEP_SHA_Encryptor encryptor(key);

    std::vector<uint8_t> ciphertext;

    CryptoPP::ArraySource(
        plaintext.data(),
        plaintext.size(),
        true,
        new CryptoPP::PK_EncryptorFilter(
            rng,
            encryptor,
            new CryptoPP::VectorSink(ciphertext)
        )
    );

    return ciphertext;
}