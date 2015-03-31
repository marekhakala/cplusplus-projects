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

#include "ftpcommandlist.h"

namespace DummyFTPServer {

FTPCommandLIST::FTPCommandLIST(ServerThread *parent)
    : FTPCommand(parent) {

    isConnected = true;
    parent->pathPtr->list(list);
    parent->sendMessage("150", "Opening ASCII mode data connection for file list.");

    connect(parent->passvSocket, SIGNAL(disconnected()), this, SLOT(error()));
    connect(parent->passvSocket, SIGNAL(error(QAbstractSocket::SocketError)), this, SLOT(error()));
    connect(parent->passvSocket, SIGNAL(bytesWritten(qint64)), this, SLOT(sendData()));

    if (parent->passvSocket->state() == QTcpSocket::ConnectedState)
        sendData();
    else
        connect(parent->passvSocket, SIGNAL(connected()), this, SLOT(sendData()));
}

void FTPCommandLIST::sendData() {
    if (!isConnected) return;

    if (stParent->mode != ServerThread::NONE) {
        finishing();
        return;
    }

    if (stParent->passvSocket->bytesToWrite() > 0)
        return;

    if (list.isEmpty()) {
        finishing();
        return;
    }

    QByteArray data = list.takeFirst() + "\r\n";
    stParent->passvSocket->write(data);

}

void FTPCommandLIST::error(bool stop) {

    disconnect(stParent->passvSocket, 0, this, 0);
    list.clear();

    if (stop)
        stParent->stopDTP();
    else
        stParent->mode = ServerThread::NONE;

    stParent->sendMessage("451", "Requested action aborted. Local error in processing.");

    emit finished();

    isConnected = false;
    deleteLater();
}

void FTPCommandLIST::finishing() {
    disconnect(stParent->passvSocket, 0, this, 0);
    list.clear();

    stParent->stopDTP();
    stParent->sendMessage("226", "Transfer complete");

    emit finished();

    isConnected = false;
    deleteLater();
}
}
