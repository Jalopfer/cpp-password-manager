#include "Services/UserService.h"
#include "DataTypes/LogInResult.h"
#include "DataTypes/User.h"
#include "Storage/UserRepository.h"
#include "Security/Auth.h"
#include <sodium.h>
#include <string>
#include <vector>

UserService::UserService (UserRepository& userRepository) : m_userRepository(userRepository) {}

//Errorcode: -1 userFile could not be accessed, 0 no users, 1 there are users
short UserService::areThereUsers ()
{
    std::vector<User> userList;
    short errorCode;

    errorCode = getUsers(userList);
    if (errorCode == -1) {return -1;}
    if (userList.size() == 0) 
    {
        return 0;
    }
    else 
    {
        return 1;
    }
}

//Errorcode: -1 the user database could not be open -2 the hash generation could not be made. We add conversion of the salt into base 64
short UserService::createUser (std::string& username, std::string& password)
{
    std::string passwordHash, stringSalt;
    short errorCode;
    unsigned char salt[crypto_pwhash_SALTBYTES];
    
    //Password generated for the user authentfication, 
    //then salt for encrypting the passwords
    if (Auth::generateHashFromPassword(password, passwordHash))
    {
        randombytes_buf(salt, sizeof salt);
        size_t salt_encoded_len = sodium_base64_ENCODED_LEN(sizeof salt, sodium_base64_VARIANT_ORIGINAL);

        stringSalt = std::string(salt_encoded_len, '\0');
        sodium_bin2base64(stringSalt.data(), salt_encoded_len, salt, sizeof salt, sodium_base64_VARIANT_ORIGINAL);
        errorCode = m_userRepository.addUser({username, passwordHash, stringSalt});
        if (errorCode == -1)
            return -1;
        else if (errorCode == 0)
            return 0;
    }
    else 
        return -2;
    
    return -1;
}

//ERRORCODE: 0 no error, -1 the user database could not be accessed
short UserService::deleteUser(std::string& username)
{
    return m_userRepository.deleteUser(username);
}

short UserService::getUsers (std::vector<User>& userList)
{
    return m_userRepository.getUsers(userList);
}

//RETURN: 1 is available, 0 not available, -1 database could not be open, -2 it contains |
short UserService::isValidNewUsername (std::string& username, std::string& suggestedUsername)
{
    short userExistsResult;

    if (username.find('|') != std::string::npos) {return -2;}

    userExistsResult = m_userRepository.exists (username);
    switch (userExistsResult)
    {
        case 0:
            return 1;
        case 1:
            for (short i = 0; i < 100; i++)
            {
                suggestedUsername = username + std::to_string(i);
                if (m_userRepository.exists(suggestedUsername)==0){return false;}
            }
            suggestedUsername = "";
            return 0;
        default:
            return -1;
    }
}

LogInResult UserService::tryLogIn (std::string& username, std::string& password)
{
    //Errorcodes: -1 user database could not be 
    User user;
    short errorCode;

    errorCode = m_userRepository.getUser(username, user);
    if (errorCode == -1)
    {
        return {true, false, ""};//No user with the introduced name
    }
    if (errorCode == -2)
    {
        return {false, false, ""};//database could not be accessed
    }

    if (Auth::verifyPassword(password, user.passwordHash))
    {
        return {true, true, user.salt};
    }
    else
    {
        return {true, false, ""};
    }
}

//Error code: -2 error in the cryptographic package, -1 could not access the database, 0 no error
short UserService::updatePassword(std::string& username, std::string& newPassword)
{
    std::string newPasswordHash;
    short errorCode;

    if (Auth::generateHashFromPassword(newPassword, newPasswordHash))
    {
        errorCode = m_userRepository.updatePasswordHash(username, newPasswordHash);
        if (errorCode == -1)
            return -1;
        else if (errorCode == 0)
            return 0;
    }
    else 
        return -2;
    
    return -1;
}
