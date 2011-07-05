/****************************************************************************
**
** Copyright (C) 2011 Daniel-Constantin Mierla
**
****************************************************************************/

#include <QtGui>
#include <QtWebKit>
#include <QWebFrame>
#include <QDebug>
#include "e_pjsip.h"

#include <iostream>
#include <resolv.h>
#include <arpa/inet.h>
using namespace std;

#define THIS_FILE	"ELSIPO-PJSIP"

extern PjsipCB *globalPjsipCB;


Pjsip::Pjsip(QObject *parent) : QObject(parent)
{
    thisObj = NULL;
    pjStatus = 0;
    pjStatusRegistration = 0;
    webView = NULL;
    globalPjsipCB = 0;
    pjsipCB = 0;
    activePjCall = 0;
    activePjCallID = 0;
    pjsipCB = new PjsipCB();

    QObject::connect((PjsipCB*)globalPjsipCB, SIGNAL(signal_on_reg_state(int)),
                     this, SLOT(slot_on_reg_state(int)), Qt::QueuedConnection);
    QObject::connect((PjsipCB*)globalPjsipCB, SIGNAL(signal_on_message_request(QString,QString,QString)),
                     this, SLOT(slot_on_message_request(QString,QString,QString)), Qt::QueuedConnection);
    QObject::connect((PjsipCB*)globalPjsipCB, SIGNAL(signal_on_message_request_status(QString,int,QString)),
                     this, SLOT(slot_on_message_request_status(QString,int,QString)), Qt::QueuedConnection);
    QObject::connect((PjsipCB*)globalPjsipCB, SIGNAL(signal_on_incoming_call(int,int,QString)),
                     this, SLOT(slot_on_incoming_call(int,int,QString)), Qt::QueuedConnection);
    QObject::connect((PjsipCB*)globalPjsipCB, SIGNAL(signal_on_call_state(int,QString)),
                     this, SLOT(slot_on_call_state(int,QString)), Qt::QueuedConnection);
    QObject::connect((PjsipCB*)globalPjsipCB, SIGNAL(signal_on_call_media_state(int)),
                     this, SLOT(slot_on_call_media_state(int)), Qt::QueuedConnection);
}

Pjsip::~Pjsip()
{
    /* Destroy pjsua */
    pjsua_destroy();
}

int Pjsip::init(QObject *parent)
{
    pj_status_t status;

    qDebug() << "initializing pjsip module";

    iniSettings = new QSettings(QDir::homePath() + QDir::separator()
                                + ".elsipo" + QDir::separator() + "elsipo.ini",
                                QSettings::IniFormat);
    if(iniSettings==NULL) {
        qDebug() << "Error - Could not load configuration file";
        return -1;
    }
    iniSettings->beginGroup("Plugin.Pjsip");

    pjUser = iniSettings->value("username").toString();
    pjPassword = iniSettings->value("password").toString();
    pjSrvDomain = iniSettings->value("domain").toString();
    pjSrvPort = iniSettings->value("port").toString();
    if(pjSrvPort=="5060") {
        pjUserURI = "sip:" + pjUser + "@" + pjSrvDomain;
        pjRegistrarURI = "sip:" + pjSrvDomain;
    } else {
        pjUserURI = "sip:" + pjUser + "@" + pjSrvDomain + ":" + pjSrvPort;
        pjRegistrarURI = "sip:" + pjSrvDomain + ":" + pjSrvPort;
    }

    iniSettings->endGroup();

    qDebug() << "pjsip module settings: [" << pjUser << "/" << pjPassword
             << "/" << pjSrvDomain << "/" << pjSrvPort << "/" << pjUserURI
             << "/" << pjRegistrarURI << "]";

    pjStatus = 0;
    /* Create pjsua first! */
    status = pjsua_create();
    if (status != PJ_SUCCESS) {
        qDebug() << "Error in pjsua_create() >> " << QString::number(status);
        return -1;
    }


    /* Init pjsua */
    {
        pjsua_config cfg;
        pjsua_logging_config log_cfg;

        pjsua_config_default(&cfg);
        cfg.cb.on_incoming_call = &PjsipCB::static_on_incoming_call;
        cfg.cb.on_call_media_state = &PjsipCB::static_on_call_media_state;
        cfg.cb.on_call_state = &PjsipCB::static_on_call_state;
        cfg.cb.on_reg_state = &PjsipCB::static_on_reg_state;
        cfg.cb.on_pager = &PjsipCB::static_on_message_request;
        cfg.cb.on_pager_status = &PjsipCB::static_on_message_request_status;
        cfg.nameserver_count = 0;

        /* Add DNS servers from OS */
        if ((_res.options & RES_INIT) == 0) res_init();

        int i;
        for (i = 0; i<_res.nscount; i++)
        {
            cfg.nameserver[cfg.nameserver_count++] = pj_str(strdup(inet_ntoa(_res.nsaddr_list[i].sin_addr)));
        }

        if (cfg.nameserver_count == 0) {
            // list is empty, add Google public DNS as last resort
            const char *google_dns = "8.8.8.8";
            cfg.nameserver[cfg.nameserver_count++] = pj_str( (char*) google_dns );
        }

        pjsua_logging_config_default(&log_cfg);
        log_cfg.console_level = 4;

        status = pjsua_init(&cfg, &log_cfg, NULL);
        if (status != PJ_SUCCESS) {
            qDebug() << "Error in pjsua_init()" << QString::number(status);
            return -1;
        }
    }

    /* Add UDP transport. */
    {
        pjsua_transport_config cfg;

        pjsua_transport_config_default(&cfg);
        cfg.port = 5084;
        status = pjsua_transport_create(PJSIP_TRANSPORT_UDP, &cfg, NULL);
        if (status != PJ_SUCCESS) {
            qDebug() << "Error creating transport " << QString::number(status);
            return -1;
        }
    }

    /* Initialization is done, now start pjsua */
    status = pjsua_start();
    if (status != PJ_SUCCESS) {
        qDebug() << "Error starting pjsua " << QString::number(status);
        return -1;
    }

    thisObj = this; //new Pjsip(parent);
    pjStatus = 1;
    return 0;
}

