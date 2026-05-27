#pragma once
#include "Security/Crypto.h"
#include "Storage/PasswordRepository.h"
#include "DataTypes/Credential.h"
#include <vector>
#include <string>

class LockService
{
    private:
        Crypter& m_crypter;
        PasswordRepository& m_passwordRepository;

    public:
        LockService(Crypter& crypter, PasswordRepository& passwordRepository);

        short addCredential(const std::string& name, const std::string& password);
        short areThereCredentials ();
        short deleteCredential(std::string credentialName);
        short getCredentials(std::vector<Credential>& credentialList);
        short isValidNewCredentialName(const std::string& credentialName, std::string& suggestedCredentialName);
        short revealCredential(Credential& credential, std::string& decryptedPassword);
        short updateCredential(std::string& credentialName, std::string& newPassword);
};