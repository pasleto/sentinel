#include "CryptoHandler.h"

#define RSA_KEY_SIZE 2048
#define RSA_EXPONENT 65537
#define AES_BUFF_SIZE 16

// MBEDTLS_CIPHER_PADDING_ZEROS // ?
// MBEDTLS_CIPHER_PADDING_PKCS7 // ?

mbedtls_pk_context pk;
mbedtls_entropy_context entropy;
mbedtls_ctr_drbg_context ctr_drbg;
// mbedtls_mpi N, P, Q, D, E, DP, DQ, QP;
mbedtls_aes_context aes;

CryptoHandler* cryptoHandler = new CryptoHandler();

void CryptoHandler::_printError(const char* caller, int errnum) {
  char error_buf[200];
  mbedtls_strerror(-errnum, error_buf, sizeof(error_buf));
  Serial.printf("[CryptoHandler] %s - failed: -0x%04x - %s\n\n", caller, (unsigned int) -errnum, error_buf);
}

void CryptoHandler::_rsa_cleanup() {
  mbedtls_pk_free(&pk);
  mbedtls_ctr_drbg_free(&ctr_drbg);
  mbedtls_entropy_free(&entropy);
}

RsaGenerateResponse CryptoHandler::rsa_keypair_generate() {
  int ret = 0;
  mbedtls_ctr_drbg_init(&ctr_drbg);
  mbedtls_entropy_init(&entropy);
  mbedtls_pk_init(&pk);
  // mbedtls_mpi_init(&N);
  // mbedtls_mpi_init(&P);
  // mbedtls_mpi_init(&Q);
  // mbedtls_mpi_init(&D);
  // mbedtls_mpi_init(&E);
  // mbedtls_mpi_init(&DP);
  // mbedtls_mpi_init(&DQ);
  // mbedtls_mpi_init(&QP);
  RsaGenerateResponse res;
  res.success = false;
  memset(res.pubKey, 0, sizeof(res.pubKey));
  memset(res.privKey, 0, sizeof(res.privKey));
  if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, NULL, 0)) != 0) {
    _printError("generate - mbedtls_ctr_drbg_seed", ret);
    _rsa_cleanup();
    return res;
  }
  if ((ret = mbedtls_pk_setup(&pk, mbedtls_pk_info_from_type(MBEDTLS_PK_RSA))) != 0) {
    _printError("generate - mbedtls_pk_setup", ret);
    _rsa_cleanup();
    return res;
  }
  if ((ret = mbedtls_rsa_gen_key(mbedtls_pk_rsa(pk), mbedtls_ctr_drbg_random, &ctr_drbg, RSA_KEY_SIZE, RSA_EXPONENT)) != 0) {
    _printError("generate - mbedtls_rsa_gen_key", ret);
    _rsa_cleanup();
    return res;
  }
  // mbedtls_rsa_context *rsa = mbedtls_pk_rsa(pk);
  // if ((ret = mbedtls_rsa_export(rsa, &N, &P, &Q, &D, &E)) != 0 || (ret = mbedtls_rsa_export_crt(rsa, &DP, &DQ, &QP)) != 0) {
  //   _printError("generate - mbedtls_rsa_export", ret);
  //   _rsa_cleanup();
  // }
  // mbedtls_mpi_write_file("N:  ", &N,  16, NULL);
  // mbedtls_mpi_write_file("E:  ", &E,  16, NULL);
  // mbedtls_mpi_write_file("D:  ", &D,  16, NULL);
  // mbedtls_mpi_write_file("P:  ", &P,  16, NULL);
  // mbedtls_mpi_write_file("Q:  ", &Q,  16, NULL);
  // mbedtls_mpi_write_file("DP: ", &DP, 16, NULL);
  // mbedtls_mpi_write_file("DQ: ", &DQ, 16, NULL);
  // mbedtls_mpi_write_file("QP: ", &QP, 16, NULL);
  if ((ret = mbedtls_rsa_check_pubkey(mbedtls_pk_rsa(pk))) != 0) {
    _printError("generate - mbedtls_rsa_check_pubkey", ret);
    _rsa_cleanup();
    return res;
  }
  if ((ret = mbedtls_rsa_check_privkey(mbedtls_pk_rsa(pk))) != 0) {
    _printError("generate - mbedtls_rsa_check_privkey", ret);
    _rsa_cleanup();
    return res;
  }
  // if ((mbedtls_rsa_check_pub_priv(mbedtls_pk_rsa(pk), mbedtls_pk_rsa(pk))) != 0) { // ?
  //   _printError("generate - mbedtls_rsa_check_pub_priv", ret);
  //   _rsa_cleanup();
  //   return res;
  // }
  if ((ret = mbedtls_pk_write_pubkey_pem(&pk, res.pubKey, sizeof(res.pubKey))) != 0) {
    _printError("generate - mbedtls_pk_write_pubkey_pem", ret);
    _rsa_cleanup();
    return res;
  }
  if ((ret = mbedtls_pk_write_key_pem(&pk, res.privKey, sizeof(res.privKey))) != 0) {
    _printError("generate - mbedtls_pk_write_key_pem", ret);
    _rsa_cleanup();
    return res;
  }
  res.success = true;
  _rsa_cleanup();
  return res;
}

