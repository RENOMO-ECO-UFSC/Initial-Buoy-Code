#include <driver/gpio.h>
#include "ublox7/ublox7.h"
#include "ublox7/ubxmsg.h"


UBLOX7::UBLOX7(uart_port_t port){
	_port = port;
}

esp_err_t UBLOX7::ub7_set_nav_mode(ub7_nav_mode_t mode){
	uint8_t nav_msg[44] = {UBX_HDR_A, UBX_HDR_B, CLS_CFG, CFG_NAV5, 
                                    /* payload size */ 0x24, 0x00,
                                    /* mask */ 0xFF, 0xFF,
                                    mode, /* auto 2D-3D */ 0x03,
                                    /* fixedAlt */ 0x00, 0x00, 0x00, 0x00,
                                    /* fixedAltVar */ 0x10, 0x27, 0x00, 0x00,
                                    /* minElev */ 0x05, /* drLimit */ 0x00,
                                    /* pDop */ 0xFA, 0x00,
                                    /* tDop */ 0xFA, 0x00,
                                    /* pAcc */ 0x64, 0x00,
                                    /* tAcc */ 0x2C, 0x01,
                                    /* staticHoldThresh */ 0x00,
                                    /* dgpsTimeOut */ 0x00,
                                    /* cnoThreshNumSVs */ 0x00,
                                    /* cnoThresh */ 0x00,
                                    /* reserved */ 0x00, 0x00, 0x00, 0x00, 0x00,
                                    /* reserved */ 0x00, 0x00, 0x00, 0x00, 0x00,
                                    /* chksum */ 0x00, 0x00};

	 add_checksum(nav_msg, sizeof(nav_msg), 
					&nav_msg[sizeof(nav_msg)-2], &nav_msg[sizeof(nav_msg)-1]);
	
	if(uart_write_bytes(_port, (char*)nav_msg,
						 sizeof(nav_msg)) != sizeof(nav_msg))
		return ESP_FAIL;

	return  ack_status(CLS_CFG, CFG_NAV5);
}

esp_err_t UBLOX7::ub7_set_output_rate(ub7_output_rate_t rate){
	uint8_t output_vec[2];

	switch(rate){
	case UB7_OUTPUT_1HZ:
		output_vec[0] = 0xE8;
		output_vec[1] = 0x03;
		break;
	case UB7_OUTPUT_2HZ:
		output_vec[0] = 0xF4;
		output_vec[1] = 0x01;
		break;
	case UB7_OUTPUT_5HZ:
		output_vec[0] = 0xC8;
		output_vec[1] = 0x00;
		break;
	default:
		return ESP_ERR_INVALID_ARG;
	}

	uint8_t out_msg[14] = {UBX_HDR_A, UBX_HDR_B, CLS_CFG, CFG_RATE, 0x06, 0x00,
							output_vec[0], output_vec[1],
							/* navRate always 1 */ 0x01, 0x00,
							/* timeRef UTC */ 0x01, 0x00,
							/* checksum */ 0x00, 0x00};

	 add_checksum(out_msg, 14, &out_msg[12], &out_msg[13]);

	if(uart_write_bytes(_port, (char*)out_msg,
						sizeof(out_msg)) != sizeof(out_msg))
		return ESP_FAIL;

	return  ack_status(CLS_CFG, CFG_RATE);
}

esp_err_t UBLOX7::ub7_set_message(ub7_message_t message, bool active)
{
	uint8_t msg_vec[11] = {UBX_HDR_A, UBX_HDR_B, CLS_CFG, CFG_MSG, 0x03, 0x00,
							NMEA_STD, message, active, 0x00, 0x00};
	 add_checksum(msg_vec, sizeof(msg_vec),
					&msg_vec[sizeof(msg_vec)-2], &msg_vec[sizeof(msg_vec)-1]);

	if(uart_write_bytes(_port, (char*)msg_vec,
							sizeof(msg_vec)) != sizeof(msg_vec))
		return ESP_FAIL;

	return  ack_status(CLS_CFG, CFG_MSG);
}

esp_err_t UBLOX7::ack_status(uint8_t cls_id, uint8_t msg_id)
{
	esp_err_t ret;
	uint8_t data = 0;
	uint8_t i = 0;
	size_t size = 0;
	uint8_t ack_pkt[10] = {UBX_HDR_A, UBX_HDR_B, CLS_ACK, ACK_ACK, 0x02, 0x00,
							cls_id, msg_id, 0x00, 0x00};

	uint32_t then = millis();
	while(i<10 && millis() - then < ACK_TIMEOUT){
		ret = uart_get_buffered_data_len(_port, &size);
		if(ret)
			return ret;
		if(size){
			if(!uart_read_bytes(_port, &data, 1, 20/portTICK_PERIOD_MS))
				return ESP_ERR_TIMEOUT;

			if(data == ack_pkt[i])
				i++;
			else if(i > 2){
				ack_pkt[i] = data;
				i++;
			}
		}
	}
	if(millis() - then >= 1500)
		return ESP_ERR_TIMEOUT;
	if(!ack_pkt[3])
		return ESP_ERR_INVALID_RESPONSE;	

	uint8_t CK_A = 0;
	uint8_t CK_B = 0;
	add_checksum(ack_pkt, 10, &CK_A, &CK_B);

	if(cls_id == ack_pkt[6] && msg_id == ack_pkt[7] && 
					CK_A == ack_pkt[8] && CK_B == ack_pkt[9])
		return ESP_OK;
	else
		return ESP_ERR_INVALID_CRC;
}

void UBLOX7::add_checksum(uint8_t *message, uint8_t size, uint8_t *CK_A, uint8_t *CK_B)
{
	for(uint8_t i = 2; i < size-2; i++){
		*CK_A = *CK_A + message[i];
		*CK_B = *CK_B + *CK_A;
	}
}

inline uint32_t UBLOX7::millis()
{
	return xTaskGetTickCount() * portTICK_PERIOD_MS;
}
