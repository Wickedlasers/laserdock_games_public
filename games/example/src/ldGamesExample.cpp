/**
    libLaserdockCore
    Copyright(c) 2018 Wicked Lasers

    This file is part of libLaserdockCore.

    libLaserdockCore is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    libLaserdockCore is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with libLaserdockCore.  If not, see <https://www.gnu.org/licenses/>.
**/

#include "ldGamesExample.h"

#include <QtGui/QGuiApplication>
#include <QtCore/QtDebug>
#include <QtCore/QTimer>
#include <QtQml/QtQml>

#include <ldCore/ldCore.h>
#include <ldGames/ldGameManager.h>

ldGamesExample::ldGamesExample(QQmlApplicationEngine *engine, QObject *parent)
    : QObject(parent)
    , m_ldCore(ldCore::create(parent))
    , m_qmlEngine(engine)
{
    qmlRegisterType<ldCore>();
    qmlRegisterType<ldGameManager>();

    m_ldCore->initialize();
    m_gameManager = new ldGameManager(parent);

    QTimer::singleShot(0, this, &ldGamesExample::init);
}

ldGamesExample::~ldGamesExample()
{
}

void ldGamesExample::init()
{
    // init qml
    m_qmlEngine->rootContext()->setContextObject(this);
    m_qmlEngine->load(QUrl(QStringLiteral("qrc:/qml/main.qml")));
    if(m_qmlEngine->rootObjects().isEmpty()) {
        QTimer::singleShot(0, qApp, &QGuiApplication::quit);
        return;
    }

    m_gameManager->update_isActive(true);
}

void ldGamesExample::setWindow(QObject *window)
{
    window->installEventFilter(m_gameManager);
}
