# 🔐 Password Manager

A terminal-based password manager written in C++.
 
## Features
 
- Master password authentication with Argon2 key derivation (via libsodium)
- Per-entry encryption using XSalsa20-Poly1305 authenticated encryption
- Unique random nonce generated for every encryption operation
- Passwords stored as Base64-encoded ciphertext — never in plaintext
- Multi-user support with per-user salt and password hash
- Clean layered architecture: UI → Service → Repository → FileManager


## Security Design
 
| Concern | Solution |
|---|---|
| Master password storage | Argon2 hash + random salt (libsodium `crypto_pwhash`) |
| Password encryption | XSalsa20-Poly1305 (`crypto_secretbox_easy`) |
| Key derivation | Master password + stored salt → encryption key |
| Nonce reuse | Fresh `randombytes_buf` nonce per encryption |
| Storage format | `name\|nonce_base64\|ciphertext_base64` per line |
 
Passwords are never stored or logged in plaintext. The encryption key is derived at runtime from the master password and is never persisted to disk.
 
## Project Structure

```
gestor/
├── src/
│   ├── main.cpp                  # Entry point, application flow
│   ├── const/
│   │   └── DataLocation.h/.cpp   # File path constants
│   ├── DataTypes/
│   │   ├── Credential.h          # Password entry data struct
│   │   ├── LogInResult.h         # Login result data struct
│   │   └── User.h                # User data struct
│   ├── Security/
│   │   ├── Auth.h/.cpp           # Master password hashing and verification
│   │   └── Crypto.h/.cpp         # Encryption, decryption, key derivation
│   ├── Services/
│   │   ├── LockService.h/.cpp    # Credentials business logic
│   │   └── UserService.h/.cpp    # User management business logic
│   ├── Storage/
│   │   ├── PasswordRepository.h/.cpp  # Password persistence
│   │   └── UserRepository.h/.cpp      # User persistence
│   └── UI/
│       ├── InitialMenu.h/.cpp    # Login and user creation screens
│       └── LoggedInMenu.h/.cpp   # Main app menu after authentication
└── Data/
    ├── users.txt                 # Hashed master passwords and salts
    └── passwords.txt             # Encrypted password entries
```
 
### Data Files
 
The files inside `Data/` are managed exclusively by the application. **Do not edit them manually** — they contain binary data encoded as Base64 with a specific format. Manual edits will corrupt the data and may prevent you from accessing your passwords.
 
## Dependencies
 
- [libsodium](https://libsodium.org/) — cryptographic primitives
Install on Ubuntu:
```bash
sudo apt install libsodium-dev
```
 
## Building
 
```bash
make
```

## Usage
 
```
1. Log In
2. Create a new user
3. See the users
4. Change password of a user
5. Delete a user
0. Exit
```
 
Once logged in:
```
1. Reveal a credential password
2. Add a credential
3. List of stored credentials
4. Edit a credential
5. Delete a credential
0. Log Off
```

## Known Limitations
- Usernames cannot contain spaces or the | character
- Credentials are stored in a plain text file rather than a database

## ⚠️ Disclaimer
 
This is a personal learning project built to explore cryptography concepts. It has not been audited, reviewed, or tested for security beyond the developer's own use.

**Do not use this to store passwords you cannot afford to lose or have exposed.**
 
Use it at your own risk. The author takes no responsibility for data loss, corruption, or security breaches of any kind.
