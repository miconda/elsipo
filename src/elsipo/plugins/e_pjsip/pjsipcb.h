#ifndef PJSIPCB_H_
#define PJSIPCB_H_

#include <QObject>

extern "C" {
	#include <pjsua-lib/pjsua.h>
}

class PjsipCB : public QObject {
Q_OBJECT

public:
	PjsipCB();
	virtual ~PjsipCB();

	void on_reg_state(pjsua_acc_id acc_id);
	static void static_on_reg_state(pjsua_acc_id acc_id);

        void on_message_request(pjsua_call_id call_id, const pj_str_t *from,
                        const pj_str_t *to, const pj_str_t *contact,
                        const pj_str_t *mime_type, const pj_str_t *text);
        static void static_on_message_request(pjsua_call_id call_id, const pj_str_t *from,
                        const pj_str_t *to, const pj_str_t *contact,
                        const pj_str_t *mime_type, const pj_str_t *text);
        void on_message_request_status(pjsua_call_id call_id,
                        const pj_str_t *to, const pj_str_t *body, void *user_data,
                        pjsip_status_code status, const pj_str_t *reason);
        static void static_on_message_request_status(pjsua_call_id call_id,
                        const pj_str_t *to, const pj_str_t *body, void *user_data,
                        pjsip_status_code status, const pj_str_t *reason);
        void on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
                                     pjsip_rx_data *rdata);
        static void static_on_incoming_call(pjsua_acc_id acc_id, pjsua_call_id call_id,
                                     pjsip_rx_data *rdata);
        void on_call_state(pjsua_call_id call_id, pjsip_event *e);
        static void static_on_call_state(pjsua_call_id call_id, pjsip_event *e);
        void on_call_media_state(pjsua_call_id call_id);
        static void static_on_call_media_state(pjsua_call_id call_id);


signals:
        void signal_on_reg_state(int rcv_acc_id);
        void signal_on_message_request(QString from, QString ctype, QString body);
        void signal_on_message_request_status(QString to, int code, QString reason);
        void signal_on_incoming_call(int rcv_acc_id, int call_id, QString from);
        void signal_on_call_state(int call_id, QString state);
        void signal_on_call_media_state(int call_id);

};

#endif /*PJSIPCB_H_*/
