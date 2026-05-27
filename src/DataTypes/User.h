#pragma once
#include <string>

struct User
{
    std::string name;
    std::string passwordHash;
    std::string salt;
};