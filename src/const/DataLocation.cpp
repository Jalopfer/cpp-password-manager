#include "const/DataLocation.h"
#include <string>

std::string DataLocation::passwordFileLocation (std::string& username)
{
    return std::string("Data/"+username+".txt");
}