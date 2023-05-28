#ifndef RINGBUF_H
#define RINGBUF_H

extern "C" {
#include <freertos/FreeRTOS.h>   // Include the base FreeRTOS definitions.
#include <freertos/ringbuf.h>    // Include the ringbuffer definitions.
}

class Ringbuffer {
public:
	Ringbuffer(size_t length, RingbufferType_t type = RINGBUF_TYPE_NOSPLIT);
	~Ringbuffer();

	void*    receive(size_t* size, TickType_t wait = portMAX_DELAY);
	void     returnItem(void* item);
	bool     send(void* data, size_t length, TickType_t wait = portMAX_DELAY);
	size_t   getSize();
private:
	RingbufHandle_t m_handle;
};

#endif