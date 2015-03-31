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

#include "ftpserver.h"

namespace DummyFTPServer {

FTPServer *FTPServer::pInstance = NULL;
unsigned int FTPServer::port = 0;

void FTPServer::start(unsigned int tcpPort) {
    if (pInstance)
        delete pInstance;
    pInstance = new FTPServer(tcpPort);
}

void FTPServer::stop() {
    if (pInstance)
        delete pInstance;
    pInstance = NULL;
}

void FTPServer::restart() {
    stop();
    start(FTPServer::getPort());
}

bool FTPServer::isRunning() {
    if (pInstance != NULL)
        return true;
    return false;
}

FTPServer::FTPServer(unsigned int tcpPort, QObject *parent) : QObject(parent) {
    pInstance = this;

    port = tcpPort;
    socket = new QTcpServer(this);
    if (!socket->listen(QHostAddress::Any, tcpPort))
        throw ErrorException(QString("Can't bind to port %1.").arg(tcpPort).toStdString(), true);

    connect(socket, SIGNAL(newConnection()), this, SLOT(addConnection()));
}

bool FTPServer::clean() {
    if (!this->threadList.empty()) {

        QList<QObject *>::iterator it;
        for (it = threadList.begin(); it != threadList.end(); it++)
            delete (*it);

        threadList.clear();
        return true;
    }
    return false;
}

FTPServer::~FTPServer() {
    emit closing();

    clean();
    delete socket;
    pInstance = NULL;
}

void FTPServer::addConnection() {
    while (this->socket->hasPendingConnections()) {
        if (QTcpSocket * tcpSocket = this->socket->nextPendingConnection()) {
            std::cout << "[+] Incoming connection from "
                      << "(" << tcpSocket->peerAddress().toString().toStdString() << ")"
                      << " Port " << tcpSocket->peerPort() << "." << std::endl;

            threadList.append(new ServerThread(tcpSocket, this));
        }
    }
}
}
