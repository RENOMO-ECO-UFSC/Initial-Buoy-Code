#include "ring_buffer.h"

/**
 * C++ wrapper for FreeRTOS's ring buffer
 */

Ringbuffer::Ringbuffer(size_t length, RingbufferType_t type) {
	m_handle = ::xRingbufferCreate(length, type);
} // Ringbuffer


Ringbuffer::~Ringbuffer() {
	::vRingbufferDelete(m_handle);
} // ~Ringbuffer


/**
 * @brief Receive data from the buffer.
 * @param [out] size On return, the size of data returned.
 * @param [in] wait How long to wait.
 * @return A pointer to the storage retrieved.
 */
void* Ringbuffer::receive(size_t* size, TickType_t wait) {
	return ::xRingbufferReceive(m_handle, size, wait);
} // receive


/**
 * @brief Return an item.
 * @param [in] item The item to be returned/released.
 */
void Ringbuffer::returnItem(void* item) {
	::vRingbufferReturnItem(m_handle, item);
} // returnItem


/**
 * @brief Send data to the buffer.
 * @param [in] data The data to place into the buffer.
 * @param [in] length The length of data to place into the buffer.
 * @param [in] wait How long to wait before giving up.  The default is to wait indefinitely.
 * @return
 */
bool Ringbuffer::send(void* data, size_t length, TickType_t wait) {
	return ::xRingbufferSend(m_handle, data, length, wait) == pdTRUE;
} // send

size_t Ringbuffer::getSize() {
return ::xRingbufferGetCurFreeSize(m_handle);
}