// void CryptoHandler::_rsa_generate_cleanup() {
//   // mbedtls_mpi_free(&N);
//   // mbedtls_mpi_free(&P);
//   // mbedtls_mpi_free(&Q);
//   // mbedtls_mpi_free(&D);
//   // mbedtls_mpi_free(&E);
//   // mbedtls_mpi_free(&DP);
//   // mbedtls_mpi_free(&DQ);
//   // mbedtls_mpi_free(&QP);
//   _rsa_cleanup();
// }

RsaResponse CryptoHandler::rsa_pk_encrypt(const char* publicKey, const char* data, size_t dataLen) {
  int ret = 0;
  mbedtls_ctr_drbg_init(&ctr_drbg);
  mbedtls_entropy_init(&entropy);
  mbedtls_pk_init(&pk);
  RsaResponse res;
  res.success = false;
  memset(res.data, 0, sizeof(res.data));
  if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, NULL, 0)) != 0) {
    _printError("encrypt - mbedtls_ctr_drbg_seed", ret);
    _rsa_cleanup();
    return res;
  }
  if ((ret = mbedtls_pk_parse_public_key(&pk, (const unsigned char*)publicKey, strlen(publicKey) + 1)) != 0) {
    _printError("encrypt - mbedtls_pk_parse_public_key", ret);
    _rsa_cleanup();
    return res;
  }
  if ((ret = mbedtls_pk_encrypt(&pk, (const unsigned char*)data, dataLen, res.data, &res.len, sizeof(res.data), mbedtls_ctr_drbg_random, &ctr_drbg)) != 0) {
    _printError("encrypt - mbedtls_pk_encrypt", ret);
    _rsa_cleanup();
    return res;
  }
  res.success = true;
  _rsa_cleanup();
  return res;
}

RsaResponse CryptoHandler::rsa_pk_decrypt(const char* privateKey, const char* data, size_t dataLen) {
  int ret = 0;
  mbedtls_ctr_drbg_init(&ctr_drbg);
  mbedtls_entropy_init(&entropy);
  mbedtls_pk_init(&pk);
  RsaResponse res;
  res.success = false;
  memset(res.data, 0, sizeof(res.data));
  if ((ret = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, NULL, 0)) != 0) {
    _printError("decrypt - mbedtls_ctr_drbg_seed", ret);
    _rsa_cleanup();
    return res;
  }
  if ((ret = mbedtls_pk_parse_key(&pk, (const unsigned char*)privateKey, strlen(privateKey) + 1, NULL, 0)) != 0) {
    _printError("decrypt - mbedtls_pk_parse_key", ret);
    _rsa_cleanup();
    return res;
  }
  if ((ret = mbedtls_pk_decrypt(&pk, (const unsigned char*)data, dataLen, res.data, &res.len, sizeof(res.data), mbedtls_ctr_drbg_random, &ctr_drbg)) != 0) {
    _printError("decrypt - mbedtls_pk_decrypt", ret);
    _rsa_cleanup();
    return res;
  }
  res.success = true;
  _rsa_cleanup();
  return res;
}

String CryptoHandler::aes_ecb_decrypt(unsigned char* cipherText, size_t cipherLen, const char* key, size_t keyLen) {
  String buffer = "";
  String decipheredTextString = "";
  for (int i = 0; i < (cipherLen / AES_BUFF_SIZE); i++) {
    for (int j = (i * AES_BUFF_SIZE); j < ((i + 1) * AES_BUFF_SIZE); j++) {
      buffer += (char)cipherText[j];
    }
    decipheredTextString += _aes_ecb_decrypt_chunk(buffer, key, keyLen);
    buffer = "";
  }
  return decipheredTextString;
}

String CryptoHandler::_aes_ecb_decrypt_chunk(String cipherText, const char* key, size_t keyLen) {
  String decipheredTextString = "";
  unsigned char cipherTextOutput[16];
  unsigned char decipheredTextOutput[16];
  for (int i = 0; i < 16; i++) {
    cipherTextOutput[i] = (char)cipherText[i];
  }
  _aes_ecb_decrypt(cipherTextOutput, key, keyLen, decipheredTextOutput);
  for (int i = 0; i < 16; i++) {
    if ((char)decipheredTextOutput[i] == '\0') {
      break;
    }
    decipheredTextString += (char)decipheredTextOutput[i]; // decipheredTextString = decipheredTextString + (char)decipheredTextOutput[i];
  }
  return decipheredTextString;
}

