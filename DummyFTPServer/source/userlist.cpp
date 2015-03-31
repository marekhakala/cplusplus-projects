/***************************************************************************
 *   Copyright (C) 2010 by Marek Hakala   *
 *   hakala.marek@gmail.com   *
 *
 *   Semester project for BI-PA2 @ CTU FIT
 *   Topic: FTP server implementation
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Library General Public License as       *
 *   published by the Free Software Foundation; either version 2 of the    *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU Library General Public     *
 *   License along with this program; if not, write to the                 *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#include "core.h"
#include "userlist.h"

namespace DummyFTPServer {
namespace AccountManager {

UserList::UserList(std::string userListFile)
    : userListPath(userListFile) {

    this->userListFile.open(userListFile.c_str(), std::ios::in);

    std::cout << "[D] Loading user list (parsing content) ..." << std::endl;
    std::cout << "[D] is_open == " << this->userListFile.is_open()
              << " | filename == " << userListFile.c_str() << std::endl;
}

User *UserList::getByUsername(std::string username) {
    std::list<User *>::iterator it;

    for (it = userList.begin(); it != userList.end(); it++) {
        if (username == (*it)->getUsername())
            return (*it);
    }
    return NULL;
}

bool UserList::add(User *user) {
    if (contains(user->getUsername()))
        return false;

    userList.push_back(user);
    return true;
}

bool UserList::remove(User *user) {
    std::list<User *>::iterator it;

    for (it = userList.begin(); it != userList.end(); it++) {
        if (user->getUsername() == (*it)->getUsername()) {
            userList.erase(it);
            return true;
        }
    }
    return false;
}

bool UserList::loadUserList() {

    if (!this->userListFile.is_open())
        this->userListFile.open(this->userListPath.c_str(), std::ios::in);

    if (!this->userListFile.is_open())
        return false;

    clean();
    std::string bufferLine;

    while (getline(this->userListFile, bufferLine)) {

        if (*bufferLine.c_str() != '#') // Without comment
            parseUser(bufferLine);
        bufferLine.clear(); // Clear buffer
    }

    if (this->userListFile.is_open())
        this->userListFile.close();

    return true;
}

bool UserList::parseUser(std::string input) {
    std::stringstream ss(std::stringstream::in | std::stringstream::out);
    ss << input;

    std::string username;
    if (!Core::split(ss, &username, USERLIST_DELIMITER))
        return false;

    std::string password;
    if (!Core::split(ss, &password, USERLIST_DELIMITER))
        return false;

    std::string homeDirectory;
    if (!Core::split(ss, &homeDirectory, USERLIST_DELIMITER))
        return false;

    if (!User::isValidPath(QString::fromStdString(homeDirectory))) {

        std::cerr << "[W] !! username : " << username
                  << " | Path \"" << homeDirectory << "\" is not a valid home directory." << std::endl;

        return false;
    }

    std::string isEnabled;
    if (!Core::split(ss, &isEnabled, USERLIST_DELIMITER))
        return false;


    std::cout << "[D] username = " << username << " | password = " << password
              << " | home directory = " << homeDirectory
              << " | isEnabled = " << isEnabled << std::endl;

    if (Core::stringToBool(isEnabled)) {
        PasswordType type;

        if(parsePassword(password, &password, &type)) {
            add(new User(username, password, type, homeDirectory));
            return true;
        }
    }
    return false;
}

bool UserList::parsePassword(std::string input, std::string *password, PasswordType *passwordType) {
    std::string buffer;
    std::stringstream ss(std::stringstream::in | std::stringstream::out);
    ss << input;

    if (std::getline(ss, buffer, '|').fail())
        return false;
    std::string pass = buffer;

    if(std::getline(ss, buffer, '|').fail())
        return false;
    std::string type = buffer;

    std::transform(type.begin(), type.end(), type.begin(), ::tolower);

    if(type == "plain")
        *passwordType = PLAIN;
    else if(type == "md5")
        *passwordType = MD5;
    else if(type == "sha1")
        *passwordType = SHA1;
    else
        return false;

    password->clear();
    password->append(pass);
    return true;
}

bool UserList::contains(std::string username) {
    if (getByUsername(username) != NULL)
        return true;
    return false;
}

int UserList::count() const {
    return userList.size();
}

bool UserList::isEmpty() const {
    return userList.empty();
}

bool UserList::clean() {
    if (!userList.empty()) {
        std::list<User *>::iterator it;
        for (it = userList.begin(); it != userList.end(); it++)
            delete (*it);

        userList.clear();
        return true;
    }
    return false;
}

UserList::~UserList() {
    clean();

    if (this->userListFile.is_open())
        this->userListFile.close();
}
}
}
