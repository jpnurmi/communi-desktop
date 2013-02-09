/*
* Copyright (C) 2008-2013 Communi authors
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

#ifndef QUASSELPROTOCOL_H
#define QUASSELPROTOCOL_H

#ifdef HAVE_QUASSEL

#include <ircprotocol.h>
#include <QtCore/qvariant.h>

#include "bufferinfo.h"

class Message;
class IrcChannel;
class SignalProxy;

class QuasselProtocol : public IrcProtocol
{
    Q_OBJECT

public:
    explicit QuasselProtocol(IrcSession* session);
    virtual ~QuasselProtocol();

    virtual void login(const QString& password = QString());
    virtual void receive();
    virtual bool send(const QByteArray& data);

signals:
    void sendInput(const BufferInfo& buffer, const QString& message);

private slots:
    void initialize();
    void onChannelAdded(IrcChannel* channel);
    void onMessageReceived(const Message& message);

private:
    void loginToCore();
    void syncToCore(const QVariantMap& state);

    struct Private {
        SignalProxy* proxy;
        quint32 blockSize;
        QString password;
        bool initialized;
        QList<IrcChannel*> channels;
        QList<BufferInfo> buffers;
    } d;
};

#endif // HAVE_QUASSEL
#endif // QUASSELPROTOCOL_H
