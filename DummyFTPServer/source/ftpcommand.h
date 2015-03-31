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

#ifndef FTPCOMMAND_H
#define FTPCOMMAND_H

#include <QObject>

#include <QFile>
#include <QString>

#include "serverthread.h"

using DummyFTPServer::ServerThread;

namespace DummyFTPServer {

class FTPCommand : public QObject {

    Q_OBJECT

public:
    FTPCommand(ServerThread *parent = 0);
    FTPCommand(const QString &fileName, ServerThread *parent = 0);

    virtual ~FTPCommand() {}
    virtual ServerThread *parent() const;

protected:
    bool isConnected;
    QFile file;
    ServerThread *stParent;

public slots:
    virtual void sendData() = 0;
    virtual void finishing();
    virtual void error(bool stop = true);

signals:
    void finished();
};
}

#endif // FTPCOMMAND_H
