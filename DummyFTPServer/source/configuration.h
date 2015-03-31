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

#ifndef _CONFIGURATION_H
#define	_CONFIGURATION_H

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <map>

#include "config.h"
#include "confsection.h"

using DummyFTPServer::ConfSection;

namespace DummyFTPServer {

class Configuration {

public:
    Configuration(std::string configFile = DEFAULT_CONF_PATH);
    ~Configuration();

    bool appendToSection(std::string key, std::string value,
                         std::string sectionName = "global");

    bool clean();
    bool loadConfiguration();
    ConfSection *getSection(std::string name);

private:
    bool parseSection(std::string input, std::string *section);
    bool parseValue(std::string sectionName, std::string input);

    std::string configPath;
    std::fstream configFile;
    std::map<std::string, ConfSection *> sections;
};
}

#endif // CONFIGURATION_H
