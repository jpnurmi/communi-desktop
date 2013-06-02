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

#ifndef SERVERWIZARDPAGE_H
#define SERVERWIZARDPAGE_H

#include "ui_serverwizardpage.h"

class ServerWizardPage : public QWizardPage
{
    Q_OBJECT

public:
    ServerWizardPage(QWidget* parent = 0);
    ~ServerWizardPage();

    QString hostName() const;
    void setHostName(const QString& hostName);

    quint16 port() const;
    void setPort(quint16 port);

    bool isSecure() const;
    void setSecure(bool secure);

    bool isQuassel() const;
    void setQuassel(bool quassel);

    QString userName() const;
    void setUserName(const QString& userName);

    QString password() const;
    void setPassword(const QString& password);

    bool isComplete() const;

private:
    Ui::ServerWizardPage ui;
};

#endif // SERVERWIZARDPAGE_H
