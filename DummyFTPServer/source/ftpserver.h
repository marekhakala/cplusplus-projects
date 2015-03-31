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

#ifndef FTPSERVER_H
#define FTPSERVER_H

#include <QList>
#include <QObject>
#include <QTcpServer>

#include "serverthread.h"

namespace DummyFTPServer {

class FTPServer : public QObject {
    Q_OBJECT

public:
    FTPServer(unsigned int tcpPort, QObject *parent = 0);
    static unsigned int getPort() {
        return port;
    }
    ~FTPServer();

    static void start(unsigned int tcpPort);
    static void stop();
    static void restart();
    static bool isRunning();

public slots:
    void addConnection();

signals:
    void closing();

private:
    bool clean();

    static FTPServer *pInstance;
    QTcpServer *socket;
    QList<QObject *> threadList;
    static unsigned int port;
};
}

#endif // FTPSERVER_H
