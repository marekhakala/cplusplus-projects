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

#include "serverthread.h"
#include "ftpserver.h"
#include "config.h"

#include "ftpcommandlist.h"
#include "ftpcommandretr.h"
#include "ftpcommandstor.h"

namespace DummyFTPServer {

ServerThread::ServerThread(QTcpSocket *socket, QObject *parent)
    : QObject(parent) {

    isLogged = false;
    flagWaitForDTP = false;
    this->socket = socket;
    mode = NONE;
    loggedUser = NULL;
    passvSocket = NULL;
    pathPtr = NULL;

    listCommand = NULL;
    retrCommand = NULL;
    storCommand = NULL;

    server = new QTcpServer(this);
    server->listen(QHostAddress::Any);

    transferContinueFrom = 0;
    connect(server, SIGNAL(newConnection()), this, SLOT(pasvConnection()));

    if(socket->state() == QTcpSocket::ConnectedState)
        welcomeMessage();
    else
        connect(socket, SIGNAL(connected()), this, SLOT(welcomeMessage()));

    connect(socket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(deleteLater()));
    connect(socket, SIGNAL(disconnected()), this, SLOT(deleteLater()));
    connect(socket, SIGNAL(readyRead()), this, SLOT(parseCommand()));

    connect(this, SIGNAL(connectedDTP()), this, SLOT(parseCommand()));
}

ServerThread::~ServerThread() {
    std::cout << "[-] Disconnect" << std::endl;

    if(socket)
        delete socket;
    if(passvSocket)
        delete passvSocket;
    if(server)
        delete server;
    if(pathPtr)
        delete pathPtr;

    if(listCommand)
        delete listCommand;
    if(retrCommand)
        delete retrCommand;
    if(storCommand)
        delete storCommand;
}

void ServerThread::welcomeMessage() {
    sendMessage("220", QString("%1 (%2-%3)").arg(WELCOME_MESSAGE).arg(APP_NAME).arg(VERSION));
}

void ServerThread::parseCommand() {
    QByteArray line;

    if(!socket || !this->isConnected() || !socket->canReadLine())
        return;

    while(socket->canReadLine()) {
        QString command;
        QByteArray code;
        QList<QByteArray> commandList;

        if(flagWaitForDTP || (line = socket->readLine().trimmed()).isEmpty())
            return;

        commandList = line.split(' ');
        commandList[0] = commandList[0].toLower();

        code = commandList[0];
        command = line.right(line.length()-code.length()).trimmed();

        if(code == "user") {
            if (commandList.length() > 1 && (loggedUser = Core::Instance()->getUser(command.toStdString()))) {
                validUsername = true;
                pathPtr = new WorkPath(QString::fromStdString(Core::Instance()->getUser(command.toStdString())->getHomeDirectory()));

                sendMessage("331", "User name OK, need password.");
            } else if(Core::Instance()->isEnabledAnonymousAccess() && command == "anonymous") {
                pathPtr = new WorkPath(Core::Instance()->getAnonymousHome());

                validUsername = validPassword = isLogged = true;
                sendMessage("331", "Anonymous login ok, send your complete email address as your password.");
            } else {
                pathPtr = NULL;
                validUsername = validPassword = isLogged = false;
                sendMessage("332", "Need account for login.");
            }
        } else if(code == "pass") {

            if(validUsername) {
                if(validPassword && !loggedUser && Core::Instance()->isEnabledAnonymousAccess()) {
                    sendMessage("230", "Anonymous access granted, restrictions apply.");
                    isLogged = true;
                } else if(commandList.length() > 1 && loggedUser->checkPassword(QString(commandList[1]).toStdString(), AccountManager::PLAIN)) {
                    sendMessage("230", "User logged in, proceed.");
                    isLogged = true;
                } else {
                    sendMessage("530", "Not logged in.");
                }
            } else {
                sendMessage("530", "Not logged in.");
            }

        } else if(code == "noop") {
            sendMessage("200", "NOOP command successful.");
        } else if(code == "opts") {
            sendMessage("200", "OPTS command successful."); // FAKE
        } else if(isLogged) {

            if(code == "pwd") {
                sendMessage("257", "\"" + pathPtr->pwd() + "\" is the current directory.");
            } else if (code == "rnfr") {
                rnfrNameCache = command;
                sendMessage("350", "Requested file action pending further information.");
            } else if (code == "rnto") {

                if((!rnfrNameCache.isEmpty() && !command.isEmpty())
                        && pathPtr->rename(rnfrNameCache, command))
                    sendMessage("250", "Requested file action OK, completed.");
                else
                    sendMessage("550", "Requested action not taken.");

                rnfrNameCache.clear();
            } else if(code == "cwd") {

                if(commandList.length() > 1 && pathPtr->cwd(command))
                    sendMessage("250", "Requested file action OK, completed.");
                else if(commandList.length() == 1 && pathPtr->cwd())
                    sendMessage("250", "Requested file action OK, completed.");
                else
                    sendMessage("550", "Requested action not taken.");

            } else if(code == "cdup") {

                if(pathPtr->cdUp())
                    sendMessage("200", "Command OK.");
                else
                    sendMessage("550", "Requested action not taken.");

            } else if(code == "rmd") {

                if(commandList.length() > 1 && pathPtr->rmd(command))
                    sendMessage("250", "Requested file action OK, completed.");
                else
                    sendMessage("550", "Requested action not taken.");

            } else if(code == "dele") {

                if(commandList.length() > 1 && pathPtr->dele(command))
                    sendMessage("250", "Requested file action OK, completed.");
                else
                    sendMessage("550", "Requested action not taken.");
            } else if(code == "mkd") {
                if(commandList.length() > 0 && pathPtr->mkd(command))
                    sendMessage("257", "directory \"" + command + "\" created.");
                else
                    sendMessage("550", "Requested action not taken.");
            } else if(code == "quit") {
                isLogged = false;
                validUsername = false;
                validPassword = false;
                loggedUser = NULL;

                sendMessage("221", "Service closing control connection.");

                socket->close();
            } else if(code == "syst") {
                sendMessage("215", "UNIX Type: L8");
            } else if(code == "type") {
                sendMessage("200", "Command OK."); // FAKE
            } else if(code == "port") {

                if(commandList.length() > 1) {
                    QList<QString> addressList;
                    addressList = command.split(',');

                    if(addressList.length() != 6) {
                        sendMessage("501", "Syntax error in parameters or arguments.");
                    } else {
                        int port;
                        quint32 ip4;

                        ip4 = ((addressList.at(0).toUInt() << 24) | (addressList.at(1).toUInt() << 16)
                               | (addressList.at(2).toUInt() << 8) | addressList.at(3).toUInt());

                        port = ((addressList.at(4).toUInt() << 8) | addressList.at(5).toUInt());

                        stopDTP();

                        if(passvSocket)
                            delete passvSocket;

                        passvSocket = new QTcpSocket(this);

                        QHostAddress socketAddress(ip4);
                        passvSocket->connectToHost(socketAddress, port);

                        connect(passvSocket, SIGNAL(connected()),
                                this, SLOT(portConnection()));
                        connect(passvSocket, SIGNAL(error(QAbstractSocket::SocketError)),
                                this, SLOT(errorDTP()));

                        sendMessage("200", "Command OK.");
                    }
                } else {
                    sendMessage("501", "Syntax error in parameters or arguments.");
                }
            } else if(code == "pasv") {
                // Firefox (bug)
                openPassive();
            } else if(code == "list") {

                globalCommand = command;

                if(passvSocket) {
                    list();
                } else {
                    connect(this, SIGNAL(connectedDTP()), this, SLOT(list()));
                    openPassive();
                }
            } else if(code == "nlst") {

                globalCommand = command;

                if(passvSocket) {
                    list();
                } else {
                    connect(this, SIGNAL(connectedDTP()), this, SLOT(list()));
                    openPassive();
                }
            } else if(code == "rest") {
                sendMessage("350", "Requested file action pending further information.");
                transferContinueFrom = command.toLongLong();

            } else if(code == "retr") {
                if(mode != NONE) {
                    sendMessage("550", "Requested action not taken.");
                } else {
                    if(commandList.length() == 1) {
                        sendMessage("500", "'RETR' not understood");
                    } else {
                        QFileInfo file(pathPtr->realFilePath(command));

                        if(!file.isFile()) {
                            mode = RETR; // FIREFOX HACK
                            sendMessage("550", "Not a regular file.");
                        } else {
                            mode = RETR;
                            fileName = command;

                            if(retrCommand)
                                delete retrCommand;

                            retrCommand = new FTPCommandRETR(pathPtr->realFilePath(command), this);
                        }
                    }
                }
            } else if(code == "stor") {
                if(mode != NONE) {
                    sendMessage("550", "Requested action not taken.");
                } else {
                    if(commandList.length() == 1) {
                        sendMessage("550", "'STOR' not understood");
                    } else {
                        if(pathPtr->isWritable(pathPtr->realFilePath(command))) {
                            mode = STOR;
                            fileName = command;
                            if(storCommand)
                                delete storCommand;

                            storCommand = new FTPCommandSTOR(pathPtr->realFilePath(command), false, this);
                        } else {
                            sendMessage("451", "Requested action aborted. Local error in processing.");
                        }
                    }
                }
            } else if(code == "appe") {
                if(mode != NONE)
                    sendMessage("550", "Requested action not taken.");
                else {
                    if(commandList.length() == 1) {
                        sendMessage("500", "'APPE' not understood.");
                    } else {
                        if(pathPtr->isWritable(pathPtr->realFilePath(command))) {
                            mode = APPE;
                            fileName = command;
                            if(storCommand)
                                delete storCommand;

                            storCommand = new FTPCommandSTOR(pathPtr->realFilePath(command), true, this);
                        } else {
                            sendMessage("451", "Requested action aborted. Local error in processing.");
                        }
                    }
                }
            } else if(code == "size") {
                if(commandList.length() == 1) {
                    sendMessage("500", "'SIZE' not understood.");
                } else {
                    QFileInfo file(pathPtr->realFilePath(command));

                    if(!file.isFile())
                        sendMessage("550", "Not a regular file.");
                    else if(file.exists())
                        sendMessage("213", QString::number(file.size()));
                    else
                        sendMessage("451", "Requested action aborted. Local error in processing.");
                }
            } else if(code == "mdtm") {
                if(commandList.length() == 1) {
                    sendMessage("500", "'MDTM' not understood");
                } else {
                    QFileInfo file(pathPtr->realFilePath(command));

                    if(!file.isFile())
                        sendMessage("550", "Not a plain file.");
                    else if(file.exists())
                        sendMessage("213", file.lastModified().toString("yyyyMMddhhmmss"));
                    else
                        sendMessage("451", "Requested action aborted. Local error in processing.");
                }
            } else if(code == "abor") {
                if(mode != NONE) {
                    mode = NONE;
                    sendMessage("226", "Closing data connection.");
                } else if(passvSocket && passvSocket->isValid() &&
                          (passvSocket->state() == QTcpSocket::ConnectedState)) {
                    sendMessage("225", "Data connection open; no transfer in progress.");
                } else {
                    stopDTP();
                    sendMessage("226", "Closing data connection.");
                }
            } else if(code == "hello") {
                sendMessage("555", "Hello FTP world !!!.");
            } else {
                sendMessage("502", "Command not implemented.");
            }
        } else {
            sendMessage("550", "Requested action not taken.");
        }
    }
}

bool ServerThread::sendMessage(const QString &commandCode, const QString &message) {
    if(!socket || !socket->isValid())
        return false;

    socket->write(commandCode.toAscii() + " " + message.toAscii() + "\r\n");
    return true;
}

void ServerThread::openPassive() {
    int tcpPort;
    quint32 tcpAddress;
    QString message;

    QList<int> address;

    stopDTP();

    tcpAddress = socket->localAddress().toIPv4Address();
    tcpPort = server->serverPort();

    address.push_back(tcpAddress >> 24);
    address.push_back((tcpAddress >> 16) & 0xFF);
    address.push_back((tcpAddress >> 8) & 0xFF);
    address.push_back(tcpAddress & 0xFF);

    address.push_back((tcpPort >> 8) & 0xFF);
    address.push_back(tcpPort & 0xFF);

    message = QString("Entering Passive Mode (%1,%2,%3,%4,%5,%6).")
              .arg(address.at(0))
              .arg(address.at(1))
              .arg(address.at(2))
              .arg(address.at(3))
              .arg(address.at(4))
              .arg(address.at(5));

    sendMessage("227", message);
    flagWaitForDTP = true;
}

void ServerThread::stopDTP() {
    if (passvSocket) {
        passvSocket->close();
        passvSocket->deleteLater();
        passvSocket = NULL;
    }

    mode = NONE;

    if (flagWaitForDTP) {
        flagWaitForDTP = false;
        emit connectedDTP();
    }
}

void ServerThread::errorDTP() {
    stopDTP();
}

void ServerThread::portConnection() {
    flagWaitForDTP = false;
    emit connectedDTP();
}

void ServerThread::pasvConnection() {
    if (passvSocket) {
        passvSocket->deleteLater();
        passvSocket = NULL;
    }

    passvSocket = server->nextPendingConnection();
    flagWaitForDTP = false;
    emit connectedDTP();
}

void ServerThread::list() {
    disconnect(this, SLOT(list()));

    if (passvSocket) {
        if(listCommand)
            delete listCommand;

        listCommand = new FTPCommandLIST(this);
    } else {
        sendMessage("426", "Connection closed; transfer aborted.");
        stopDTP();
    }
}
}
