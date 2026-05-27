#pragma once
#include "DataTypes/User.h"
#include "Storage/UserRepository.h"
#include "Services/UserService.h"
#include <vector>
#include <string>

class InitialMenu
{

    public:
        InitialMenu (UserService& userService);
        void menu();

    private:
        UserService& m_userService;
        
        void createUserOption();
        void deleteUserOption();
        void logInOption ();
        void showUsersOption();
        bool printWithoutUsersMenu();
        void printWithUsersMenu();
        void updatePasswordOption();
};