QString Pjsip::echoStr(const QString &message)
{
    return QString("Pjsip Plugin Says: ") + message;
}


QString Pjsip::name()
{
    return QString("Pjsip");
}


QString Pjsip::version()
{
    return QString("0.1.0");
}

QString Pjsip::getJSObjName()
{
    return QString("sip");
}

QObject* Pjsip::getJSObj()
{
    return thisObj;
}

int Pjsip::registerJSAPI(QWebView *view)
{
    if(thisObj==NULL)
        return -1;
    QWebFrame *frame = view->page()->mainFrame();
    frame->addToJavaScriptWindowObject( QString("sip"), thisObj );
    webView = view;
    return 0;
}

int Pjsip::addSIPUA()
{
    pj_status_t status;
    /* Register to SIP server by creating SIP account. */
    {
        pjsua_acc_config cfg;

        pjsua_acc_config_default(&cfg);
        cfg.id = pj_str(strdup(pjUserURI.toLatin1().data()));
        /**int i = 0;
          char *data =pjRegistrarURI.toLatin1().data();
          while (i<(int)cfg.id.slen) {
              cout << "*[" << cfg.id.ptr[i] << "]" << endl;
              i++;
          }**/

        cfg.reg_uri = pj_str(strdup(pjRegistrarURI.toLatin1().data()));

        cfg.cred_count = 1;
        cfg.cred_info[0].realm = pj_str(strdup(pjSrvDomain.toAscii().data()));
        cfg.cred_info[0].scheme = pj_str((char*)"digest");
        cfg.cred_info[0].username = pj_str(strdup(pjUser.toAscii().data()));
        cfg.cred_info[0].data_type = PJSIP_CRED_DATA_PLAIN_PASSWD;
        cfg.cred_info[0].data = pj_str(strdup(pjPassword.toAscii().data()));

        status = pjsua_acc_add(&cfg, PJ_TRUE, &acc_id);
        if (status != PJ_SUCCESS) {
            qDebug() << "Error in pjsua_acc_add() " << QString::number(status)
                     << " / [" << pjRegistrarURI.toLatin1().data() << "]"
                     << " [" << QString(( const QChar *)cfg.reg_uri.ptr, (int)cfg.reg_uri.slen) << "]";
            return -1;
        }
    }
    return 0;
}

