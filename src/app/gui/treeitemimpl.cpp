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

#include "treeitemimpl.h"
#include "treewidget.h"
#include "sharedtimer.h"
#include "treerole.h"
#include <IrcBufferModel>
#include <IrcBuffer>

TreeItemImpl::TreeItemImpl(IrcBuffer* buffer, TreeItem* parent) : TreeItem(parent)
{
    init(buffer);
}

TreeItemImpl::TreeItemImpl(IrcBuffer* buffer, TreeWidget* parent) : TreeItem(parent)
{
    init(buffer);
}

void TreeItemImpl::init(IrcBuffer* buffer)
{
    d.badge = 0;
    d.buffer = buffer;
    d.highlighted = false;
    setFlags(Qt::ItemIsEnabled | Qt::ItemIsSelectable);

    connect(buffer, SIGNAL(destroyed()), this, SLOT(deleteLater()));
    connect(buffer, SIGNAL(activeChanged(bool)), this, SLOT(refresh()));
    connect(buffer, SIGNAL(titleChanged(QString)), this, SLOT(refresh()));
}

TreeItemImpl::~TreeItemImpl()
{
}

IrcBuffer* TreeItemImpl::buffer() const
{
    return d.buffer;
}

IrcConnection* TreeItemImpl::connection() const
{
    return d.buffer->connection();
}

TreeItem* TreeItemImpl::parentItem() const
{
    return static_cast<TreeItem*>(QTreeWidgetItem::parent());
}

TreeWidget* TreeItemImpl::treeWidget() const
{
    return static_cast<TreeWidget*>(QTreeWidgetItem::treeWidget());
}

QVariant TreeItemImpl::data(int column, int role) const
{
    switch (role) {
    case TreeRole::Badge:
        return d.badge;
    case TreeRole::Highlight:
        return d.highlighted || (!isExpanded() && !d.highlightedChildren.isEmpty());
    case Qt::ForegroundRole:
        if (!d.buffer->isActive())
            return treeWidget()->palette().color(QPalette::Disabled, QPalette::Text);
        if (d.blink && (d.highlighted || (!isExpanded() && !d.highlightedChildren.isEmpty())))
            return QColor("#ff4040");
        return QTreeWidgetItem::data(column, role);
    default:
        if (column == 0 && d.buffer)
            return d.buffer->data(role);
        return QVariant();
    }
}

int TreeItemImpl::badge() const
{
    return d.badge;
}

void TreeItemImpl::setBadge(int badge)
{
    if (d.badge != badge) {
        d.badge = badge;
        emitDataChanged();
    }
}

bool TreeItemImpl::isHighlighted() const
{
    return d.highlighted;
}

void TreeItemImpl::setHighlighted(bool highlighted)
{
    if (d.highlighted != highlighted) {
        d.highlighted = highlighted;
        if (TreeItemImpl* p = static_cast<TreeItemImpl*>(parentItem())) {
            if (highlighted)
                p->d.highlightedChildren.insert(this);
            else
                p->d.highlightedChildren.remove(this);
            if (!p->isExpanded())
                p->emitDataChanged();
        }
        if (highlighted)
            SharedTimer::instance()->registerReceiver(this, "blink");
        else
            SharedTimer::instance()->unregisterReceiver(this, "blink");
        emitDataChanged();
    }
}

// TODO
class FriendlyModel : public IrcBufferModel
{
    friend class TreeItemImpl;
};

bool TreeItemImpl::operator<(const QTreeWidgetItem& other) const
{
    const TreeItem* otherItem = static_cast<const TreeItem*>(&other);
    if (!parentItem()) {
        QList<IrcConnection*> connections = treeWidget()->connections();
        return connections.indexOf(connection()) < connections.indexOf(otherItem->connection());
    }
    const FriendlyModel* model = static_cast<FriendlyModel*>(d.buffer->model());
    return model->lessThan(d.buffer, otherItem->buffer(), model->sortMethod());
}

void TreeItemImpl::reset()
{
    setBadge(0);
    setHighlighted(false);
}

void TreeItemImpl::refresh()
{
    emitDataChanged();
}

void TreeItemImpl::blink()
{
    d.blink = !d.blink;
    emitDataChanged();
}