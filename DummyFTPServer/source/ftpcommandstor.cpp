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

#include "ftpcommandstor.h"
#include "ftpserver.h"

namespace DummyFTPServer {

FTPCommandSTOR::FTPCommandSTOR(const QString &fileName, bool append, ServerThread *parent)
    : FTPCommand(fileName, parent) {

    isConnected = true;

    if (!append)
        file.open(QFile::WriteOnly);
    else
        file.open(QFile::WriteOnly | QFile::Append);

    if (file.exists() && file.isOpen() && file.isWritable()) {

        connect(stParent->passvSocket, SIGNAL(readyRead()),
                this, SLOT(sendData()));
        connect(stParent->passvSocket, SIGNAL(error(QAbstractSocket::SocketError)),
                this, SLOT(deleteLater()));
        connect(stParent->passvSocket, SIGNAL(disconnected()), this, SLOT(disconnected()));

        stParent->sendMessage("150", "File status OK, about to open data connection.");
    } else {
        stParent->stopDTP();
        isConnected = false;
        deleteLater();

        stParent->sendMessage("451", "Requested action aborted. Local error in processing.");
    }
}

void FTPCommandSTOR::disconnected() {
    disconnect(stParent->passvSocket, 0, this, 0);

    file.flush();
    file.close();

    isConnected = false;
    stParent->sendMessage("250", "Transfer complete.");
}

void FTPCommandSTOR::sendData() {
    if (!isConnected)
        return;

    file.write(stParent->passvSocket->readAll());

    if (stParent->mode != ServerThread::STOR && stParent->mode != ServerThread::APPE) {
        stParent->sendMessage("451", "Requested action aborted. Local error in processing.");
        stParent->stopDTP();

        deleteLater();
        isConnected = false;
    }
}
}
