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

#ifndef ERROREXCEPTION_H
#define ERROREXCEPTION_H

#include <iostream>
#include <exception>

namespace DummyFTPServer {

class ErrorException : public std::exception {

public:
    ErrorException(const std::string &errorMessage, bool critical = false) throw () {
        this->critical = critical;

        message.append("[!] ");
        message.append(errorMessage);
        if(this->critical) {
            message.append("\n[-] ");
            message.append("Stopping server ...");
        }
    }

    virtual const char *what() const throw () {
        return message.c_str();
    }

    virtual bool isCritical() const {
        return critical;
    }

    virtual ~ErrorException() throw () {
    }

private:
    std::string message;
    bool critical;
};

}

#endif // ERROREXCEPTION_H
