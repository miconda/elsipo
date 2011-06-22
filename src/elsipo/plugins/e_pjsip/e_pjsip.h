/****************************************************************************
**
** Copyright (C) 2011 Daniel Constantin Mierla
**
****************************************************************************/

#ifndef E_PJSIPPLUGIN_H
#define E_PJSIPPLUGIN_H

#include <QObject>
#include <QSettings>
#include "elsipointerface.h"

extern "C" {
#include <pjsua-lib/pjsua.h>
}

#include "pjsipcb.h"

class Pjsip : public QObject, ElsipoInterface
{
    Q_OBJECT
    Q_INTERFACES(ElsipoInterface)

public:
    Pjsip(QObject *parent = 0);
    ~Pjsip();
    int init(QObject *parent = 0);
    QObject *thisObj;
    pjsua_acc_id acc_id;
    QWebView *webView;

public slots:
    QString echoStr(const QString &message);
    QString name();
    QString version();
    int doSIPRegistration();
    int getSIPRegistrationStatus();
    int doCall(QString uri);
    int doCallAnswer();
    int doCallReject();
    int doCallHangup();
    int sendIM(QString uri, QString msg);
    int registerJSCallback(QString event, QString jsf);

    void slot_on_reg_state(int rcv_acc_id);
    void slot_on_message_request(QString from, QString ctype, QString body);
    void slot_on_message_request_status(QString to, int code, QString reason);
    void slot_on_incoming_call(int rcv_acc_id, int call_id, QString from);
    void slot_on_call_state(int call_id, QString state);
    void slot_on_call_media_state(int call_id);

public:
    QString getJSObjName();
    QObject *getJSObj();
    int registerJSAPI(QWebView *view);
    int addSIPUA();

private:
    QString pjUserURI;
    QString pjRegistrarURI;
    QString pjUser;
    QString pjPassword;
    QString pjSrvDomain;
    QString pjSrvPort;
    int pjStatus;
    int pjStatusRegistration;
    QSettings *iniSettings;
    PjsipCB *pjsipCB;
    int activePjCall;
    int activePjCallID;
    QString escapeJavascriptString(const QString & pstr);
};


#endif
