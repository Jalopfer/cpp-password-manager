#include "Security/Crypto.h"
#include <sodium.h>
#include <string>
#include <vector>
#include <iostream>

//Salt is stored as a base64 format, we do this transformation here
Crypter::Crypter(const std::string& password, const std::string& salt)
{
    generateKey(password, salt);
}

Crypter::~Crypter()
{
    sodium_memzero(m_key, sizeof(m_key));
}

//True if encription is completed
bool Crypter::encrypt(const std::string& data, std::string& encryptedData, std::string& nonce)
{
    //Authenticated encryption based on sodium: https://libsodium.gitbook.io/doc/secret-key_cryptography/secretbox
    short dataLen = data.length();
    
    std::vector<unsigned char> f_encryptedData(dataLen + crypto_secretbox_MACBYTES);
    const unsigned char* f_data = (const unsigned char *)data.c_str();
    std::vector<unsigned char> f_nonce(crypto_secretbox_NONCEBYTES);

    //Operations for encrypting
    randombytes_buf(f_nonce.data(), f_nonce.size());
    crypto_secretbox_easy(f_encryptedData.data(), f_data, dataLen, f_nonce.data(), m_key);

    //Transformations for storing in base 64 format
    size_t encoded_len = sodium_base64_ENCODED_LEN(f_encryptedData.size(), sodium_base64_VARIANT_ORIGINAL);
    encryptedData = std::string(encoded_len, '\0');
    sodium_bin2base64(encryptedData.data(), encoded_len, f_encryptedData.data(), f_encryptedData.size(), sodium_base64_VARIANT_ORIGINAL);
    size_t nonce_encoded_len = sodium_base64_ENCODED_LEN(f_nonce.size(), sodium_base64_VARIANT_ORIGINAL);
    nonce = std::string(nonce_encoded_len, '\0');
    sodium_bin2base64(nonce.data(), nonce_encoded_len, f_nonce.data(), f_nonce.size(), sodium_base64_VARIANT_ORIGINAL);
    
    return true;
}

//True if decription is completed
bool Crypter::decrypt(const std::string& encryptedData, std::string& decryptedData, std::string& nonce)
{
    std::vector<unsigned char> f_encryptedData(encryptedData.size());
    size_t cipher_len;
    sodium_base642bin(f_encryptedData.data(), f_encryptedData.size(),
                      encryptedData.data(), encryptedData.size(),
                      nullptr, &cipher_len, nullptr,
                      sodium_base64_VARIANT_ORIGINAL);
    f_encryptedData.resize(cipher_len);

    // Convertir nonce de base64 a bytes
    std::vector<unsigned char> f_nonce(nonce.size());
    size_t nonce_len;
    sodium_base642bin(f_nonce.data(), f_nonce.size(), nonce.data(), nonce.size(), nullptr, &nonce_len, nullptr, sodium_base64_VARIANT_ORIGINAL);

    // Descifrar
    size_t message_len = cipher_len - crypto_secretbox_MACBYTES;
    
    std::vector<unsigned char> f_decryptedData(message_len);
    
    if (crypto_secretbox_open_easy(f_decryptedData.data(), f_encryptedData.data(), cipher_len, f_nonce.data(), m_key) != 0) 
    {
        return false; // autenticación fallida, datos corruptos o key incorrecta
    }

    decryptedData = std::string(reinterpret_cast<char*>(f_decryptedData.data()), message_len);
    return true;
}

bool Crypter::generateKey(const std::string& password, const std::string& salt)
{
    std::vector<unsigned char> f_salt(salt.size());
    size_t salt_len;
    sodium_base642bin(f_salt.data(), f_salt.size(),
                      salt.data(), salt.size(),
                      nullptr, &salt_len, nullptr,
                      sodium_base64_VARIANT_ORIGINAL);
    f_salt.resize(salt_len);

    if (crypto_pwhash (m_key, sizeof m_key, password.c_str(), password.length(), f_salt.data(),
     crypto_pwhash_OPSLIMIT_INTERACTIVE, crypto_pwhash_MEMLIMIT_INTERACTIVE,
     crypto_pwhash_ALG_DEFAULT) != 0)
    {
        std::cerr << "Error in the key generation" << std::endl;
    }
    
    return true;
}
