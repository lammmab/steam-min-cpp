#include <steamclient/crypto/helpers.hpp>

using namespace Steam::Crypto;
using namespace CryptoPP;

uint32_t Helpers::crc32_hash(const std::vector<uint8_t>& data) {
  CRC32 crc;
  crc.Update(data.data(), data.size());

  uint32_t result;
  crc.TruncatedFinal(reinterpret_cast<byte*>(&result), sizeof(result));

  return result;
}

std::vector<uint8_t> Helpers::rsa_encrypt_oaep_sha1(
    const std::vector<uint8_t>& pubKeyDer,
    const std::vector<uint8_t>& plaintext, AutoSeededRandomPool& rng) {
  RSA::PublicKey key;
  ByteQueue queue;

  queue.Put(pubKeyDer.data(), pubKeyDer.size());
  queue.MessageEnd();

  key.BERDecode(queue);

  RSAES_OAEP_SHA_Encryptor encryptor(key);

  std::vector<uint8_t> ciphertext;

  ArraySource(
      plaintext.data(), plaintext.size(), true,
      new PK_EncryptorFilter(rng, encryptor, new VectorSink(ciphertext)));

  return ciphertext;
}

std::vector<uint8_t> Helpers::symmetric_encrypt_hmac_iv(
    const std::vector<uint8_t>& plaintext,
    const std::vector<uint8_t>& session_key,
    const std::vector<uint8_t>& hmac_secret, RandomNumberGenerator& rng) {
  std::vector<uint8_t> iv(IV_LENGTH);
  generate_iv(plaintext, iv, hmac_secret, rng);

  std::vector<uint8_t> output(calculate_max_encrypted_length(plaintext.size()));
  size_t offset = 0;

  ECB_Mode<AES>::Encryption ecb;
  ecb.SetKey(session_key.data(), session_key.size());

  ArraySource(iv.data(), iv.size(), true,
              new StreamTransformationFilter(
                  ecb, new ArraySink(output.data(), IV_LENGTH),
                  StreamTransformationFilter::NO_PADDING));

  offset += IV_LENGTH;

  CBC_Mode<AES>::Encryption cbc;
  cbc.SetKeyWithIV(session_key.data(), session_key.size(), iv.data());

  std::string cipher;
  ArraySource(
      plaintext.data(), plaintext.size(), true,
      new StreamTransformationFilter(cbc, new StringSink(cipher),
                                     StreamTransformationFilter::PKCS_PADDING));

  std::memcpy(output.data() + offset, cipher.data(), cipher.size());
  output.resize(offset + cipher.size());

  return output;
}

std::vector<uint8_t> Helpers::symmetric_decrypt_hmac_iv(
    const std::vector<uint8_t>& input, const std::vector<uint8_t>& session_key,
    const std::vector<uint8_t>& hmac_secret) {
  if (input.size() < IV_LENGTH)
    throw std::runtime_error("Invalid encrypted data");

  std::vector<uint8_t> iv(IV_LENGTH);

  ECB_Mode<AES>::Decryption ecb;
  ecb.SetKey(session_key.data(), session_key.size());

  ArraySource(
      input.data(), IV_LENGTH, true,
      new StreamTransformationFilter(ecb, new ArraySink(iv.data(), iv.size()),
                                     StreamTransformationFilter::NO_PADDING));

  CBC_Mode<AES>::Decryption cbc;
  cbc.SetKeyWithIV(session_key.data(), session_key.size(), iv.data());

  std::string recovered;

  ArraySource(
      input.data() + IV_LENGTH, input.size() - IV_LENGTH, true,
      new StreamTransformationFilter(cbc, new StringSink(recovered),
                                     StreamTransformationFilter::PKCS_PADDING));

  std::vector<uint8_t> plaintext(recovered.begin(), recovered.end());

  if (!validate_iv(plaintext, iv, hmac_secret))
    throw std::runtime_error("HMAC from server did not match computed HMAC.");

  return plaintext;
}

void Helpers::generate_iv(const std::vector<uint8_t>& plaintext,
                          std::vector<uint8_t>& iv,
                          const std::vector<uint8_t>& hmac_secret,
                          CryptoPP::RandomNumberGenerator& rng) {
  const size_t hash_len = HASH_LENGTH;

  rng.GenerateBlock(iv.data() + hash_len, IV_RAND_LENGTH);

  std::vector<uint8_t> hmac_buffer(plaintext.size() + IV_RAND_LENGTH);

  std::memcpy(hmac_buffer.data(), iv.data() + hash_len, IV_RAND_LENGTH);

  std::memcpy(hmac_buffer.data() + IV_RAND_LENGTH, plaintext.data(),
              plaintext.size());

  HMAC<SHA1> hmac(hmac_secret.data(), hmac_secret.size());

  byte digest[SHA1::DIGESTSIZE];
  hmac.CalculateDigest(digest, hmac_buffer.data(), hmac_buffer.size());

  std::memcpy(iv.data(), digest, hash_len);
}

bool Helpers::validate_iv(const std::vector<uint8_t>& plaintext,
                          const std::vector<uint8_t>& iv,
                          const std::vector<uint8_t>& hmac_secret) {
  const size_t hash_len = HASH_LENGTH;

  std::vector<uint8_t> hmac_buffer(plaintext.size() + IV_RAND_LENGTH);

  std::memcpy(hmac_buffer.data(), iv.data() + hash_len, IV_RAND_LENGTH);

  std::memcpy(hmac_buffer.data() + IV_RAND_LENGTH, plaintext.data(),
              plaintext.size());

  HMAC<SHA1> hmac(hmac_secret.data(), hmac_secret.size());

  byte digest[SHA1::DIGESTSIZE];
  hmac.CalculateDigest(digest, hmac_buffer.data(), hmac_buffer.size());

  return std::memcmp(digest, iv.data(), hash_len) == 0;
}
