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

#include "ftpcommand.h"

using DummyFTPServer::ServerThread;

namespace DummyFTPServer {

FTPCommand::FTPCommand(ServerThread *parent) : QObject(parent) {
    this->stParent = parent;
    isConnected = true;
}

FTPCommand::FTPCommand(const QString &fileName, ServerThread *parent)
    : QObject(parent),
      file(fileName) {
    this->stParent = parent;
    isConnected = true;
}

ServerThread *FTPCommand::parent() const {
    return stParent;
}

void FTPCommand::finishing() {
    disconnect(stParent->passvSocket, 0, this, 0);
    file.close();

    stParent->stopDTP();
    stParent->sendMessage("226", "Transfer complete.");

    emit finished();

    deleteLater();
    isConnected = false;
}

void FTPCommand::error(bool stop) {
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
