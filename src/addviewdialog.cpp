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

#include "addviewdialog.h"
#include <QDialogButtonBox>
#include <QRegExpValidator>
#include <IrcConnection>
#include <QPushButton>
#include <QVBoxLayout>
#include <IrcNetwork>
#include <QLineEdit>
#include <QRegExp>
#include <QLabel>

AddViewDialog::AddViewDialog(IrcConnection* connection, QWidget* parent) : QDialog(parent)
{
    setWindowTitle(tr("Add view"));
    setWindowFlags(windowFlags() & ~Qt::WindowContextHelpButtonHint);

    d.connection = connection;

    d.viewLabel = new QLabel(this);
    d.viewEdit = new QLineEdit("#", this);
    d.viewEdit->setFocus();
    d.viewEdit->setValidator(new QRegExpValidator(QRegExp("\\S+"), d.viewEdit));
    connect(d.viewEdit, SIGNAL(textEdited(QString)), this, SLOT(updateUi()));

    QLabel* subLabel = new QLabel(this);
    const IrcNetwork* network = connection->network();
    subLabel->setText(tr("<small>%1 supports channel types: %2<small>").arg(network->name()).arg(network->channelTypes().join("")));
    subLabel->setAlignment(Qt::AlignRight);
    subLabel->setDisabled(true);

    d.passLabel = new QLabel(this);
    d.passLabel->setText("Password:");
    d.passEdit = new QLineEdit(this);
    d.passEdit->setPlaceholderText(tr("Optional..."));

    d.buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, Qt::Horizontal, this);
    connect(d.buttonBox, SIGNAL(accepted()), this, SLOT(accept()));
    connect(d.buttonBox, SIGNAL(rejected()), this, SLOT(reject()));

    QVBoxLayout* layout = new QVBoxLayout(this);
    layout->addWidget(d.viewLabel);
    layout->addWidget(d.viewEdit);
    layout->addWidget(subLabel);
    layout->addSpacing(9);
    layout->addWidget(d.passLabel);
    layout->addWidget(d.passEdit);
    layout->addSpacing(9);
    layout->addStretch();
    layout->addWidget(d.buttonBox);

    updateUi();
}

bool AddViewDialog::isChannel() const
{
    return !view().isEmpty() && d.connection->network()->channelTypes().contains(view().at(0));
}

QString AddViewDialog::view() const
{
    return d.viewEdit->text();
}

QString AddViewDialog::password() const
{
    return d.passEdit->text();
}

void AddViewDialog::updateUi()
{
    bool valid = false;
    bool channel = isChannel();
    if (channel) {
        valid = view().length() > 1;
        d.viewLabel->setText(tr("Join channel:"));
    } else {
        valid = !view().isEmpty();
        d.viewLabel->setText(tr("Open query:"));
    }

    d.passLabel->setEnabled(channel);
    d.passEdit->setEnabled(channel);
    d.buttonBox->button(QDialogButtonBox::Ok)->setEnabled(valid);
}
