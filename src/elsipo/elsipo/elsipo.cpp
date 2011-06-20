/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Nokia Corporation and its Subsidiary(-ies) nor
**     the names of its contributors may be used to endorse or promote
**     products derived from this software without specific prior written
**     permission.
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtGui>
#include <QtWebKit>
#include <QDir>

#include "elsipo.h"


ElsipoWindow::ElsipoWindow()
{
    webView = new QWebView(this);
    setWindowTitle("Elsipo - the SIP Browser");
    iniSettings = new QSettings(QDir::homePath() + QDir::separator()
                                + ".elsipo" + QDir::separator() + "elsipo.ini",
                                QSettings::IniFormat);
    if(iniSettings==NULL) {
        QMessageBox::information(this, "Error", "Could not load configuration file");
        return;
    }
    iniSettings->beginGroup("Elsipo");
    QString startUrl = iniSettings->value("url").toString();
    iniSettings->endGroup();
    // QMessageBox::information(this, "Info", "Start Url is: " + startUrl);

    if (!loadPlugins()) {
        QMessageBox::information(this, "Error", "Could not load the plugin");
        return;
    }
    setCentralWidget(webView);

    webView->setUrl(startUrl);
}



int ElsipoWindow::loadPlugins()
{
    int pluginsNo = 0;
    QDir pluginsDir(qApp->applicationDirPath());
#if defined(Q_OS_WIN)
    if (pluginsDir.dirName().toLower() == "debug" || pluginsDir.dirName().toLower() == "release")
        pluginsDir.cdUp();
#elif defined(Q_OS_MAC)
    if (pluginsDir.dirName() == "MacOS") {
        pluginsDir.cdUp();
        pluginsDir.cdUp();
        pluginsDir.cdUp();
    }
#endif
    pluginsDir.cd("plugins");
    QFileInfoList list = pluginsDir.entryInfoList();
    for (int iList=0;iList<list.count();iList++)
    {
      QFileInfo info = list[iList];
      if (info.isDir() && info.fileName()!=".." && info.fileName()!=".")
      {
        QDir pgDir(info.absoluteFilePath());
        foreach (QString fileName, pgDir.entryList(QDir::Files | QDir::NoSymLinks)) {
          QPluginLoader pluginLoader(pgDir.absoluteFilePath(fileName), this);
          QObject *plugin = pluginLoader.instance();
          if (plugin) {
            ElsipoInterface* pluginInterface = qobject_cast<ElsipoInterface *>(plugin);
            if (pluginInterface) {
                pluginInterface->init();
                pluginsList.append(pluginInterface);
                pluginsNo++;
            }
         }
       }
      }
    }

    return pluginsNo;
}

void ElsipoWindow::initJSObjects()
{
    QWebFrame *frame = webView->page()->mainFrame();

    attachJSObjects();
    connect( frame, SIGNAL(javaScriptWindowObjectCleared()), this, SLOT(attachJSObjects()) );
}

void ElsipoWindow::attachJSObjects()
{
    QWebFrame *frame = webView->page()->mainFrame();
    frame->addToJavaScriptWindowObject( "elsipo", this );
    for (int iList=0;iList<pluginsList.count();iList++)
    {
        ElsipoInterface* pluginInterface = pluginsList.at(iList);
        if(pluginInterface) {
            pluginInterface->registerJSAPI(webView);

            //frame->addToJavaScriptWindowObject( pluginInterface->getJSObjName(),
            //                                    pluginInterface->getJSObj() );
            /*if(pluginInterface->getJSObj()==NULL)
            {
                QMessageBox::information(this, "Info", "Registered null JS Object: "
                                     + pluginInterface->getJSObjName());
            } else {
                QMessageBox::information(this, "Info", "Registered JS Object: "
                                         + pluginInterface->getJSObjName());
            }*/
        }
    }
}

QString ElsipoWindow::name()
{
    return QString("elsipo");
}

QString ElsipoWindow::version()
{
    return QString("0.1.0");
}

