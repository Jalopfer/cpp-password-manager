#include "Services/LockService.h"
#include "Security/Crypto.h"
#include "Storage/PasswordRepository.h"
#include <string>
#include <vector>

LockService::LockService(Crypter& crypter, PasswordRepository& passwordRepository) : m_crypter(crypter), m_passwordRepository(passwordRepository) {}

//Errorcode: -2 error while encrypting, -1 file could not be accessed, 0 if good
short LockService::addCredential(const std::string& name, const std::string& password)
{
    std::string encryptedPassword, nonce;

    if(!m_crypter.encrypt(password, encryptedPassword, nonce) == true)
    {
        return -2;
    }

    return m_passwordRepository.addCredential({name, encryptedPassword, nonce});
}

//Errorcode: -1 CredentialFile could not be accessed, 0 no credentials stored, 1 there are stored credentials
short LockService::areThereCredentials ()
{
    std::vector<Credential> credentialList;
    short errorCode;

    errorCode = getCredentials(credentialList);
    if (errorCode == -1) {return -1;}
    if (credentialList.size() == 0) 
    {
        return 0;
    }
    else 
    {
        return 1;
    }
}

//Errorcode: -1 error with the files, 0 good
short LockService::deleteCredential(std::string credentialName)
{
    return m_passwordRepository.deleteCredential(credentialName);
}

//Errorcode: -1 if file could not be accessed, 0 if good
short LockService::getCredentials(std::vector<Credential>& credentialList)
{
    return m_passwordRepository.getCredentials(credentialList);
}

//Errorcode: 1 is available, 0 not available , -1 could not open the password database, -2 credential name contains -2
short LockService::isValidNewCredentialName(const std::string& credentialName, std::string& suggestedCredentialName)
{
    short errorCode;

    if (credentialName.find('|') != std::string::npos) {return -2;}

    errorCode = m_passwordRepository.exists(credentialName);
    switch (errorCode)
    {
        case 0:
            return 1;
        case 1:
            for (short i = 0; i < 100; i++)
            {
                suggestedCredentialName = credentialName + std::to_string(i);
                if (m_passwordRepository.exists(suggestedCredentialName)==0){return false;}
            }
            suggestedCredentialName = "";
            return 0;
        default:
            return -1;
    }
}

//Errorcode: 0 success, -1 could not decrypt
short LockService::revealCredential(Credential& credential, std::string& decryptedPassword)
{
    if(m_crypter.decrypt(credential.encryptedPassword, decryptedPassword, credential.nonce))
    {
        return 0;
    }
    return -1;
}

//Error code: -2 error in the cryptographic package, -1 could not access the database, 0 no error
short LockService::updateCredential(std::string& credentialName, std::string& newPassword)
{
    std::string encryptedPassword, nonce;

    if(!m_crypter.encrypt(newPassword, encryptedPassword, nonce) == true)
    {
        return -2;
    }

    return m_passwordRepository.updateCredential({credentialName, encryptedPassword, nonce});
}
