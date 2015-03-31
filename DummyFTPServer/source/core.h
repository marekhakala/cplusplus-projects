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

#ifndef _CORE_H
#define	_CORE_H

#include <QObject>
#include <QPointer>

#include <string>
#include <exception>

#include "ftpserver.h"
#include "configuration.h"
#include "userlist.h"

#include "errorexception.h"

using DummyFTPServer::Configuration;
using DummyFTPServer::AccountManager::User;
using DummyFTPServer::AccountManager::UserList;

namespace DummyFTPServer {

class Core : public QObject {
    Q_OBJECT

public:
    bool loadConfiguration(std::string configurationPath, std::string userListPath);
    bool startServer();

    QString getAnonymousHome();

    bool getSettingsValue(std::string sectionAndKey, std::string *value);
    bool getSettingsValue(std::string section, std::string key, std::string *value);

    User *getUser(std::string username);
    int getUserCount() const;

    inline bool isEnabledAnonymousAccess() {
        return enableAnonymous;
    }

    inline static Core *Instance() {
        if (!pInstance)
            pInstance = new Core();

        return pInstance;
    }

    static bool validatePortRange(unsigned int port);
    static bool split(std::stringstream &ss, std::string *output, char delimiter = DEFAULT_DELIMITER);
    static bool stringToBool(std::string str);

private:
    Core() : QObject(0) {
        configuration = NULL;
        userList = NULL;
        enableAnonymous = false;
    }

    bool enableAnonymous;
    UserList *userList;
    Configuration *configuration;
    static QPointer<Core> pInstance;

    ~Core();
};
}

#endif // CORE_H
