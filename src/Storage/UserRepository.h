#pragma once
#include "DataTypes/User.h"
#include <vector>

class UserRepository
{
private:
    std::string m_filename;

public:
    UserRepository(const std::string& filename, bool& filenameExisted);
    short addUser(User newUser);
    bool cleanFile ();
    short deleteUser(std::string& username);
    short exists(std::string& username);
    short getUser(std::string& username, User& user);
    short getUsers(std::vector<User>& userList);
    short updatePasswordHash(std::string& username, std::string& newPasswordHash);
};
