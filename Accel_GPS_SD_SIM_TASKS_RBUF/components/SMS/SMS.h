#ifndef SMS_H
#define SMS_H

/**
 * C++ wrapper for sim800l code
 */

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