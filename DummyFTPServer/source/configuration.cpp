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

#include "configuration.h"

using DummyFTPServer::ConfSection;

namespace DummyFTPServer {

Configuration::Configuration(std::string configFile)
    : configPath(configFile) {
    this->configFile.open(configFile.c_str(), std::ios::in);

    std::cout << "[D] Loading configuration (parsing content) ..." << std::endl;
    std::cout << "[D] is_open == " << this->configFile.is_open()
              << " | filename == " << configFile.c_str() << std::endl;
}

bool Configuration::appendToSection(std::string key, std::string value,
                                    std::string sectionName) {

    ConfSection *section = NULL;
    section = getSection(sectionName);

    if (section == NULL) {
        section = new ConfSection(sectionName);
        sections.insert(std::pair<std::string, ConfSection *>(key, section));
    }

    section->append(key, value);
    return true;
}

bool Configuration::loadConfiguration() {
    if (!this->configFile.is_open())
        this->configFile.open(this->configPath.c_str(), std::ios::in);

    if (!this->configFile.is_open())
        return false;

    clean();

    std::string section = "global";
    std::string bufferLine;

    while (getline(this->configFile, bufferLine)) {

        if (*bufferLine.c_str() == '[')
            parseSection(bufferLine, &section);
        else if (*bufferLine.c_str() != '#')
            parseValue(section, bufferLine);

        bufferLine.clear();
    }

    if (this->configFile.is_open())
        this->configFile.close();

    return true;
}

ConfSection *Configuration::getSection(std::string name) {
    std::map<std::string, ConfSection *>::iterator it;

    for (it = sections.begin(); it != sections.end(); it++) {
        if (name.compare((*it).second->getName()) == 0)
            return (*it).second;
    }

    return NULL;
}

bool Configuration::parseSection(std::string input, std::string *section) {
    std::stringstream ss(std::stringstream::in | std::stringstream::out);

    int nameSize = 0;
    char dummy0;
    std::string name;

    ss << input;
    ss >> dummy0 >> name;

    if (ss.fail())
        return false;
    nameSize = name.size();

    if (dummy0 != '[' || name.at(nameSize - 1) != ']')
        return false;

    name = name.substr(0, nameSize - 1);

    std::cout << "[D] SECTION -> name = " << name << std::endl;

    section->clear();
    section->append(name);
    return true;
}

bool Configuration::parseValue(std::string sectionName, std::string input) {
    std::string buffer;
    std::stringstream ss(std::stringstream::in | std::stringstream::out);
    ss << input;

    getSection(sectionName);

    if (std::getline(ss, buffer, '=').fail())
        return false;
    std::string key = buffer;

    if (std::getline(ss, buffer, '=').fail())
        return false;
    std::string value = buffer;

    std::cout << "[D] SECTION = (" << sectionName << ") :: VALUE -> key = "
              << key << " | value = " << value << std::endl;

    appendToSection(key, value, sectionName);
    return true;
}

bool Configuration::clean() {
    if (!this->sections.empty()) {

        std::map<std::string, ConfSection *>::iterator it;

        for (it = sections.begin(); it != sections.end(); it++)
            delete (*it).second;
        sections.clear();
        return true;
    }
    return false;
}

Configuration::~Configuration() {
    clean();

    if (this->configFile.is_open())
        this->configFile.close();
}
}
