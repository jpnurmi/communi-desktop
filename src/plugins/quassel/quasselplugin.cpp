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

#include "quasselplugin.h"
#include "quasselprotocol.h"
#include <IrcConnection>

QuasselPlugin::QuasselPlugin(QObject* parent) : QObject(parent)
{
}

class FriendlyConnection : public IrcConnection
{
    friend class QuasselPlugin;
};

void QuasselPlugin::connectionAdded(IrcConnection* connection)
{
    if (connection->userData().value("quassel", false).toBool())
        static_cast<FriendlyConnection*>(connection)->setProtocol(new QuasselProtocol(connection));
}
