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

#ifdef HAVE_QUASSEL

#include "quasselprotocol.h"
#include <qtcpsocket.h>
#include <ircsession.h>
#include <ircmessage.h>
#include <irc.h>

#include "types.h"
#include "message.h"
#include "network.h"
#include "identity.h"
#include "legacypeer.h"
#include "signalproxy.h"

static void registerTypes()
{
    qRegisterMetaType<Message>("Message");
    qRegisterMetaType<BufferInfo>("BufferInfo");
    qRegisterMetaType<NetworkInfo>("NetworkInfo");
    qRegisterMetaType<Network::Server>("Network::Server");
    qRegisterMetaType<Identity>("Identity");
    qRegisterMetaType<Network::ConnectionState>("Network::ConnectionState");

    qRegisterMetaTypeStreamOperators<Message>("Message");
    qRegisterMetaTypeStreamOperators<BufferInfo>("BufferInfo");
    qRegisterMetaTypeStreamOperators<NetworkInfo>("NetworkInfo");
    qRegisterMetaTypeStreamOperators<Network::Server>("Network::Server");
    qRegisterMetaTypeStreamOperators<Identity>("Identity");
    qRegisterMetaTypeStreamOperators<qint8>("Network::ConnectionState");

    qRegisterMetaType<IdentityId>("IdentityId");
    qRegisterMetaType<BufferId>("BufferId");
    qRegisterMetaType<NetworkId>("NetworkId");
    qRegisterMetaType<UserId>("UserId");
    qRegisterMetaType<AccountId>("AccountId");
    qRegisterMetaType<MsgId>("MsgId");

    qRegisterMetaTypeStreamOperators<IdentityId>("IdentityId");
    qRegisterMetaTypeStreamOperators<BufferId>("BufferId");
    qRegisterMetaTypeStreamOperators<NetworkId>("NetworkId");
    qRegisterMetaTypeStreamOperators<UserId>("UserId");
    qRegisterMetaTypeStreamOperators<AccountId>("AccountId");
    qRegisterMetaTypeStreamOperators<MsgId>("MsgId");
}

QuasselProtocol::QuasselProtocol(IrcSession* session) : IrcProtocol(session)
{
    registerTypes();

    d.peer = 0;
    d.blockSize = 0;
    d.initialized = false;
    d.proxy = new SignalProxy(this);
    d.proxy->attachSignal(this, SIGNAL(sendInput(BufferInfo, QString)));
    d.proxy->attachSlot(SIGNAL(displayMsg(Message)), this, SLOT(onMessageReceived(Message)));
}

QuasselProtocol::~QuasselProtocol()
{
}

void QuasselProtocol::login(const QString& password)
{
    QVariantMap msg;
    msg["MsgType"] = "ClientInit";
    msg["ProtocolVersion"] = 10;

    d.password = password;
//    SignalProxy::writeDataToDevice(session()->socket(), msg);
}

void QuasselProtocol::receive()
{
    QVariant item;
//    while (SignalProxy::readDataFromDevice(socket(), d.blockSize, item)) {
//        QVariantMap msg = item.toMap();
//        if (!msg.contains("MsgType")) {
//            qCritical() << "OLD PROTOCOL";
//            d.proxy->removeAllPeers();
//            session()->close();
//            return;
//        }
//        if (msg["MsgType"] == "ClientInitAck") {
//            qDebug() << "INIT ACK";
//            if (msg["LoginEnabled"].toBool())
//                loginToCore();
//        }
//        else if(msg["MsgType"] == "ClientInitReject") {
//            d.proxy->removeAllPeers();
//            session()->close();
//            return;
//        }
//        else if (msg["MsgType"] == "ClientInitReject") {
//            qDebug() << "INIT REJECT";
//        }
//        else if (msg["MsgType"] == "ClientLoginReject") {
//            qDebug() << "LOGIN REJECT";
//        }
//        else if (msg["MsgType"] == "ClientLoginAck") {
//            qDebug() << "LOGIN ACK";
//        }
//        else if (msg["MsgType"] == "SessionInit") {
//            qDebug() << "SESSION INIT";
//            disconnect(socket(), SIGNAL(readyRead()), session(), SLOT(_irc_readData()));
//            d.proxy->addPeer(socket());
//            // TODO: IrcSessionPrivate::get(session())->setConnected(true);
//            syncToCore(msg["SessionState"].toMap());
//        } else {
//            d.proxy->removeAllPeers();
//            session()->close();
//            return;
//        }
//    }
}

