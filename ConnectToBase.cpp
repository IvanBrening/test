#include "ConnectToBase.h"
#include "Error.h"
#include <fstream>
#include <sstream>
#include <openssl/sha.h>
#include <iomanip>
#include <algorithm>
#include <iostream> // Добавлено для cout

bool ConnectToBase::authenticateUser(const std::string& login, const std::string& salt, const std::string& clientHash, const std::string& dbFileName) {
    std::ifstream dbFile(dbFileName);
    if (!dbFile.is_open()) {
        Error::logError("Cannot open database file.", true);
        return false;
    }

    std::string dbLogin, dbPassword;
    while (dbFile >> dbLogin >> dbPassword) {
        if (dbLogin == login) {
            std::string serverHash = hashPassword(dbPassword, salt);
            return compareHashes(serverHash, clientHash);
        }
    }
    return false; // Если не нашли пользователя
}

std::string ConnectToBase::hashPassword(const std::string& password, const std::string& salt) {
    std::string data = salt + password;
    unsigned char result[SHA_DIGEST_LENGTH];
    SHA1(reinterpret_cast<const unsigned char*>(data.c_str()), data.size(), result);

    std::stringstream hashStream;
    for (int i = 0; i < SHA_DIGEST_LENGTH; ++i) {
        hashStream << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(result[i]);
    }
    return hashStream.str();
}

bool ConnectToBase::compareHashes(const std::string& serverHash, const std::string& clientHash) {
    std::string clientHashLower = clientHash;
    std::string serverHashLower = serverHash;
    std::transform(clientHashLower.begin(), clientHashLower.end(), clientHashLower.begin(), ::tolower);
    std::transform(serverHashLower.begin(), serverHashLower.end(), serverHashLower.begin(), ::tolower);
    return serverHashLower == clientHashLower;
}
