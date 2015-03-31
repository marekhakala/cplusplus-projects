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

#ifndef LISTENERTHREAD_H
#define LISTENERTHREAD_H

#include <iostream>
#include <iomanip>
#include <list>

#include <QObject>
#include <QTcpServer>
#include <QTcpSocket>
#include <QHostAddress>
#include <QDateTime>
#include <QPointer>

#include "core.h"
#include "user.h"
#include "workpath.h"
#include "passwordtype.h"

using DummyFTPServer::WorkPath;
using DummyFTPServer::AccountManager::User;
using DummyFTPServer::AccountManager::PasswordType;

namespace DummyFTPServer {

class ServerThread : public QObject {
    Q_OBJECT

    friend class FTPCommand;
    friend class FTPCommandLIST;
    friend class FTPCommandRETR;
    friend class FTPCommandSTOR;

public:
    ServerThread(QTcpSocket *_socket, QObject *parent = 0);
    virtual ~ServerThread();

    enum MODE_DTP {
        NONE, STOR, RETR, APPE
    };

    bool isConnected() {
        if (socket && ((socket->state() == QTcpSocket::ConnectingState)
                       || (socket->state() == QTcpSocket::ConnectedState)))
            return true;
        return false;
    }

    MODE_DTP getMode() {
        return this->mode;
    }

    User *getUser() {
        return loggedUser;
    }

    std::string getFileName() {
        return fileName.toStdString();
    }

    std::string getAddress() {
        return (socket->peerAddress().toString()).toStdString();
    }

signals:
    void connectedDTP();

public slots:
    void parseCommand();
    void list();
    void stopDTP();
    void errorDTP();
    void portConnection();
    void pasvConnection();
    void welcomeMessage();

protected:
    bool sendMessage(const QString &commandCode, const QString &message);
    void openPassive();

private:
    bool validUsername;
    bool validPassword;
    bool isLogged;
    bool flagWaitForDTP;

    QString fileName;
    QString globalCommand;
    QString rnfrNameCache;

    QPointer<QTcpSocket> socket;
    QPointer<QTcpSocket> passvSocket;
    QPointer<QTcpServer> server;

    WorkPath *pathPtr;
    User *loggedUser;
    MODE_DTP mode;

    QPointer<QObject> listCommand;
    QPointer<QObject> retrCommand;
    QPointer<QObject> storCommand;

    quint64 transferContinueFrom;
};

}

#endif // LISTENERTHREAD_H