bool QuasselProtocol::send(const QByteArray& data)
{
    QByteArray copy(data);
    int idx = copy.indexOf(' ');
    QByteArray command = copy.left(idx).toUpper();
    copy.remove(0, idx + 1);

    if (command == "PRIVMSG") {
        idx = copy.indexOf(' ');
        QString target = QString::fromUtf8(copy.left(idx)).toLower();
        copy.remove(0, idx + 1);

        BufferInfo buffer;
        foreach (const BufferInfo& b, d.buffers) {
            if (b.bufferName().toLower() == target) {
                buffer = b;
                break;
            }
        }

        if (buffer.isValid()) {
            QString msg = QString::fromUtf8(copy.mid(1));
            emit sendInput(buffer, "/SAY " + msg);
            return true;
        }
    } else if (command == "JOIN") {
        emit sendInput(d.buffers.first(), "/" + QString::fromUtf8(data));
        return true;
    } else if (command == "PART") {
        idx = copy.indexOf(' ');
        QString target = QString::fromUtf8(copy.left(idx)).toLower();
        copy.remove(0, idx + 1);

        BufferInfo buffer;
        foreach (const BufferInfo& b, d.buffers) {
            if (b.bufferName().toLower() == target) {
                buffer = b;
                break;
            }
        }

        if (buffer.isValid()) {
            QString msg = QString::fromUtf8(copy.mid(1));
            emit sendInput(buffer, "/PART " + msg);
            return true;
        }
    }

    qDebug() << "###" << data;
    return false;
}

void QuasselProtocol::onChannelAdded(IrcChannel* channel)
{
    if (!d.initialized) {
        d.channels += channel;
        return;
    }

    IrcSender sender;
    sender.setHost("quassel");
    sender.setName(session()->nickName());
    sender.setUser(session()->userName());

    IrcMessage* msg = IrcMessage::fromParameters(sender.prefix(), "JOIN", QStringList() << channel->name(), session());
    receiveMessage(msg);

    if (!channel->topic().isEmpty())
        msg = IrcMessage::fromParameters(sender.prefix(), QString::number(Irc::RPL_TOPIC), QStringList() << sender.name() << channel->name() << channel->topic(), session());
    else
        msg = IrcMessage::fromParameters(sender.prefix(), QString::number(Irc::RPL_NOTOPIC), QStringList() << sender.name() << channel->name(), session());
    receiveMessage(msg);

    QStringList users;
    Network* network = channel->network();
    foreach (IrcUser* user, channel->ircUsers()) {
        QString nick = user->nick();
        QString prefix = channel->userModes(nick);
        if (!prefix.isEmpty())
            prefix = network->modeToPrefix(prefix);
        users += prefix + nick;
    }
    msg = IrcMessage::fromParameters(sender.prefix(), QString::number(Irc::RPL_NAMREPLY), QStringList() << sender.name() << "=" << channel->name() << users.join(" "), session());
    receiveMessage(msg);

    msg = IrcMessage::fromParameters(sender.prefix(), QString::number(Irc::RPL_ENDOFNAMES), QStringList() << sender.name() << channel->name() << "End of /NAMES list.", session());
    receiveMessage(msg);
}

static QString messageCommand(Message::Type type)
{
    switch (type)
    {
    case Message::Plain:        return QLatin1String("PRIVMSG");
    case Message::Notice:       return QLatin1String("NOTICE");
    case Message::Action:       return QLatin1String("PRIVMSG");
    case Message::Nick:         return QLatin1String("NICK");
    case Message::Mode:         return QLatin1String("MODE");
    case Message::Join:         return QLatin1String("JOIN");
    case Message::Part:         return QLatin1String("PART");
    case Message::Quit:         return QLatin1String("QUIT");
    case Message::Kick:         return QLatin1String("KICK");
    case Message::Kill:         return QString();
    case Message::Server:       return QString();
    case Message::Info:         return QString();
    case Message::Error:        return QString();
    case Message::DayChange:    return QString();
    case Message::Topic:        return QLatin1String("TOPIC");
    case Message::NetsplitJoin: return QString();
    case Message::NetsplitQuit: return QString();
    case Message::Invite:       return QLatin1String("INVITE");
    default:                    return QString();
    }
}

