#pragma once
#include <string>

struct Credential
{
    std::string name;
    std::string encryptedPassword;
    std::string nonce;
};