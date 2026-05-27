#include "UI/LoggedInMenu.h"
#include "Services/LockService.h"
#include "DataTypes/Credential.h"
#include <string>
#include <vector>
#include <iostream>
#include <fstream>



//EL CREDENTIAL SELECTION NO FUNCIONA

//BEFORE ENTERING THE MENU, I SHOULD CHECK IF THERE ARE STORED CREDENTIALS OR NOT
//Have a passwordservice, number of password, if it is 0, then do not allow options which cannot be done without passwords

LoggedInMenu::LoggedInMenu(LockService& lockService) : m_lockService(lockService) {}

void LoggedInMenu::createCredentialOption()
{
    std::string credentialName, suggestedCredentialName, passwordFirstInput, passwordSecondInput;
    short errorCode;;

    //Inputs    
    do
    {
        std::cout << "Introduce the new credential name (0 to cancel): ";
        getline(std::cin, credentialName);
        if (credentialName == "0") {return;}
        errorCode = m_lockService.isValidNewCredentialName(credentialName, suggestedCredentialName);
        if (errorCode == -2) {std::cout << "Invalid credential name: The credential name cannot contain \"|\"." << std::endl; continue;}
        if (errorCode == -1) {std::cout << "The credentials database could not be accessed. Please try again." << std::endl; continue;}
        if (errorCode != 1)
        {
            if (suggestedCredentialName != "")
                std::cout << "The credential " << credentialName << " already exists. An available credential is: " << suggestedCredentialName << "." << std::endl;
            else
                std::cout << "The credential " << credentialName << " already exists." << std::endl;
        }
    }while (errorCode != 1);
    do
    {
        std::cout << "Introduce the password: ";
        getline(std::cin, passwordFirstInput);
        std::cout << "Introduce the password again: ";
        getline(std::cin, passwordSecondInput);
        if (passwordFirstInput != passwordSecondInput)
            std::cout << "Passwords do not match. Pleasy, try again." << std::endl;
    } while (passwordFirstInput != passwordSecondInput);

    errorCode = m_lockService.addCredential(credentialName, passwordFirstInput);
    if (errorCode == 0)
    {
        std::cout << "The credential was sucessfully added" << std::endl;
        return;
    }
    if (errorCode == -1)
    {
        std::cout << "The credential was not added. Error: the credential database could not be accessed." << std::endl;
        return;
    }
    else if (errorCode == -2)
    {
        std::cout << "The credential was not added. Error: there was an error in the cryptographic module." << std::endl;
    }
}

void LoggedInMenu::deleteACredentialOption()
{
    std::vector<Credential> credentialList;
    short errorCode, credentialIdentifierInt, credentialIndex;

    errorCode = m_lockService.getCredentials(credentialList);
    if (errorCode == -1)
    {
        std::cout << "Error: the credential list could not be accessed. Please try again later." << std::endl;
    }
    do 
    {
        std::cout << "Select the credential to reval. Available credentials: " << std::endl;
        credentialIndex = 0;
        for (auto & credential : credentialList)
        {
            credentialIndex++; 
            std::cout << " " << credentialIndex << ". " << credential.name << std::endl;
        }
        std::cout << "Credential number: ";
        std::cin >> credentialIdentifierInt;
        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(INT_MAX, '\n');
            continue;
        }    
    }while (credentialIdentifierInt > credentialIndex || credentialIdentifierInt <= 0);
    //after knowing which credential is going to be changed the new password is input
    std::cin.ignore(INT_MAX, '\n');
    
    errorCode = m_lockService.deleteCredential(credentialList[credentialIdentifierInt-1].name);
    
}

void LoggedInMenu::editACredentialOption()
{
    std::vector<Credential> credentialList;
    std::string newPasswordFirstInput, newPasswordSecondInput;
    short errorCode, credentialIdentifierInt, credentialIndex;

    errorCode = m_lockService.getCredentials(credentialList);
    if (errorCode == -1)
    {
        std::cout << "Error: the credential list could not be accessed. Please try again later." << std::endl;
    }
    do 
    {
        std::cout << "Select the credential to reval. Available credentials: " << std::endl;
        credentialIndex = 0;
        for (auto & credential : credentialList)
        {
            credentialIndex++; 
            std::cout << " " << credentialIndex << ". " << credential.name << std::endl;
        }
        std::cout << "Credential number: ";
        std::cin >> credentialIdentifierInt;
        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(INT_MAX, '\n');
            continue;
        }    
    }while (credentialIdentifierInt > credentialIndex || credentialIdentifierInt <= 0);
    
    //after knowing which credential is going to be changed the new password is input
    std::cin.ignore(INT_MAX, '\n');
    do
    {
        std::cout << "Introduce the new password (0 to cancel): ";
        getline(std::cin, newPasswordFirstInput);
        if (newPasswordFirstInput == "0"){return;}
        std::cout << "Introduce the new password again: ";
        getline(std::cin, newPasswordSecondInput);
        if (newPasswordFirstInput != newPasswordSecondInput)
            std::cout << "Passwords do not match. Pleasy, try again." << std::endl;
    } while (newPasswordFirstInput != newPasswordSecondInput);

    //Finally the information is changed using the lockService
    errorCode = m_lockService.updateCredential(credentialList[credentialIdentifierInt-1].name, newPasswordFirstInput);
    if (errorCode == -2)
    {
        std::cout << "The credential could not be updated. There was an error in the cryptographic module." << std::endl;
    }
    else if (errorCode == -1)
    {
        std::cout << "The credential could not be updated. The user database could not be accessed." << std::endl;
    }
    else if (errorCode == 0)
    {
        std::cout << "The credential was succesfully updated." << std::endl;
    }
    else
    {
        std::cout << "The credential could not be updated. Please, try again" << std::endl;
    }
}

