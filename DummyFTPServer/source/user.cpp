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

#include "user.h"

namespace DummyFTPServer {
namespace AccountManager {

User::User(std::string username,
           std::string password,
           PasswordType format,
           std::string homeDirectory)
    : m_username(username),
      m_password(password),
      m_passwordFormat(format) {
    m_homeDirectory = homeDirectory;
}

std::string User::getUsername() const {
    return this->m_username;
}

std::string User::getHomeDirectory() const {
    return this->m_homeDirectory;
}

bool User::checkPassword(std::string password, PasswordType format) const {
    if(format != this->m_passwordFormat) {

        if(format == AccountManager::PLAIN) {
            switch(this->m_passwordFormat) {
            case AccountManager::MD5:
                if(QCryptographicHash::hash(QByteArray(password.c_str()),
                                            QCryptographicHash::Md5).toHex() == QString::fromStdString(m_password))
                    return true;
                return false;
                break;

            case AccountManager::SHA1:
                if(QCryptographicHash::hash(QByteArray(password.c_str()),
                                            QCryptographicHash::Sha1).toHex() == QString::fromStdString(m_password))
                    return true;
                return false;
                break;
            case AccountManager::PLAIN:
                break;
            }
        } else
            return false;
    }

    if(m_password.compare(password) != 0)
        return false;

    return true;
}

bool User::isValidPath() const {
    QDir directory(QString(this->getHomeDirectory().c_str()));
    return directory.exists();
}

bool User::isValidPath(const QString &path) {
    QDir directory(path);
    return directory.exists();
}


bool User::operator==(const User &u) const {
    if(u.checkPassword(m_password, m_passwordFormat))
        return true;
    return false;
}

bool User::operator!=(const User &u) const {
    if((*this) == u)
        return false;
    return true;
}
}
}
