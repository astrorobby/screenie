/* This file is part of the Screenie project.
   Screenie is a fancy screenshot composer.

   Copyright (C) 2008 Ariya Hidayat <ariya.hidayat@gmail.com>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2, or (at your option)
   any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License along
   with this program; if not, write to the Free Software Foundation,
   Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <QtCore/QList>
#include <QtCore/QString>
#include <QtCore/QVariant>
#include <QtCore/QSignalMapper>
#include <QtGui/QMainWindow>
#include <QtGui/QAction>
#include <QtGui/QActionGroup>

#include "../../Model/src/ScreenieScene.h"
#include "DocumentInfo.h"
#include "DocumentManager.h"

class DocumentManagerPrivate
{
public:
    DocumentManagerPrivate()
        : windowActionGroup(new QActionGroup(0))
    {}

    ~DocumentManagerPrivate()
    {
        delete windowActionGroup;
    }

    QList<const DocumentInfo *> documentInfos;
    QActionGroup *windowActionGroup;
    QSignalMapper windowMapper;
    static DocumentManager *instance;
    static int nextWindowId;
};

DocumentManager *DocumentManagerPrivate::instance = 0;
int DocumentManagerPrivate::nextWindowId = 0;

// public

DocumentManager &DocumentManager::getInstance()
{
    if (DocumentManagerPrivate::instance == 0) {
        DocumentManagerPrivate::instance = new DocumentManager();
    }
    return *DocumentManagerPrivate::instance;
}

void DocumentManager::destroyInstance()
{
    if (DocumentManagerPrivate::instance != 0) {
        delete DocumentManagerPrivate::instance;
        DocumentManagerPrivate::instance = 0;
    }
}

void DocumentManager::add(DocumentInfo *documentInfo)
{
    QMainWindow *mainWindow = documentInfo->mainWindow;
    d->documentInfos.append(documentInfo);
    connect(mainWindow, SIGNAL(destroyed(QObject *)),
            this, SLOT(remove(QObject *)));
    // update object name ("window ID")
    mainWindow->setObjectName(mainWindow->objectName() + QString::number(d->nextWindowId));
    documentInfo->id = d->nextWindowId;
    ++d->nextWindowId;
    QAction *action = new QAction(d->windowActionGroup);
    action->setText(mainWindow->objectName());
    emit changed();
}

const QList<const DocumentInfo *> &DocumentManager::getDocumentInfos() const
{
    return d->documentInfos;
}

QActionGroup &DocumentManager::getActionGroup() const
{
    return *d->windowActionGroup;
}

int DocumentManager::count() const
{
    return d->documentInfos.count();
}

void DocumentManager::activate(int id) const
{
    const DocumentInfo *documentInfo = d->documentInfos.at(id);
    if (documentInfo != 0) {
        documentInfo->mainWindow->activateWindow();
    }
}

int DocumentManager::getModifiedCount() const
{
    int result = 0;
    foreach (const DocumentInfo *documentInfo, d->documentInfos) {
        if (documentInfo->screenieScene->isModified()) {
            result++;
        }
    }
    return result;
}

// protected

DocumentManager::~DocumentManager()
{
    delete d;
}

// private

DocumentManager::DocumentManager()
    : d(new DocumentManagerPrivate())
{
}

// private slots

void DocumentManager::remove(QObject *object)
{
    foreach(const DocumentInfo *documentInfo, d->documentInfos) {
        if (documentInfo->mainWindow->objectName() == object->objectName()) {
            d->documentInfos.removeOne(documentInfo);
            emit changed();
            break;
        }
    }
}




