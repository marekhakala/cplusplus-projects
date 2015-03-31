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

#ifndef _USER_H
#define	_USER_H

#include <iostream>
#include <string>

#include <QDir>
#include <QString>
#include <QByteArray>
#include <QCryptographicHash>

#include "passwordtype.h"

namespace DummyFTPServer {
namespace AccountManager {

class User {
public:
    User(std::string username,
         std::string password,
         PasswordType format,
         std::string homeDirectory);

    std::string getUsername() const;
    std::string getHomeDirectory() const;

    bool checkPassword(std::string password, PasswordType format) const;
    bool isValidPath() const;

    bool operator==(const User &u) const;
    bool operator!=(const User &u) const;

    static bool isValidPath(const QString &path);

private:
    std::string m_username;
    std::string m_password;

    PasswordType m_passwordFormat;
    std::string m_homeDirectory;
};
}
}

#endif // USER_H
