#pragma once
#include "Services/LockService.h"

class LoggedInMenu
{
    private:
        LockService& m_lockService;

        void createCredentialOption();
        void deleteACredentialOption();
        void editACredentialOption();
        void revealCredentialOption();
        void showCredentialsOption();
        void printMenuWithCredentials();
        bool printMenuWithoutCredentials();

    public:
        LoggedInMenu(LockService& lockService);
        void menu();
};
