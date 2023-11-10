#ifndef CRYPTOHANDLER_H
#define CRYPTOHANDLER_H

#include <Arduino.h>
#include "mbedtls/platform.h"
#include "mbedtls/error.h"
#include "mbedtls/pk.h"
#include "mbedtls/rsa.h"
#include "mbedtls/entropy.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/aes.h"

struct RsaGenerateResponse {
  bool success;
  unsigned char pubKey[1024];
  unsigned char privKey[2048];
};

struct RsaResponse {
  bool success;
  unsigned char data[256];
  size_t len;
};

class CryptoHandler{
  public:
    RsaGenerateResponse rsa_keypair_generate();
    RsaResponse rsa_pk_encrypt(const char* publicKey, const char* data, size_t dataLen);
    RsaResponse rsa_pk_decrypt(const char* privateKey, const char* data, size_t dataLen);
    String aes_ecb_decrypt(unsigned char* cipherText, size_t cipherLen, const char* key, size_t keyLen);
    String aes_cbc_decrypt(const char* key, size_t keyLen, const char* iv, int length, unsigned char* chipherText);
    // String aes_ecb_encrypt_string(String plainText, const char* key, size_t keyLen); // ?
    // void aes_cbc_encrypt(const char* key, const char* iv, int length, unsigned char* plainText, unsigned char* outputBuffer); // ?
  private:
    // void _generateCleanup();
    void _printError(const char* caller, int errnum);
    void _rsa_cleanup();
    String _aes_ecb_decrypt_chunk(String cipherText, const char* key, size_t keyLen);
    void _aes_ecb_decrypt(unsigned char* cipherText, const char* key, size_t keyLen, unsigned char* outputBuffer);
    void _aes_cbc_decrypt(const char* key, size_t keyLen, const char* iv, int length, unsigned char* chipherText, unsigned char* outputBuffer);
    // String _aes_ecb_encrypt_chunk(char* plainText, const char* key, size_t keyLen); // ?
    // void _aes_ecb_encrypt(char* plainText, const char* key, size_t keyLen, unsigned char* outputBuffer); // ?
    // void _aes_cbc_decrypt(const char* key, size_t keyLen, const char* iv, int length, unsigned char* plainText, unsigned char* outputBuffer); // ?
};

extern CryptoHandler* cryptoHandler;

#endif
