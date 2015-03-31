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

using DummyFTPServer::ConfSection;
using DummyFTPServer::AccountManager::User;
using DummyFTPServer::AccountManager::UserList;

namespace DummyFTPServer {

QPointer<Core> Core::pInstance = NULL;

bool Core::loadConfiguration(std::string configurationPath, std::string userListPath) {

    configuration = new Configuration(configurationPath);

    if (!configuration->loadConfiguration())
        throw ErrorException("Bad configuration file.");

    userList = new UserList(userListPath);

    if (!userList->loadUserList())
        throw ErrorException("Bad user list file.");

    std::string anonymous;
    if (getSettingsValue("global/anonymous", &anonymous) && Core::stringToBool(anonymous)) {
        std::cout << "[D] Anonymous [enable] " << std::endl;
        this->enableAnonymous = true;
    } else {
        std::cout << "[D] Anonymous [disable]" << std::endl;
        this->enableAnonymous = false;
    }

    return true;
}

bool Core::startServer() {
    std::string portValue;

    if (!getSettingsValue("global/port", &portValue))
        throw ErrorException("Port is not set.", true);

    if (!Core::validatePortRange(atoi(portValue.c_str())))
        throw ErrorException("Bad port number.", true);

    if (!isEnabledAnonymousAccess() && getUserCount() == 0)
        throw ErrorException("The user list is empty.", true);

    DummyFTPServer::FTPServer::start(atoi(portValue.c_str()));
    return true;
}

bool Core::getSettingsValue(std::string sectionAndKey, std::string* value) {
    std::string buffer;
    std::stringstream ss(std::stringstream::in | std::stringstream::out);

    ss << sectionAndKey;

    if (std::getline(ss, buffer, '/').fail())
        return false;
    std::string section = buffer;

    if (std::getline(ss, buffer, '/').fail())
        return false;
    std::string key = buffer;

    return getSettingsValue(section, key, value);
}

bool Core::getSettingsValue(std::string sectionName, std::string key,
                            std::string* value) {
    ConfSection* section = this->configuration->getSection(sectionName);

    if (!section)
        return false;

    std::string val = section->getValue(key);

    if (val.empty())
        return false;

    value->clear();
    value->append(val);
    return true;
}

QString Core::getAnonymousHome() {
    std::string anonymousValue;

    if (!getSettingsValue("global/anonymousPath", &anonymousValue))
        return QString("/tmp");
    return QString::fromStdString(anonymousValue);
}

User* Core::getUser(std::string username) {
    return this->userList->getByUsername(username);
}

int Core::getUserCount() const {
    return this->userList->count();
}

bool Core::validatePortRange(unsigned int port) {
    if ((port > 0 && port <= 1023)
            || (port >= 1024 && port <= 49151)
            || (port >= 49152 && port <= 65535))
        return true;
    return false;
}

bool Core::split(std::stringstream& ss, std::string* output, char delimiter) {
    std::string buffer;

    if (std::getline(ss, buffer, delimiter).fail())
        return false;

    output->clear();
    output->append(buffer);
    return true;
}

bool Core::stringToBool(std::string str) {
    std::transform(str.begin(), str.end(), str.begin(), ::tolower);

    if (str.compare("true") == 0)
        return true;
    return false;
}

Core::~Core() {
    delete userList;
    delete configuration;
}
}
