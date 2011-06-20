#include <QDebug>

#include "pjsipcb.h"

PjsipCB *globalPjsipCB;

PjsipCB::PjsipCB() {
	globalPjsipCB = this;
}

PjsipCB::~PjsipCB() {
}

void PjsipCB::on_reg_state(pjsua_acc_id rcv_acc_id) {
        emit signal_on_reg_state(rcv_acc_id);
}

void PjsipCB::static_on_reg_state(pjsua_acc_id rcv_acc_id) {
	/* call the non-static member */
	if (globalPjsipCB) {
		PjsipCB *myCb = (PjsipCB*) globalPjsipCB;
                myCb->on_reg_state(rcv_acc_id);
	}
}

void PjsipCB::on_message_request(pjsua_call_id call_id, const pj_str_t *from,
                const pj_str_t *to, const pj_str_t *contact, const pj_str_t *mime_type,
                const pj_str_t *text) {
    PJ_UNUSED_ARG(call_id);
    PJ_UNUSED_ARG(to);
    PJ_UNUSED_ARG(contact);

    emit signal_on_message_request(QString::fromAscii(from->ptr,from->slen),
                QString::fromAscii(mime_type->ptr, mime_type->slen),
                    QString::fromAscii(text->ptr, text->slen));
}


void PjsipCB::static_on_message_request(pjsua_call_id call_id, const pj_str_t *from,
                const pj_str_t *to, const pj_str_t *contact, const pj_str_t *mime_type,
                const pj_str_t *text) {
    qDebug() << "received SIP MESSAGE request";
    if (globalPjsipCB) {
            PjsipCB *myCb = (PjsipCB*) globalPjsipCB;
            myCb->on_message_request(call_id, from, to, contact, mime_type, text);
    }
}

void PjsipCB::on_message_request_status(pjsua_call_id call_id,
                const pj_str_t *to, const pj_str_t *body, void *user_data,
                pjsip_status_code status, const pj_str_t *reason)
{
    PJ_UNUSED_ARG(call_id);
    PJ_UNUSED_ARG(body);
    PJ_UNUSED_ARG(user_data);

    emit signal_on_message_request_status(QString::fromAscii(to->ptr,to->slen),
                    (int)status, QString::fromAscii(reason->ptr, reason->slen));
}

void PjsipCB::static_on_message_request_status(pjsua_call_id call_id,
                const pj_str_t *to, const pj_str_t *body, void *user_data,
                pjsip_status_code status, const pj_str_t *reason)
{
    qDebug() << "received reply to SIP MESSAGE request";
    if (globalPjsipCB) {
            PjsipCB *myCb = (PjsipCB*) globalPjsipCB;
            myCb->on_message_request_status(call_id, to, body, user_data, status, reason);
    }
}

void PjsipCB::on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
                             pjsip_rx_data *rdata)
{
    pjsua_call_info ci;

    PJ_UNUSED_ARG(rdata);

    pjsua_call_get_info(call_id, &ci);
    qDebug() << "+ Incoming call from: "
             << QString::fromAscii(ci.remote_info.ptr, (int)ci.remote_info.slen);

    emit signal_on_incoming_call(acc_id, call_id,
                         QString::fromAscii(ci.remote_info.ptr, (int)ci.remote_info.slen));
}

/* Callback called by the library upon receiving incoming call */
void PjsipCB::static_on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
                             pjsip_rx_data *rdata)
{
    qDebug() << "received SIP call request";
    if (globalPjsipCB) {
            PjsipCB *myCb = (PjsipCB*) globalPjsipCB;
            myCb->on_incoming_call(acc_id, call_id, rdata);
    }
}

void PjsipCB::on_call_state(pjsua_call_id call_id, pjsip_event *e)
{
    pjsua_call_info ci;

    PJ_UNUSED_ARG(e);

    pjsua_call_get_info(call_id, &ci);

    qDebug() << "Call " << QString::number(call_id) << " state=" <<
                         QString::fromAscii(ci.state_text.ptr, (int)ci.state_text.slen);
    emit signal_on_call_state(call_id,
                         QString::fromAscii(ci.state_text.ptr, (int)ci.state_text.slen));
}

/* Callback called by the library when call's state has changed */
void PjsipCB::static_on_call_state(pjsua_call_id call_id, pjsip_event *e)
{
    qDebug() << "received call state update";
    if (globalPjsipCB) {
            PjsipCB *myCb = (PjsipCB*) globalPjsipCB;
            myCb->on_call_state(call_id, e);
    }
}

void PjsipCB::on_call_media_state(pjsua_call_id call_id)
{
    emit signal_on_call_media_state(call_id);
}

/* Callback called by the library when call's media state has changed */
void PjsipCB::static_on_call_media_state(pjsua_call_id call_id)
{
    qDebug() << "received call media state update";
    if (globalPjsipCB) {
            PjsipCB *myCb = (PjsipCB*) globalPjsipCB;
            myCb->on_call_media_state(call_id);
    }
}
