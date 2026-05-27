#include "UI/InitialMenu.h"
#include "UI/LoggedInMenu.h"
#include "DataTypes/User.h"
#include "Security/Crypto.h"
#include "DataTypes/LogInResult.h"
#include "const/DataLocation.h"
#include "Storage/PasswordRepository.h"
#include "Services/LockService.h"
#include <iostream>
#include <string>


//Have a userservice, number of users, if it is 0, then do not allow options which cannot be done without users

InitialMenu::InitialMenu (UserService& userService) : m_userService(userService) {}

void InitialMenu::createUserOption ()
{
    std::string username, suggestedUsername, passwordFirstInput, passwordSecondInput;
    short errorCode;

    //Inputs    
    do
    {
        std::cout << "Introduce the new username, it cannot contain \"|\" (0 to cancel): ";
        getline(std::cin, username);
        if (username == "0") {return;}
        errorCode = m_userService.isValidNewUsername(username, suggestedUsername);
        if (errorCode == -2) {std::cout << "Invalid username: the username cannot contain \"|\"." << std::endl; continue;}
        if (errorCode == -1) {std::cout << "The user database could not be accessed. Please try again."; continue;}
        if (errorCode != 1)
        {
            if (suggestedUsername != "")
                std::cout << "The user " << username << " already exists. An available username is: " << suggestedUsername << "." << std::endl;
            else
                std::cout << "The user " << username << " already exists." << std::endl;
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
    
    errorCode = m_userService.createUser(username, passwordFirstInput);
    
    switch (errorCode)
    {
    case 0:
        std::cout << "The user " << username << " was succesfully created." << std::endl;
        break;
    case -1:
        std::cout << "The user could not be created. The user database could not be accessed." << std::endl;
        break;
    case -2:
        std::cout << "The user could not be created. There was an error in the cryptographic module." << std::endl;
        break;
    default:
        std::cout << "The user could not be created." << std::endl;
        break;
    }
}

void InitialMenu::deleteUserOption()
{
    std::string username, password, deleteConfirmation;
    LogInResult logInResult;
    short errorCode;

    do
    {
        std::cout << "Introduce the user to delete (0 to cancel): ";
        getline(std::cin, username);
        if (username=="0"){return;}
        std::cout << "Introduce the password: ";
        getline(std::cin, password);

        logInResult = m_userService.tryLogIn(username, password);
        if (!logInResult.userDatabaseWasAccessed)
        {
            std::cout << "Error: The userdatabase could not be accessed. (Check if there are available users)" << std::endl;
        }
        else if (!logInResult.correctUserAndPassword)
        {
            std::cout << "The username or the password are not correct." << std::endl;
        }
    }while (!logInResult.correctUserAndPassword);

    std::cout << "The credentials are correct. Are you sure you want to PERMANENTLY delete the user " << username << ", losing the access to all its stored passwords? [Y for confirmation] ";
    getline(std::cin, deleteConfirmation);
    if (deleteConfirmation == "Y")
    {
        errorCode = m_userService.deleteUser(username);
        switch (errorCode)
        {
        case 0:
            std::cout << "The user " << username << " was succesfully deleted." << std::endl;
            return;
        case -1:
            std::cout << "The user database could not be accessed. Please try again." << std::endl;
        }
    }
    else
    {
        std::cout << "No action was done." << std::endl;
        return;
    }
}

void InitialMenu::logInOption ()
{
    std::string username, password;
    LogInResult logInResult;
    bool passwordFilenameExists;

    do
    {
        std::cout << "Introduce the username (0 to cancel): ";
        getline(std::cin, username);
        if (username=="0"){return;}
        std::cout << "Introduce the password: ";
        getline(std::cin, password);

        logInResult = m_userService.tryLogIn(username, password);
        if (!logInResult.userDatabaseWasAccessed)
        {
            std::cout << "Error: The userdatabase could not be accessed. (Check if there are available users)" << std::endl;
        }
        else if (!logInResult.correctUserAndPassword)
        {
            std::cout << "The username or the password are not correct." << std::endl;
        }
    } while (!logInResult.correctUserAndPassword);
    
    PasswordRepository passwordRepository(DataLocation::passwordFileLocation(username), passwordFilenameExists);
    Crypter crypter(password, logInResult.salt);
    LockService lockService(crypter, passwordRepository);
    LoggedInMenu loggedInMenu(lockService);
    
    std::cout << "You logged in successfully." << std::endl;
    if (passwordFilenameExists)
    {
        std::cout << "Hello, " << username << "!" << std::endl;
    }
    else
    {
        std::cout << "Welcome " << username << "!" << std::endl;
    }

    //memory is cleaned after the user loggs off
    loggedInMenu.menu();
    sodium_memzero(&password[0], password.size());
}

void InitialMenu::showUsersOption()
{
    std::vector<User> userList;
    short errorCode;

    errorCode = m_userService.getUsers(userList);

    if (errorCode == -1)
    {
        std::cout << "Error: The user datatabase could not be accessed." << std::endl;
        return;
    }
    if (userList.size() == 0)
    {
        std::cout << "There are yet no stored users" << std::endl;
        return;
    }
    std::cout << "There are " << userList.size() << " stored users:" << std::endl;
    for (auto& user : userList)
    {
        std::cout << " - " << user.name << std::endl; 
    }
}

void InitialMenu::updatePasswordOption()
{
    std::string username, password, newPasswordFirstInput, newPasswordSecondInput;
    LogInResult logInResult;
    short errorCode;

    do
    {
        std::cout << "Introduce the username (0 to cancel): ";
        getline(std::cin, username);
        if (username=="0"){return;}
        std::cout << "Introduce the password: ";
        getline(std::cin, password);

        logInResult = m_userService.tryLogIn(username, password);
        if (!logInResult.userDatabaseWasAccessed)
        {
            std::cout << "Error: The userdatabase could not be accessed. (Check if there are available users)" << std::endl;
        }
        else if (!logInResult.correctUserAndPassword)
        {
            std::cout << "The username or the password are not correct." << std::endl;
        }
    } while (!logInResult.correctUserAndPassword);

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
    
    errorCode = m_userService.updatePassword(username, newPasswordFirstInput);
    if (errorCode == -2)
    {
        std::cout << "Password could not be updated. There was an error in the cryptographic module." << std::endl;
    }
    else if (errorCode == -1)
    {
        std::cout << "Password could not be updated. The user database could not be accessed." << std::endl;
    }
    else if (errorCode == 0)
    {
        std::cout << "Password was succesfully updated." << std::endl;
    }
    else
    {
        std::cout << "Password could not be updated. Please, try again" << std::endl;
    }
}

void InitialMenu::menu ()
{
    std::string menuSelection;
    short areThereUsersRes;

    while (true)
    {
        areThereUsersRes = m_userService.areThereUsers();

        if (areThereUsersRes == 1)
        {
            printWithUsersMenu();
            getline(std::cin, menuSelection);
        
            if (menuSelection == "1")
                logInOption();
            else if (menuSelection == "2")
                createUserOption();
            else if (menuSelection == "3")
                showUsersOption();
            else if (menuSelection == "4")
                updatePasswordOption();
            else if (menuSelection == "5")
                deleteUserOption();
            else if (menuSelection == "0")
                break;
            else
                std::cout << "The selected option is not valid" << std::endl;
        }
        else if (areThereUsersRes == 0)
        {
            if(!printWithoutUsersMenu()){break;}
        }
        else if (areThereUsersRes == -1)
        {
            std::cout << "The User file (Data/Users.txt) could not be found." << std::endl;
            break;
        }
    }
    
    return;
}

void InitialMenu::printWithUsersMenu ()
{
    std::cout << "1. Log In" << std::endl;
    std::cout << "2. Create a new user" << std::endl;
    std::cout << "3. See the users" << std::endl;
    std::cout << "4. Change password of a user" << std::endl;
    std::cout << "5. Delete a user" << std::endl;
    std::cout << "0. Exit" << std::endl;
}

bool InitialMenu::printWithoutUsersMenu ()
{
    std::string ifWantTocreateUser;

    std::cout << "Welcome! There are no users stored. Do you want to create the first user? (N to cancel)" << std::endl;
    getline(std::cin, ifWantTocreateUser);
    if(ifWantTocreateUser == "N"){return false;}
    createUserOption();
    return true;
}
