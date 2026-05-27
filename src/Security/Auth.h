#pragma once
#include <string>

namespace Auth
{
    bool generateHashFromPassword (const std::string& password, std::string& hashedPassword);
    bool verifyPassword (const std::string& password, const  std::string& hashedPassword);
}