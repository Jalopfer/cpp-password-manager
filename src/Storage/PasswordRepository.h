#pragma once
#include "DataTypes/Credential.h"
#include <vector>
#include <string>

class PasswordRepository
{
    private:
        std::string m_filename;

    public:
        PasswordRepository(const std::string& filename, bool& filenameExisted);

        short addCredential(const Credential& newCredential);
        short deleteCredential(const std::string& credentialName);
        short exists(const std::string& credentialName);
        short getCredentials(std::vector<Credential>& credentialList);
        short updateCredential(const Credential& credential);
};