void Pjsip::slot_on_reg_state(int rcv_acc_id) {

    pj_status_t status;
    pjsua_acc_info info;

    cout << "pjsip registration update" << endl;

    if(webView==NULL) {
        cout << "no pjsip instance" << endl;
        return;
    }
    if(acc_id!=rcv_acc_id) {
        cout << "not current acc id" << endl;
        return;
    }
    if (!pjsua_acc_is_valid(acc_id)) {
        cout << "invalid pjsip acc id" << endl;
        return;
    }
    status = pjsua_acc_get_info(acc_id, &info);
    if (status != PJ_SUCCESS) {
        cout << "error getting pjsip account info " << status << endl;
        return;
    }
    if (info.status == 200) {
        cout << "registration ok " << info.status << endl;
        if(pjStatusRegistration == 2)
            pjStatusRegistration = 3;
        else if(pjStatusRegistration == 4)
            pjStatusRegistration = 1;
        webView->page()->mainFrame()->evaluateJavaScript("sipcb_register_ok(); null");
    } else {
        cout << "registration failed " << info.status << endl;
        pjStatusRegistration = 3;
        webView->page()->mainFrame()->evaluateJavaScript("sipcb_register_failed(); null");
    }
}

int Pjsip::doSIPRegistration()
{
    pj_status_t status;
    if(pjStatusRegistration==0) {
        if(addSIPUA()<0) {
            pjStatusRegistration = 0;
            return -1;
        } else {
            pjStatusRegistration = 1;
            return 0;
        }
    }
    if (!pjsua_acc_is_valid(acc_id)) {
        qDebug() << "invalid pjsip acc id " << QString::number(acc_id);
        return -1;
    }
    if(pjStatusRegistration==1) {
        status = pjsua_acc_set_registration(acc_id, 0);
        if (status != PJ_SUCCESS) {
            qDebug() << "Error in pjsua unregister " << QString::number(status);
            return -1;
        }
        pjStatusRegistration = 2;
        return 0;
    }
    if(pjStatusRegistration==3) {
        status = pjsua_acc_set_registration(acc_id, 1);
        if (status != PJ_SUCCESS) {
            qDebug() << "Error in pjsua unregister " << QString::number(status);
            return -1;
        }
        pjStatusRegistration = 4;
        return 0;
    }
    return -1;
}

int Pjsip::registerJSCallback(QString event, QString jsf)
{
    return 0;
}

int Pjsip::getSIPRegistrationStatus()
{
    return pjStatusRegistration;
}

void Pjsip::slot_on_message_request(QString from, QString ctype, QString body)
{
    if(webView==NULL) {
        cout << "no pjsip instance" << endl;
        return;
    }

    webView->page()->mainFrame()->evaluateJavaScript(QString("sipcb_message(\"%1\",\"%2\",\"%3\");null").arg(from).arg(ctype).arg(escapeJavascriptString(body)));
}

void Pjsip::slot_on_message_request_status(QString to, int code, QString reason)
{
    if(webView==NULL) {
        cout << "no pjsip instance" << endl;
        return;
    }

    if(code>=300) {
        webView->page()->mainFrame()->evaluateJavaScript(QString("sipcb_message_failed(\"%1\",\"%2\",\"%3\");null").arg(to).arg(code).arg(escapeJavascriptString(reason)));
    }
}

QString Pjsip::escapeJavascriptString(const QString & pstr)
{
    QString out;
    QRegExp rx("(\\r|\\n|\\\\|\")");
    int pos = 0, lastPos = 0;

    while ((pos = rx.indexIn(pstr, pos)) != -1)
    {
        out += pstr.mid(lastPos, pos - lastPos);

        switch (rx.cap(1).at(0).unicode())
        {
        case '\r':
            out += "\\r";
            break;
        case '\n':
            out += "\\n";
            break;
        case '"':
            out += "\\\"";
            break;
        case '\\':
            out += "\\\\";
            break;
        }
        pos++;
        lastPos = pos;
    }
    out += pstr.mid(lastPos);
    return out;
}

/* Callback called by the library upon receiving incoming call */
void Pjsip::slot_on_incoming_call(int rcv_acc_id, int call_id, QString from)
{
    /*
    pjsua_call_info ci;
    pjsua_call_get_info(call_id, &ci);
    */
    qDebug() << "Incoming call from: " << from;
    if(webView==NULL) {
        cout << "no pjsip instance" << endl;
        return;
    }
    if(acc_id!=rcv_acc_id) {
        cout << "not current acc id" << endl;
        return;
    }
    if(activePjCall != 0)
    {
        qDebug() << "Another call is active - rejecting";
        pjsua_call_answer(call_id, 486, NULL, NULL);
        return;
    }
    /* Automatically answer incoming calls with 200/OK */
    activePjCall = 1;
    activePjCallID = call_id;
    webView->page()->mainFrame()->evaluateJavaScript(QString("sipcb_call_in(\"%1\");null").arg(escapeJavascriptString(from)));

    // pjsua_call_answer(call_id, 200, NULL, NULL);
}

