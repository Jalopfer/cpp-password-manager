#include "Storage/PasswordRepository.h"
#include "const/DataLocation.h"
#include <string>
#include <fstream>
#include <sstream>
#include <filesystem>

PasswordRepository::PasswordRepository (const std::string& filename, bool& filenameExisted) : m_filename(filename) 
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

//Errorcode: -1 if file cannot be open, 0 if success
short PasswordRepository::addCredential(const Credential& newCredential)
{
    std::ofstream file(m_filename, std::ios::app);

    if (file.is_open())
        file << newCredential.name << "|" << newCredential.encryptedPassword << "|" << newCredential.nonce << "\n";
    else
        return -1;
    return 0;
}

//Creation of another file for deleting ensures avoids possible problems of very long lists of users
//Errorcode: -1 if file cannot be open, 0 if success
short PasswordRepository::deleteCredential(const std::string& credentialName)
{
    std::ifstream inputFile(m_filename);
    std::ofstream outputFile(TEMPORALFILELOCATION);
    std::string currentLine, name, encryptedPassword, nonce;

    if (inputFile.is_open())
    {
        if (outputFile.is_open())
        {
            while (getline(inputFile, currentLine)) 
            {
                std::stringstream ss(currentLine);
                getline(ss, name, '|');
                if (name == credentialName)
                {
                    continue;
                }
                getline(ss, encryptedPassword, '|');
                getline(ss, nonce, '|');
                outputFile << name << "|" << encryptedPassword << "|" << nonce << "\n";
            }
        } 
        else {return -1;}
    }    
    else{return -1;}

    remove(m_filename.c_str());
    rename(TEMPORALFILELOCATION.c_str(),m_filename.c_str());

    return 0;
}

//Errorcode: 0 if not exists, 1 if exists
short PasswordRepository::exists(const std::string& credentialName)
{
    std::ifstream file(m_filename);
    std::string currentLine, name;

    if (file.is_open())
    {
        while (getline(file, currentLine))
        {
            std::stringstream ss(currentLine);
            getline(ss, name, '|');
            if (name == credentialName)
            {
                return 1;
            }
        }
        return 0;
    }
    else {return -1;}
}

//Errorcode: -1 if file could not be accessed, 0 if good
short PasswordRepository::getCredentials (std::vector<Credential>& credentialList)
{
    std::ifstream file(m_filename);
    std::string currentLine;
    std::string name, encryptedPassword, nonce;

    if (file.is_open())
        {
            while (getline(file, currentLine)) 
            {
                std::stringstream ss(currentLine);
                getline(ss, name, '|');
                getline(ss, encryptedPassword, '|');
                getline(ss, nonce, '|');
                credentialList.push_back({name, encryptedPassword, nonce});
            }
        }    
        else
            return -1;

    return 0;
}

//Creation of another file for deleting ensures avoids possible problems of very long lists of users
//ERRORCODE: 0 if success, -1 if the file cannot be open
short PasswordRepository::updateCredential(const Credential& credential)
{
    std::ifstream inputFile(m_filename);
    std::ofstream outputFile(TEMPORALFILELOCATION);
    std::string currentLine, name, encryptedPassword, nonce;

    if (inputFile.is_open())
    {
        if (outputFile.is_open())
        {
            while (getline(inputFile, currentLine)) 
            {
                std::stringstream ss(currentLine);
                getline(ss, name, '|');
                getline(ss, encryptedPassword, '|');
                getline(ss, nonce, '|');
                if (name == credential.name)
                {
                    outputFile << name << "|" << credential.encryptedPassword << "|" << credential.nonce << "\n";
                    continue;
                }
                else
                {
                    outputFile << name << "|" << encryptedPassword << "|" << nonce << "\n";
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
