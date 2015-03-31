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

#include <QDateTime>
#include <QDebug>
#include <iostream>

#include "workpath.h"

namespace DummyFTPServer {

QString WorkPath::getOwner(const QFileInfo &info) {

    if(info.owner().isEmpty()) {
        if(info.ownerId() != uint(-2))
            return QString::number(info.ownerId());
        return QString("owner");
    }

    return info.owner();
}

QString WorkPath::getGroup(const QFileInfo &info) {
    if(info.group().isEmpty()) {
        if(info.groupId() != uint(-2))
            return QString::number(info.groupId());
        return QString("group");
    }
    return info.group();
}

WorkPath::WorkPath(const QString &path) {
    homePath = path;
    currentPath = "/";
}

QString WorkPath::realPath() {
    if(homePath.at(homePath.length()-1) != '/' && currentPath.at(0) != '/')
        return homePath + "/" + currentPath;
    return homePath + currentPath;
}

QString WorkPath::realFilePath(QString filename) {

    if(filename.at(0) == '/') {
        if(homePath.at(homePath.length()-1) != '/')
            return homePath + filename;
        else
            return (homePath + filename.right(filename.length()-1));
    }

    if(realPath().at(realPath().length()-1) != '/' && filename.at(0) != '/')
        return realPath() + "/" + filename;
    return realPath() + filename;
}

QString WorkPath::pwd() {
    return currentPath;
}

bool WorkPath::cwd(const QString &path) {
    QString realPath;
    QString newPath = path;

    if(newPath == "")
        newPath = "/";

    if(newPath.at(0) != '/') {
        if(currentPath.at(currentPath.length()-1) != '/')
            newPath = currentPath + "/" + newPath;
        else
            newPath = currentPath + newPath;
    } else if(newPath != "/") {
        newPath = path;
    }

    if(homePath.at(homePath.length()-1) != '/' && newPath.at(0) != '/')
        realPath = homePath + "/" + newPath;
    realPath = homePath + newPath;

    QDir directory(realPath);

    if(directory.exists()) {
        currentPath = newPath;
        return true;
    }

    return false;
}

bool WorkPath::mkd(const QString &name) {
    QString n = name;

    if(name.at(0) == '/') {
        cwd(name.left(name.lastIndexOf("/")));
        n =  name.right(name.length() - (name.lastIndexOf("/")+1));
    }

    QDir directory(realPath());

    return directory.mkdir(n);
}

bool WorkPath::rmd(const QString &name) {
    QString n = name;
    QDir directory(realPath());

    if(name.at(name.length()-1) == '/')
        n = name.left(name.length() - 1);

    if(n.contains('/'))
        n =  n.right(n.length() - (n.lastIndexOf("/")+1));

    if(!directory.exists())
        return false;

    if(directory.rmdir(n))
        return true;
    return false;
}

bool WorkPath::dele(const QString &name) {
    QFile file(realFilePath(name));
    return file.remove();
}

bool WorkPath::cdUp() {
    if (currentPath == "/")
        return false;

    if(currentPath.at(0) != '/')
        currentPath = "/" + currentPath;

    QString newPath = currentPath.left(currentPath.lastIndexOf("/"));

    if(newPath.isEmpty()) newPath = "/";

    currentPath = newPath;
    return true;
}

bool WorkPath::rename(const QString &from, const QString &to) {
    QString o = from;
    QString n = to;

    if(from.at(0) == '/') {
        cwd(from.left(from.lastIndexOf("/")));
        o =  from.right(from.length() - (from.lastIndexOf("/")+1));
    }

    if(n.contains('/'))
        n =  to.right(to.length() - (to.lastIndexOf("/")+1));

    QDir directory(realPath());
    return directory.rename(o, n);
}

bool WorkPath::isWritable(const QString &filename) {
    // TODO
    QFileInfo file(filename);
    return true;
}

void WorkPath::list(QList<QByteArray> &sList) {
    QDir directory(this->realPath());
    QFileInfoList fileList = directory.entryInfoList(QDir::AllEntries, QDir::Name | QDir::DirsFirst | QDir::IgnoreCase);

    QString currentYear = QDateTime::currentDateTime().toString("yyyy");
    sList.clear();

    int maxSizeLength(0);
    int maxGroupLength(0);
    int maxOwnerLength(0);

    foreach(QFileInfo info, fileList) {
        maxSizeLength = qMax(maxSizeLength, QString::number(info.size()).length());
        maxOwnerLength = qMax(maxOwnerLength, getOwner(info).length());
        maxGroupLength = qMax(maxGroupLength, getGroup(info).length());
    }

    foreach(QFileInfo info, fileList) {
        QByteArray line;

        QString owner = getOwner(info);
        QString group = getGroup(info);
        QString size = QString::number(info.size());

        const char *cmonth[] = { "Jan", "Feb", "Mar", "Apr", "May", "Jun", "Jul", "Aug", "Sep", "Oct", "Nov", "Dec" };
        int monthID = qMin(qMax(info.created().toString("M").toInt() - 1, 0), 11);

        QString month = cmonth[monthID];
        QString day = info.created().toString("d");
        QString year = info.created().toString("yyyy") == currentYear ? info.created().toString("hh:mm") : info.created().toString("yyyy");

        line += info.isDir() ? "d" : "-";

        line += (info.permissions() & QFile::ReadOwner) ? "r" : "-";
        line += (info.permissions() & QFile::WriteOwner) ? "w" : "-";
        line += (info.permissions() & QFile::ExeOwner) ? "x" : "-";

        line += (info.permissions() & QFile::ReadGroup) ? "r" : "-";
        line += (info.permissions() & QFile::WriteGroup) ? "w" : "-";
        line += (info.permissions() & QFile::ExeGroup) ? "x" : "-";

        line += (info.permissions() & QFile::ReadOther) ? "r" : "-";
        line += (info.permissions() & QFile::WriteOther) ? "w" : "-";
        line += (info.permissions() & QFile::ExeOther) ? "x" : "-";

        line += "   1 ";
        line += owner;
        line += " ";

        line += QString(maxOwnerLength - owner.length(), ' ');
        line += group;
        line += QString(maxGroupLength + maxSizeLength + 1 - group.length() - size.length(), ' ');
        line += size;
        line += " ";

        line += month;
        line += QString(6 - day.length(), ' ');
        line += day;
        line += QString(6 - year.length(), ' ');
        line += year;
        line += " ";

        line += info.fileName().toAscii();
        sList << line;
    }
}

}
