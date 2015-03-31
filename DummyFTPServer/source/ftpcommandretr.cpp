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
#include "ftpcommandretr.h"

namespace DummyFTPServer {

FTPCommandRETR::FTPCommandRETR(const QString &fileName, ServerThread *parent)
    : FTPCommand(fileName, parent) {

    file.open(QFile::ReadOnly);
    isConnected = true;

    if (file.exists() && file.isOpen() && file.isReadable()) {
        file.seek(stParent->transferContinueFrom);
        stParent->sendMessage("150", "Opening BINARY mode data connection for "
                              + fileName + " (" + QString::number(file.size()) + " bytes)");

        connect(stParent->passvSocket, SIGNAL(bytesWritten(qint64)),
                this, SLOT(sendData()));
        connect(stParent->passvSocket, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(error()));
        connect(stParent->passvSocket, SIGNAL(disconnected()),
                this, SLOT(error()));

        if (stParent->passvSocket->state() == QTcpSocket::ConnectedState)
            sendData();
        else
            connect(stParent->passvSocket, SIGNAL(connected()), this, SLOT(sendData()));

    } else if (!file.exists()) {
        stParent->sendMessage("550", "Requested action not taken.");
    } else {
        error(false);
    }
}

void FTPCommandRETR::sendData() {
    QByteArray data;

    if (!isConnected)
        return;

    if (stParent->mode != ServerThread::RETR) {
        finishing();
        return;
    } else if (stParent->passvSocket->bytesToWrite() > 0)
        return;

    data = file.read(BUFFER_SIZE);

    if (data.isEmpty()) {
        finishing();
        return;
    }

    stParent->passvSocket->write(data);
}

void FTPCommandRETR::error(bool stop) {
    disconnect(stParent->passvSocket, 0, this, 0);

    file.close();

    if (stop)
        stParent->stopDTP();
    else
        stParent->mode = ServerThread::NONE;

    emit finished();

    deleteLater();
    isConnected = false;

    stParent->sendMessage("451", "Requested action aborted. Local error in processing.");
}
}
