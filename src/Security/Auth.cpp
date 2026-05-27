#include "Security/Auth.h"
#include <string>
#include <sodium.h>


bool Auth::generateHashFromPassword (const std::string& password, std::string& hashedPassword)
{
    char f_hashed_password[crypto_pwhash_STRBYTES];

    if (crypto_pwhash_str(f_hashed_password, password.c_str(), password.length(),
        crypto_pwhash_OPSLIMIT_SENSITIVE, crypto_pwhash_MEMLIMIT_SENSITIVE) != 0)
    {
        return false;
    }

    hashedPassword = std::string(f_hashed_password, crypto_pwhash_STRBYTES);
    return true;
}

bool Auth::verifyPassword (const std::string& password, const  std::string& hashedPassword)
{
    if (crypto_pwhash_str_verify (hashedPassword.c_str(), password.c_str(), password.length()) != 0) 
    {
        return false;
    }

    return true;
}
