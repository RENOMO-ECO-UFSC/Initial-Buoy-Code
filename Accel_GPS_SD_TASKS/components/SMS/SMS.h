#ifndef SMS_H
#define SMS_H

extern "C" {
#include "sim800l_cmds.h"
#include "sim800l.h"
}

class SMS
{

    public:
    SMS();
    void GSM_init();
    void sendSMS();

};

#endif