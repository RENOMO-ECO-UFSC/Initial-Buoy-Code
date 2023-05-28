#include <stdio.h>
#include "esp_log.h"
#include "mpu6050.h"
#include "SDcard.h"
#include "cppi2c.h"
#include <cstring>
#include "ublox7/ublox7.h"
#include "cppuart.h"
#include "SMS.h"
#include "ring_buffer.h"

static const char *TAG1 = "MPU6050";

//RTC_DATA_ATTR circbuf queue;

TaskHandle_t ACCELHandle;
TaskHandle_t GPSHandle;
TaskHandle_t GSMHandle;
TaskHandle_t SDHandle;
TaskHandle_t OnceHandle;

CPPUART::Uart uart {UART_NUM_1};
CPPI2C::I2c i2c {I2C_NUM_0};
MPU6050 device(0x68);
UBLOX7 gps(UART_NUM_1);
SDcard sdcard;
Ringbuffer Ringbuf(4096);
SMS sms;

using namespace std;
char buffer[EXAMPLE_MAX_CHAR_SIZE];
const char *file_hello = MOUNT_POINT"/hello.txt";

static void Initialise_Task(void *arg)
{
	ESP_ERROR_CHECK(i2c.InitMaster(I2C_MASTER_SDA_IO, I2C_MASTER_SCL_IO, I2C_MASTER_FREQ_HZ, 1, 1, 0));
	ESP_LOGI(TAG1, "I2C initialized successfully");
	ESP_ERROR_CHECK(uart.uart_init(UB7_BAUD_9600, UB7_TXD, UB7_RXD, UB7_EN)); 
    ESP_ERROR_CHECK(gps.ub7_set_nav_mode(UB7_MODE_PEDESTRIAN));
    ESP_ERROR_CHECK(gps.ub7_set_message(UB7_MSG_RMC, false));
    ESP_ERROR_CHECK(gps.ub7_set_message(UB7_MSG_VTG, false));
    ESP_ERROR_CHECK(gps.ub7_set_message(UB7_MSG_GSA, false));
    ESP_ERROR_CHECK(gps.ub7_set_message(UB7_MSG_GLL, false));
    ESP_ERROR_CHECK(gps.ub7_set_message(UB7_MSG_GSV, false));
    ESP_ERROR_CHECK(gps.ub7_set_output_rate(UB7_OUTPUT_1HZ));
	ESP_ERROR_CHECK(sdcard.SDcard_init());
	sms.GSM_init();
	//sdcard.s_example_open_file(file_hello);
	vTaskDelete(NULL);
}

// Create the task
void Once_Task(void *arg)
{
	TaskHandle_t InitialiseHandle;
	xTaskCreate(Initialise_Task, "Initialise_Task", 4096, NULL, 10, &InitialiseHandle);
	vTaskDelay(500/portTICK_PERIOD_MS);
	vTaskDelete(NULL);
}

void ACCEL_Task(void *arg)
{	
	float ax, ay, az; //Variables to store the accel values
 	TickType_t xLastWakeTime;
 	const TickType_t xFrequency = 10; // read ACCEL every tenth of a second
    // Initialise the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();
    for( ;; )
    {
        // Wait for the next cycle.
        vTaskDelayUntil( &xLastWakeTime, xFrequency );
		device.getAccel(&ax, &ay, &az);
		cout << "Accelerometer Readings: X: " << ax << ", Y: " << ay << ", Z: " << az << "\n";
        snprintf(buffer, EXAMPLE_MAX_CHAR_SIZE, "%g\n", ay );
        //insert(queue, buffer);
		Ringbuf.send(buffer, sizeof(buffer));
	}
}

void GPS_Task(void *arg)
{
	int length = 0;
	TickType_t xLastWakeTime;
 	const TickType_t xFrequency = 100; // read GPS every second
    // Initialise the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();
    for( ;; )
    {
        // Wait for the next cycle.
        vTaskDelayUntil( &xLastWakeTime, xFrequency);
		uart_get_buffered_data_len(UART_NUM_1, (size_t*)&length);
		uint8_t GPSdata[length];
		uart_read_bytes(UART_NUM_1, &GPSdata, length, 100);
		cout << "Length: " << length << "\n";
    	cout << "Data: " << GPSdata << "\n";
		Ringbuf.send(GPSdata, length);
	}
}

void GSM_Task(void *arg) // number can be changed in sim800l/include/sim800l_cmds
{
	int length = 0;
	uint8_t CTRL_Z[]={0x1a}; 
	TickType_t xLastWakeTime;
 	const TickType_t xFrequency = 100; // send message every second
    // Initialise the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();
    for( ;; )
    {
        // Wait for the next cycle.
        vTaskDelayUntil( &xLastWakeTime, xFrequency);
		uart_get_buffered_data_len(UART_NUM_1, (size_t*)&length);
		uint8_t GPSdata[length];
		uart_read_bytes(UART_NUM_1, &GPSdata, length, 100);
		runSingleGSMCommand(&confTEXT);
		runSingleGSMCommand(&confSETUP);
		runSingleGSMCommand(&confPHONENUMBER);
		uart_write_bytes(UART_NUM_2, &GPSdata, length);
		uart_write_bytes(UART_NUM_2, CTRL_Z, sizeof(CTRL_Z)); // return to send message
		uart_wait_tx_done(UART_NUM_2, 200 / portTICK_PERIOD_MS);
	}
}

void SD_Task(void *arg)
{	
	esp_err_t ret;
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 100; // write every second
	size_t item_size;
    // Initialise the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();
    for( ;; )
    {
        // Wait for the next cycle.
        vTaskDelayUntil( &xLastWakeTime, xFrequency);
		while(Ringbuf.getSize() < 2000){
		char *item = (char *)Ringbuf.receive(&item_size);
		Ringbuf.returnItem(item);
		ret = sdcard.s_example_write_file(file_hello, item);
        	if (ret != ESP_OK) {
             	return;
        	}
		}
	//sdcard.SDcard_close();
	}
}

extern "C" void app_main(void)
{	
	TaskHandle_t OnceHandle1 = NULL;
	xTaskCreate(Once_Task, "Once_Task", 4096, NULL, 28, &OnceHandle1);
	vTaskDelay(12000/portTICK_PERIOD_MS); // wait for initialisation to complete
		
	xTaskCreatePinnedToCore(ACCEL_Task, "ACCEL_Task", 4096, &device, 10, &ACCELHandle, 0);
	xTaskCreatePinnedToCore(GPS_Task, "GPS_Task", 4096, NULL, 10, &GPSHandle, 0);
	xTaskCreatePinnedToCore(GSM_Task, "Gsm_Task", 4096, NULL, 15, &GSMHandle, 0);
	xTaskCreatePinnedToCore(SD_Task, "SD_Task", 4096, NULL, 10, &SDHandle, 1);
}