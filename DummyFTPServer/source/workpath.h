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

#ifndef FILEPATH_H
#define FILEPATH_H

#include <QDir>
#include <QByteArray>
#include <QObject>

namespace DummyFTPServer {

class WorkPath {
public:
    WorkPath(const QString &path);

    QString realPath();
    QString realFilePath(QString filename);

    bool cdUp();
    QString pwd();
    bool cwd(const QString &path);
    bool cwd() {
        return cwd(homePath);
    }

    bool mkd(const QString &name);
    bool rmd(const QString &name);
    bool dele(const QString &name);
    bool rename(const QString &fromFileName, const QString &toFileName);
    bool isWritable(const QString &filename);

    void list(QList<QByteArray> &sList);

    static QString getOwner(const QFileInfo &info);
    static QString getGroup(const QFileInfo &info);

private:
    QString homePath;
    QString currentPath;
};
}

#endif // FILEPATH_H
