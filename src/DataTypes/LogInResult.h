#pragma once
#include <string>

struct LogInResult
{
    bool userDatabaseWasAccessed;
    bool correctUserAndPassword;
    std::string salt;
};