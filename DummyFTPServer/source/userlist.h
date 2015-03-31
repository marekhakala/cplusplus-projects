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

#ifndef _USERLIST_H
#define	_USERLIST_H

#include <iostream>
#include <algorithm>

#include <list>
#include <fstream>
#include <sstream>
#include <string>

#include "config.h"
#include "user.h"

namespace DummyFTPServer {
namespace AccountManager {

class UserList {
public:
    UserList(std::string userListFile);
    ~UserList();

    User *getByUsername(std::string username);
    bool add(User *user);
    bool remove(User *user);
    bool loadUserList();

    bool parseUser(std::string input);
    bool parsePassword(std::string input, std::string *password, PasswordType *type);

    bool contains(std::string username);
    int count() const;

    bool isEmpty() const;
    bool clean();

private:
    std::string userListPath;

    std::fstream userListFile;
    std::list<User *> userList;
};
}
}

#endif // USERLIST_H
