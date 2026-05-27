#pragma once
#include <string>

#define USERFILELOCATION std::string("Data/Users.txt") //Location where the users are stored
#define TEMPORALFILELOCATION std::string("Data/temporal.txt")

namespace DataLocation
{
    std::string passwordFileLocation(std::string& username);
}