void CryptoHandler::_aes_ecb_decrypt(unsigned char* cipherText, const char* key, size_t keyLen, unsigned char* outputBuffer) {
  mbedtls_aes_init(&aes);
  mbedtls_aes_setkey_dec(&aes, (const unsigned char*) key, keyLen * 8);
  mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_DECRYPT, (const unsigned char*) cipherText, outputBuffer);
  mbedtls_aes_free(&aes);
}

String CryptoHandler::aes_cbc_decrypt(const char* key, size_t keyLen, const char* iv, int length, unsigned char* chipherText) {
  String decipheredTextString = "";
  unsigned char dec_buf[length];
  _aes_cbc_decrypt(key, keyLen, iv, length, chipherText, dec_buf);
  for (int i = 0; i < length; i++) {
    if ((char)dec_buf[i] == '\0') { // ?
      break;
    }
    decipheredTextString += (char)dec_buf[i]; // decipheredTextString = decipheredTextString + (char)dec_buf[i];
  }
  return decipheredTextString;
}

void CryptoHandler::_aes_cbc_decrypt(const char* key, size_t keyLen, const char* iv, int length, unsigned char* chipherText, unsigned char* outputBuffer) {
  mbedtls_aes_init(&aes);
  mbedtls_aes_setkey_dec(&aes, (const unsigned char*)key, keyLen * 8);
  mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_DECRYPT, length, (unsigned char*)iv, (const unsigned char*)chipherText, outputBuffer);
  mbedtls_aes_free(&aes);
}





// String CryptoHandler::aes_ecb_encrypt_string(String plainText, const char* key, size_t keyLen) { // ?
//   String buffer = "";
//   String cipherTextString = "";
//   int index = (plainText.length() / AES_BUFF_SIZE);
//   for (int i = 0; i < (plainText.length() / AES_BUFF_SIZE); i++) {
//     for (int j = (i * AES_BUFF_SIZE); j < ((i + 1) * AES_BUFF_SIZE); j++) {
//       buffer += plainText[j];
//     }
//     cipherTextString += _aes_ecb_encrypt_chunk(const_cast<char*>(buffer.c_str()), key, keyLen);
//     buffer = "";
//   }
//   buffer="";
//   if ((plainText.length() % AES_BUFF_SIZE) > 0) {
//     for (int bytes_read = (index * AES_BUFF_SIZE); bytes_read <= ((index * AES_BUFF_SIZE) + plainText.length()%AES_BUFF_SIZE); bytes_read++) {
//       buffer += plainText[bytes_read];
//     }
//     ////ADDITIONAL CODE////
//     // int no_to_pad = 16 - buffer.length();
//     // while (buffer.length() < 16) {
//     //   char padding = no_to_pad;
//     //   buffer += padding;
//     // }
//     ////////////////////////
//     cipherTextString += _aes_ecb_encrypt_chunk(const_cast<char*>(buffer.c_str()), key, keyLen);
//   }
//   return cipherTextString;
// }

// String CryptoHandler::_aes_ecb_encrypt_chunk(char* plainText, const char* key, size_t keyLen) { // ?
//   String cipherTextString = "";
//   unsigned char cipherTextOutput[16];
//   _aes_ecb_encrypt(plainText, key, keyLen, cipherTextOutput);
//   for (int i = 0; i < 16; i++) {
//     cipherTextString += (char)cipherTextOutput[i]; // cipherTextString = cipherTextString + (char)cipherTextOutput[i];
//   }
//   return cipherTextString;
// }

// void CryptoHandler::_aes_ecb_encrypt(char* plainText, const char* key, size_t keyLen, unsigned char* outputBuffer) { // ?
//   mbedtls_aes_init(&aes);
//   mbedtls_aes_setkey_enc(&aes, (const unsigned char*) key, keyLen * 8);
//   mbedtls_aes_crypt_ecb(&aes, MBEDTLS_AES_ENCRYPT, (const unsigned char*) plainText, outputBuffer);
//   mbedtls_aes_free(&aes);
// }


// void CryptoHandler::aes_cbc_encrypt(const char* key, size_t keyLen const char* iv, int length, unsigned char* plainText, unsigned char* outputBuffer) {
//   mbedtls_aes_init(&aes);
//   mbedtls_aes_setkey_enc(&aes, (const unsigned char*)key, keyLen * 8);
//   mbedtls_aes_crypt_cbc(&aes, MBEDTLS_AES_ENCRYPT, length, (unsigned char*)iv, (const unsigned char*)plainText, outputBuffer);
//   mbedtls_aes_free(&aes);
// }
