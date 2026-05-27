#pragma once
#include "Storage/UserRepository.h"
#include "DataTypes/LogInResult.h"
#include "DataTypes/User.h"
#include <string>
#include <vector>

class UserService
{
private:
    UserRepository& m_userRepository;    

public:
    UserService(UserRepository& userRepository);
    short areThereUsers ();
    short createUser(std::string& username, std::string& password);
    short deleteUser(std::string& username);
    short getUsers(std::vector<User>& userList);
    short isValidNewUsername (std::string& newUsername, std::string& suggestedUsername);
    LogInResult tryLogIn(std::string& username, std::string& password);
    short updatePassword(std::string& username, std::string& newPassword);
};