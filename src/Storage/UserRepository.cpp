#include "Storage/UserRepository.h"
#include "const/DataLocation.h"
#include "DataTypes/User.h"
#include <fstream>
#include <vector>
#include <sstream>

UserRepository::UserRepository (const std::string& filename, bool& filenameExisted) : m_filename(filename)
{
    //checks if the file exists
    std::ifstream dataFile(filename);
    if (!dataFile.good())
    {
        std::ofstream outfile (filename);
        filenameExisted = false;
    }
    else
    {
        filenameExisted = true;
    }
}

short UserRepository::addUser (User newUser)
{
    std::ofstream file(m_filename, std::ios::app);

    if (file.is_open())
        file << newUser.name << "|" << newUser.passwordHash << "|" << newUser.salt << "\n";
    else
        return -1;
    return 0;
}

bool UserRepository::cleanFile ()
{
    std::ofstream file(m_filename);

    if(file.is_open())
    {
        return true;
    }
    return false;
}

//Creation of another file for deleting ensures avoids possible problems of very long lists of users
//ERRORCODE: 0 if success, -1 if the file cannot be open
short UserRepository::deleteUser(std::string& username)
{
    std::ifstream inputFile(m_filename);
    std::ofstream outputFile(TEMPORALFILELOCATION);
    std::string currentLine, name, passwordHash, salt;

    if (inputFile.is_open())
    {
        if (outputFile.is_open())
        {
            while (getline(inputFile, currentLine)) 
            {
                std::stringstream ss(currentLine);
                getline(ss, name, '|');
                if (name == username)
                {
                    continue;
                }
                getline(ss, passwordHash, '|');
                getline(ss, salt, '|');
                outputFile << name << "|" << passwordHash << "|" << salt << "\n";
            }
        } 
        else {return -1;}
    }    
    else{return -1;}

    remove(m_filename.c_str());
    rename(TEMPORALFILELOCATION.c_str(), m_filename.c_str());
    remove(DataLocation::passwordFileLocation(username).c_str());

    return 0;
}

short UserRepository::exists(std::string& username)
{
    //Return: 1 exists, 0 does not exist, -1 could not access the folder
    std::ifstream file(m_filename);
    std::string currentLine, name;

    if (file.is_open())
    {
        while (getline(file, currentLine))
        {
            std::stringstream ss(currentLine);
            getline(ss, name, '|');
            if (name == username)
            {
                return 1;
            }
        }
        return 0;
    }
    else {return -1;}
}

short UserRepository::getUser (std::string& username, User& user)
{
    //CODE ERRORS: -1 no user with introduced username. -2 user database could not be accessed
    std::ifstream file(m_filename);
    std::string currentLine, name, hashedPassword, salt;

    if (file.is_open())
    {
        while (getline(file, currentLine))
        {
            std::stringstream ss(currentLine);
            getline(ss, name, '|');
            if (name == username)
            {
                getline(ss, hashedPassword, '|');
                getline(ss, salt, '|');
                user = {username, hashedPassword, salt};
                return 0;
            }
        }
        return -1;
    }
    else {return -2;}
}

short UserRepository::getUsers (std::vector<User>& userList)
{
    //Errorcode: -1 if file could not be accessed, 0 if good
    std::ifstream file(m_filename);
    std::string currentLine;
    std::string name, passwordHash, salt;

    if (file.is_open())
        {
            while (getline(file, currentLine)) 
            {
                std::stringstream ss(currentLine);
                getline(ss, name, '|');
                getline(ss, passwordHash, '|');
                getline(ss, salt, '|');
                userList.push_back({name, passwordHash, salt});
            }
        }    
        else
            return -1;

    return 0;
}

//Creation of another file for deleting ensures avoids possible problems of very long lists of users
//ERRORCODE: 0 if success, -1 if the file cannot be open
short UserRepository::updatePasswordHash(std::string& username, std::string& newPasswordHash)
{
    std::ifstream inputFile(m_filename);
    std::ofstream outputFile(TEMPORALFILELOCATION);
    std::string currentLine, name, passwordHash, salt;

    if (inputFile.is_open())
    {
        if (outputFile.is_open())
        {
            while (getline(inputFile, currentLine)) 
            {
                std::stringstream ss(currentLine);
                getline(ss, name, '|');
                getline(ss, passwordHash, '|');
                getline(ss, salt, '|');
                if (name == username)
                {
                    outputFile << name << "|" << newPasswordHash << "|" << salt << "\n";
                    continue;
                }
                else
                {
                    outputFile << name << "|" << passwordHash << "|" << salt << "\n";
                }
            }
        } 
        else {return -1;}
    }    
    else{return -1;}

    remove(m_filename.c_str());
    rename(TEMPORALFILELOCATION.c_str(),m_filename.c_str());

    return 0;
}