void QuasselProtocol::onMessageReceived(const Message& message)
{
    if (message.flags() & Message::Self)
        return;

    QString buffer = message.bufferInfo().bufferName();
    QString command = messageCommand(message.type());
    QString contents = message.contents();
    QStringList split;
    if (message.type() == Message::Mode || message.type() == Message::Kick ||
            message.type() == Message::Topic || message.type() == Message::Invite)
        split = contents.split(' ', QString::SkipEmptyParts);

    switch (message.type())
    {
    case Message::Action:
        contents = QString("\1ACTION %1\1").arg(contents);
        // flow through
    case Message::Plain:
    case Message::Notice:
    case Message::Join:
    case Message::Part:
        receiveMessage(IrcMessage::fromParameters(message.sender(), command, QStringList() << buffer << contents, session()));
        break;
    case Message::Nick:
    case Message::Quit:
        receiveMessage(IrcMessage::fromParameters(message.sender(), command, QStringList() << contents, session()));
        break;
    case Message::Mode:
        receiveMessage(IrcMessage::fromParameters(message.sender(), command, split, session()));
        break;
    case Message::Kick:
        receiveMessage(IrcMessage::fromParameters(message.sender(), command, QStringList() << buffer << split.value(0) << QStringList(split.mid(1)).join(" "), session()));
        break;
    case Message::Topic:
        foreach (IrcChannel* channel, d.channels) {
            if (!channel->name().compare(buffer, Qt::CaseInsensitive)) {
                receiveMessage(IrcMessage::fromParameters(split.value(0), command, QStringList() << buffer << channel->topic(), session()));
                break;
            }
        }
        break;
    case Message::Invite:
        receiveMessage(IrcMessage::fromParameters(split.first(), command, QStringList() << session()->nickName() << split.last(), session()));
        break;
        // TODO:
    case Message::Kill:
    case Message::Server:
    case Message::Info:
    case Message::Error:
    case Message::DayChange:
    case Message::NetsplitJoin:
    case Message::NetsplitQuit:
    default:
        qDebug() << "###" << message.timestamp() << message.type() << message.sender() << message.contents();
        break;
    }
}

void QuasselProtocol::initialize()
{
    Network* network = qobject_cast<Network*>(QObject::sender());
    if (network) {
        IrcSender sender;
        sender.setHost("quassel");
        sender.setName(session()->nickName());
        sender.setUser(session()->userName());

        IrcMessage* msg = IrcMessage::fromParameters(sender.prefix(), QString::number(Irc::RPL_WELCOME), QStringList() << sender.name() << "TODO: Welcome to Quassel...", session());
        receiveMessage(msg);

        QStringList support;
        support += QString("NETWORK=%1").arg(network->support("NETWORK"));
        support += QString("PREFIX=%2").arg(network->support("PREFIX"));
        support += QString("CHANTYPES=%3").arg(network->support("CHANTYPES"));
        msg = IrcMessage::fromParameters(sender.prefix(), QString::number(Irc::RPL_ISUPPORT), QStringList() << sender.name() << support, session());
        receiveMessage(msg);

        d.initialized = true;
        foreach (IrcChannel* channel, d.channels)
            onChannelAdded(channel);
    }
}

void QuasselProtocol::loginToCore()
{
    QVariantMap msg;
    msg["MsgType"] = "ClientLogin";
    msg["User"] = session()->userName();
    msg["Password"] = d.password;

    d.peer = new LegacyPeer(qobject_cast<QTcpSocket*>(socket()), this);
//    connect(peer, SIGNAL(dataReceived(QVariant)), SLOT(coreHasData(QVariant)));
//    connect(peer, SIGNAL(transferProgress(int,int)), SLOT(updateProgress(int,int)));
    d.peer->writeSocketData(msg);
}

void QuasselProtocol::syncToCore(const QVariantMap& state)
{
    // state["BufferInfos"].toList().value<BufferInfo>()
    // state["CoreFeatures"].toUint()
    // state["Identities"].toList().value<Identity>()
    // state["IrcChannelCount"].toUint()
    // state["IrcUserCount"].toUint()
    // state["NetworkIds"].toList().value<NetworkId>()

    QVariantList buffers = state["BufferInfos"].toList();
    foreach (const QVariant& buf, buffers)
        d.buffers += buf.value<BufferInfo>();

    QVariantList networks = state["NetworkIds"].toList();
    foreach (const QVariant& nid, networks) {
        Network* network = new Network(nid.value<NetworkId>(), this);
        connect(network, SIGNAL(networkNameSet(QString)), SLOT(initialize()));
        connect(network, SIGNAL(ircChannelAdded(IrcChannel*)), SLOT(onChannelAdded(IrcChannel*)));
        network->setProxy(d.proxy);
        d.proxy->synchronize(network);
    }
}

#endif // HAVE_QUASSEL
