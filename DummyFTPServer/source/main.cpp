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

#include <QCoreApplication>

#include <iostream>
#include <iomanip>
#include <exception>

#include <cstdlib>
#include <csignal>

#include "core.h"
#include "errorexception.h"
#include "config.h"

/* Using my namespace DummyFTPServer */
using DummyFTPServer::Core;
using DummyFTPServer::ErrorException;

/* Functions declaration */
void signalHandler(int);
void unsupportedOptions(char *argv, const std::string &usage);

/* Main function */
int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    /* File Path */
    std::string configFile;
    std::string userListFile;

    /* Handlering system signals */
    signal(SIGTERM, signalHandler);
    signal(SIGINT, signalHandler);

    /* Usage help */
    std::string usage = "\n    Usage:  dfsd -c <file> -u <userfile> -v --help\n\n"
                        "    -h, --help      :   Show this help\n"
                        "    -v              :   Show version\n"
                        "    -c <file>       :   Set config file\n"
                        "    -u <userfile>   :   Set user list file\n\n";

    std::cout << "[+] DummyFTPServer v. " << VERSION << "\n";

    /* Parsing input args */
    for (int i = 0; i < argc; i++) {
        if ((std::string(argv[i])).compare("--help") == 0
                || (std::string(argv[i])).compare("-h") == 0) {
            std::cout << usage;
            return 1;
        }

        if (argv[i][0] == '-')
            switch (argv[i][1]) {
            case 'v': /* Version */
                if (argv[i][2] == '\0') {
                    std::cout << "\n    Version: " << VERSION << "\n\n";
                    return 0;
                } else {
                    unsupportedOptions(argv[i], usage);
                    return 1;
                }
                break;

            case 'c': /* Set config file */
                if (argv[i][2] == '\0') {
                    configFile = std::string(argv[++i]);
                    std::cout << "[+] Set config file : " << configFile << std::endl;
                } else {
                    unsupportedOptions(argv[i], usage);
                    return 1;
                }
                break;

            case 'u': /* Set user list file */
                if (argv[i][2] == '\0') {
                    userListFile = std::string(argv[++i]);
                    std::cout << "[+] Set user list file : " << userListFile << std::endl;
                } else {
                    unsupportedOptions(argv[i], usage);
                    return 1;
                }
                break;

            default: /* Unsupported options */
                unsupportedOptions(argv[i], usage);
                return 1;
                break;
            }
    }

    try {
        Core *core = Core::Instance();

        if (configFile.empty())
            configFile = DEFAULT_CONF_PATH;

        if (userListFile.empty())
            userListFile = DEFAULT_USERLIST_FILE;

        core->loadConfiguration(configFile, userListFile);
        std::cout << "[+] Starting server ..." << std::endl;
        core->startServer();

    } catch (ErrorException &ex) {
        std::cout << ex.what() << std::endl;

// WIN32 support (deprecated)
#ifdef WIN32
        system("pause");
#endif

        if (ex.isCritical())
            return 0;
    }

    return app.exec();
}

void unsupportedOptions(char *argv, const std::string &usage) {
    std::cout << "\n      Unsupported option : " << argv << "\n" << usage << std::endl;
    exit(0);
}

void signalHandler(int) {
    std::cout << "[-] Stopping, please wait ...\n";
    exit(0);
}
