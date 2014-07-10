/*
* Copyright (C) 2008-2014 The Communi Project
*
* This program is free software; you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation; either version 2 of the License, or
* (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*/

#ifndef QUASSELPLUGIN_H
#define QUASSELPLUGIN_H

#include <QObject>
#include <QtPlugin>
#include "connectionplugin.h"

class QuasselPlugin : public QObject, public ConnectionPlugin
{
    Q_OBJECT
    Q_INTERFACES(ConnectionPlugin)
    Q_PLUGIN_METADATA(IID "Communi.ConnectionPlugin")

public:
    QuasselPlugin(QObject* parent = 0);

    void connectionAdded(IrcConnection* connection);
};

#endif // QUASSELPLUGIN_H
