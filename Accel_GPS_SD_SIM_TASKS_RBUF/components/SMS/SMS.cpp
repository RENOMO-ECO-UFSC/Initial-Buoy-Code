#include "SMS.h"

/**
 * C++ wrapper for sim800l code
 */

static const char* TAG = "[SIM DRIVER]";

SMS::SMS(){};

void SMS::GSM_init(){
    configureUART();
    uint8_t res = ::runGSMCommands(GSM_Init, GSM_Init_CmdsSize);
	ESP_LOGW(TAG, "GSM Initilazation returned %d",res);
	::runSingleGSMCommand(&confTEXT);
	::runSingleGSMCommand(&confSETUP);
}

void SMS::sendSMS(){
    ::runSingleGSMCommand(&confPHONENUMBER);
}