/* Callback called by the library when call's state has changed */
void Pjsip::slot_on_call_state(int call_id, QString state)
{
    pjsua_call_info ci;
    pj_status_t status;

    qDebug() << "Call " << QString::number(call_id) << " state=" << state;
    if(webView==NULL) {
        cout << "no pjsip instance" << endl;
        return;
    }
    status = pjsua_call_get_info(call_id, &ci);
    if (status != PJ_SUCCESS) {
        qDebug() << "error fetching info for call: " << QString::number(call_id);
        if(state=="DISCONNCTD") {
            activePjCall = 0;
            activePjCallID = 0;
            webView->page()->mainFrame()->evaluateJavaScript(QString("sipcb_call_ended();null"));
        }
        return;
    }
    switch(ci.state) {
    case PJSIP_INV_STATE_DISCONNECTED:
        activePjCall = 0;
        activePjCallID = 0;
        webView->page()->mainFrame()->evaluateJavaScript(QString("sipcb_call_ended();null"));
        break;
    default:
        ;
    }

}

/* Callback called by the library when call's media state has changed */
void Pjsip::slot_on_call_media_state(pjsua_call_id call_id)
{
    pjsua_call_info ci;

    pjsua_call_get_info(call_id, &ci);

    if (ci.media_status == PJSUA_CALL_MEDIA_ACTIVE) {
        // When media is active, connect call to sound device.
        pjsua_conf_connect(ci.conf_slot, 0);
        pjsua_conf_connect(0, ci.conf_slot);
    }
}

int Pjsip::doCall(QString uri)
{
    if(pjStatusRegistration==0) {
        webView->page()->mainFrame()->evaluateJavaScript(QString("sipcb_alert(\"Not registered!\");null"));
        return -1;
    }
    if(activePjCall != 0)
    {
        qDebug() << "Another call is active - ignoring";
        return -1;
    }
    QByteArray turi = uri.toLatin1();
    int call_id;
    pj_str_t pjuri = pj_str(strdup(turi.data()));
    pj_status_t status = pjsua_call_make_call(acc_id, &pjuri, 0, 0, 0, &call_id);
    if (status != PJ_SUCCESS) {
        qDebug() << "error calling " << uri << QString::number(status);
        return -1;
    } else {
        activePjCall = 1;
        activePjCallID = call_id;
        // set button ("hang up");
    }
    return 0;
}

int Pjsip::doCallAnswer()
{
    if(activePjCall == 1) {
        pjsua_call_answer(activePjCallID, 200, NULL, NULL);
    }
    return 0;
}

int Pjsip::doCallReject()
{
    if(activePjCall == 1) {
        pjsua_call_hangup(activePjCallID, 486, NULL, NULL);
        activePjCall = 0;
        activePjCallID = 0;
    }
    return 0;
}

int Pjsip::doCallHangup()
{
    if(activePjCall == 1) {
        pjsua_call_hangup(activePjCallID, 0, NULL, NULL);
        activePjCall = 0;
        activePjCallID = 0;
    }
    return 0;
}

int Pjsip::sendIM(QString uri, QString msg) {
    pj_status_t status;

    if(pjStatusRegistration==0) {
        webView->page()->mainFrame()->evaluateJavaScript(QString("sipcb_alert(\"Not registered!\");null"));
        return -1;
    }
    qDebug() << "sending IM to " << uri << " [" + msg + "]";
    QByteArray turi = uri.toLatin1();
    QByteArray tmsg = msg.toLatin1();

    pj_str_t pjuri = pj_str(strdup(turi.data()));
    pj_str_t pjmsg = pj_str(strdup(tmsg.data()));

    status = pjsua_im_send(acc_id, &pjuri,
                           NULL, &pjmsg,
                           NULL, NULL);
    if (status != PJ_SUCCESS)
    {
        qDebug() << "Error sending IM to " << uri << " status " << QString::number(status);
        return -1;
    }
    qDebug() << "IM sent to " << uri << " status " << QString::number(status);
    return 0;
}

Q_EXPORT_PLUGIN2(pjsip, Pjsip);
