#include "UI/InitialMenu.h"
#include "Services/UserService.h"
#include "const/DataLocation.h"
#include <sodium.h>
#include <iostream>


int main ()
{
    bool filenameExisted;
    UserRepository userRepository(USERFILELOCATION, filenameExisted);
    UserService userService(userRepository);

    if (sodium_init() < 0)
    {
        std::cerr << "Failed to load sodium cryptographic library." << std::endl;
        return 1;
    }

    InitialMenu initialMenu(userService);
    initialMenu.menu();

    return 0;
}