void LoggedInMenu::revealCredentialOption()
{
    std::string decryptedPassword;
    std::vector<Credential> credentialList;
    short errorCode, credentialIdentifierInt;
    short i;

    errorCode = m_lockService.getCredentials(credentialList);
    if (errorCode == -1)
    {
        std::cout << "Error: the credential list could not be accessed. Please try again later." << std::endl;
    }
    do 
    {
        std::cout << "Select the credential to reval. Available credentials: " << std::endl;
        i = 0;
        for (auto & credential : credentialList)
        {
            i++; 
            std::cout << " " << i << ". " << credential.name << std::endl;
        }
        std::cout << "Credential number: ";
        std::cin >> credentialIdentifierInt;
        if (std::cin.fail())
        {
            std::cin.clear();
            std::cin.ignore(INT_MAX, '\n');
            continue;
        }    
    }while (credentialIdentifierInt > i || credentialIdentifierInt <= 0);
    errorCode = m_lockService.revealCredential(credentialList[credentialIdentifierInt-1], decryptedPassword);
    std::cin.ignore(INT_MAX, '\n');
    if (errorCode == 0)
    {
        std::cout << "The password of " << credentialList[credentialIdentifierInt-1].name << " is " << decryptedPassword << "." << std::endl;
        return;
    }
    else
    {
        std::cout << "Error: the password could not be recovered. Please, try again." << std::endl;
        return;
    }
}

void LoggedInMenu::showCredentialsOption()
{
    std::vector<Credential> credentialList;
    short errorCode;

    errorCode = m_lockService.getCredentials(credentialList);

    //Errorcode: -1 if file could not be accessed, 0 if good
    if (errorCode == -1){std::cout << "Error: the credential database could not be accessed." << std::endl;}
    
    std::cout << "There are " << credentialList.size() << " stored credentials:" << std::endl;
    for (auto& credential : credentialList)
    {
        std::cout << " - " << credential.name << std::endl; 
    }
}

void LoggedInMenu::menu ()
{
    std::string menuSelection;
    short areThereCredentialsRes;

    while (true)
    {
        areThereCredentialsRes = m_lockService.areThereCredentials();

        if (areThereCredentialsRes == 1)
        {
            printMenuWithCredentials();
            getline(std::cin, menuSelection);
        
            if (menuSelection == "1")
                revealCredentialOption();
            else if (menuSelection == "2")
                createCredentialOption();
            else if (menuSelection == "3")
                showCredentialsOption();
            else if (menuSelection == "4")
                editACredentialOption();
            else if (menuSelection == "5")
                deleteACredentialOption();
            else if (menuSelection == "0")
                break;
            else
                std::cout << "The selected option is not valid" << std::endl;
        }
        else if (areThereCredentialsRes == 0)
        {
            if (!printMenuWithoutCredentials()){break;}
        }
    }
    

    return;
}

void LoggedInMenu::printMenuWithCredentials ()
{
    std::cout << "1. Reveal a credential password" << std::endl;
    std::cout << "2. Add a credential" << std::endl;
    std::cout << "3. List of stored credentials" << std::endl;
    std::cout << "4. Edit a credential" << std::endl;
    std::cout << "5. Delete a credential" << std::endl;
    std::cout << "0. Log Off" << std::endl;
}

bool LoggedInMenu::printMenuWithoutCredentials ()
{
    std::string ifWantToAddCredential;

    std::cout << "There are no stored credentials, do you want to add your first credential? (N to cancel)" << std::endl;
    getline(std::cin, ifWantToAddCredential);
    if (ifWantToAddCredential == "N"){return false;}
    createCredentialOption();
    return true;
}
