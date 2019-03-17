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

#ifndef LDGAMESEXAMPLE_H
#define LDGAMESEXAMPLE_H

#include <memory>

#include <QQmlHelpers>
#include <QQmlGadgetListModel>

class QQmlApplicationEngine;

class ldCore;
class ldGameManager;
class ldVisualizer;

class ldGamesExample : public QObject
{
    Q_OBJECT

    QML_CONSTANT_PROPERTY(ldCore*, ldCore)
    QML_CONSTANT_PROPERTY(ldGameManager*, gameManager)

public:
    ldGamesExample(QQmlApplicationEngine *engine, QObject *parent = nullptr);
    ~ldGamesExample();

    void init();

public slots:
    void setWindow(QObject *window);

private:
    QQmlApplicationEngine *m_qmlEngine;
};

#endif // LDGAMESEXAMPLE_H
