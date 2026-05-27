#pragma once
#include <string>
#include <vector>
#include <sodium.h>

class Crypter
{
    public:
        Crypter(const std::string& password, const std::string& salt);
        ~Crypter();
        bool encrypt(const std::string& data, std::string& encryptedData, std::string& nonce);
        bool decrypt(const std::string& encryptedData, std::string& decryptedData, std::string& nonce);
        
    private:
        unsigned char m_key[crypto_secretbox_KEYBYTES];
        //std::vector<unsigned char> m_key;
        bool generateKey(const std::string& password, const std::string& salt);
};