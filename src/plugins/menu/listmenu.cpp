/*
* Copyright (C) 2008-2013 The Communi Project
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

#include "listmenu.h"
#include "listview.h"
#include <QContextMenuEvent>
#include <IrcCommand>
#include <IrcChannel>
#include <Irc>

ListMenu::ListMenu(ListView* list) : QMenu(list)
{
    d.list = list;
    d.whoisAction = addAction(tr("Whois"), this, SLOT(onWhoisTriggered()));
    d.queryAction = addAction(tr("Query"), this, SLOT(onQueryTriggered()));
    addSeparator();
    d.opAction = addAction(tr("Op"), this, SLOT(onModeTriggered()));
    d.voiceAction = addAction(tr("Voice"), this, SLOT(onModeTriggered()));
    addSeparator();
    d.kickAction = addAction(tr("Kick"), this, SLOT(onKickTriggered()));
    d.banAction = addAction(tr("Ban"), this, SLOT(onBanTriggered()));

    list->installEventFilter(this);
}

bool ListMenu::eventFilter(QObject *object, QEvent *event)
{
    Q_UNUSED(object);
    if (event->type() == QEvent::ContextMenu) {
        QContextMenuEvent* cme = static_cast<QContextMenuEvent*>(event);
        QModelIndex index = d.list->indexAt(cme->pos());
        if (index.isValid())
            exec(index, cme->globalPos());
        return true;
    }
    return false;
}

void ListMenu::exec(const QModelIndex& index, const QPoint& pos)
{
    setup(index);
    QMenu::exec(pos);
}

void ListMenu::onWhoisTriggered()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        IrcCommand* command = IrcCommand::createWhois(action->data().toString());
        d.list->channel()->sendCommand(command);
    }
}

void ListMenu::onQueryTriggered()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action)
        QMetaObject::invokeMethod(d.list, "queried", Q_ARG(QString, action->data().toString()));
}

void ListMenu::onModeTriggered()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        QStringList params = action->data().toStringList();
        IrcCommand* command = IrcCommand::createMode(d.list->channel()->title(), params.at(1), params.at(0));
        d.list->channel()->sendCommand(command);
    }
}

void ListMenu::onKickTriggered()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        IrcCommand* command = IrcCommand::createKick(d.list->channel()->title(), action->data().toString());
        d.list->channel()->sendCommand(command);
    }
}

void ListMenu::onBanTriggered()
{
    QAction* action = qobject_cast<QAction*>(sender());
    if (action) {
        IrcCommand* command = IrcCommand::createMode(d.list->channel()->title(), "+b", action->data().toString() + "!*@*");
        d.list->channel()->sendCommand(command);
    }
}

void ListMenu::setup(const QModelIndex& index)
{
    const QString name = index.data(Irc::NameRole).toString();
    const QString prefix = index.data(Irc::PrefixRole).toString();

    d.whoisAction->setData(name);
    d.queryAction->setData(name);
    d.kickAction->setData(name);
    d.banAction->setData(name);

    if (prefix.contains("@")) {
        d.opAction->setText(tr("Deop"));
        d.opAction->setData(QStringList() << name << "-o");
    } else {
        d.opAction->setText(tr("Op"));
        d.opAction->setData(QStringList() << name << "+o");
    }

    if (prefix.contains("+")) {
        d.voiceAction->setText(tr("Devoice"));
        d.voiceAction->setData(QStringList() << name << "-v");
    } else {
        d.voiceAction->setText(tr("Voice"));
        d.voiceAction->setData(QStringList() << name << "+v");
    }
}
