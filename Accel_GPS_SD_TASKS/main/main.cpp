#include <stdio.h>
#include "esp_log.h"
#include "mpu6050.h"
#include "SDcard.h"
#include "cppi2c.h"
#include "dataStore.h"
#include "circbuf.h"
#include <cstring>
#include "ublox7/ublox7.h"
#include "cppuart.h"

static TaskHandle_t xTaskHandle;
static StackType_t xTaskStack[configMINIMAL_STACK_SIZE];
static StaticTask_t xTaskTCB;

static const char *TAG1 = "MPU6050";

#define TIMEOUT_MS		1000
#define DELAY_MS		1000

RTC_DATA_ATTR circbuf queue;

TaskHandle_t ACCELHandle;
TaskHandle_t GPSHandle;
TaskHandle_t SDHandle;

TaskHandle_t OnceHandle;

CPPUART::Uart uart {UART_NUM_1};
CPPI2C::I2c i2c {I2C_NUM_0};
MPU6050 device(0x68);
UBLOX7 gps(UART_NUM_1);
SDcard sdcard;

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
	
	allocate(queue, 30);
	
	//sdcard.s_example_open_file(file_hello);
	vTaskDelete(NULL);

}

// Create the task
void Once_Task(void *arg)
{
	TaskHandle_t InitialiseHandle;
	xTaskCreate(Initialise_Task, "Initialise_Task", 4096, NULL, 10, &InitialiseHandle);
	vTaskDelay(5000/portTICK_PERIOD_MS);
	vTaskDelete(NULL);

}

void ACCEL_Task(void *arg)
{	
	float ax, ay, az; //Variables to store the accel values
 	TickType_t xLastWakeTime;
 	const TickType_t xFrequency = 10;
    // Initialise the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();
	
     for( ;; )
     {
        // Wait for the next cycle.
        vTaskDelayUntil( &xLastWakeTime, xFrequency );
		device.getAccel(&ax, &ay, &az);
		cout << "Accelerometer Readings: X: " << ax << ", Y: " << ay << ", Z: " << az << "\n";
        snprintf(buffer, EXAMPLE_MAX_CHAR_SIZE, "%g\n", ay );
        insert(queue, buffer);
	}
}

void GPS_Task(void *arg)
{
	int length = 0;
	TickType_t xLastWakeTime;
 	const TickType_t xFrequency = 100;
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
		insert(queue, (char*)GPSdata);
	}
}

void SD_Task(void *arg)
{	
	//const char *file_hello = MOUNT_POINT"/hello.txt";
	esp_err_t ret;
	std::string w;
	/*
	TickType_t xLastWakeTime;
	const TickType_t xFrequency = 100;
    // Initialise the xLastWakeTime variable with the current time.
    xLastWakeTime = xTaskGetTickCount();
     for( ;; )
     {
        // Wait for the next cycle.
        vTaskDelayUntil( &xLastWakeTime, xFrequency );
		*/
		int size = 0;
		cout << "queue outside infinite loop:" << size << "\n";
	while(1){
			//while(size != 0){
			remove(queue, w);
			size = getCurrSize(queue);
			cout << "queue size AFTER remove:" << size << "\n";
			strcpy(buffer, w.c_str());
			ret = sdcard.s_example_write_file(file_hello, buffer);
        	if (ret != ESP_OK) {
             	return;
        	}
			//}
		
	//sdcard.SDcard_close();
	}
}

extern "C" void app_main(void)
{	

	TaskHandle_t OnceHandle1 = NULL;
	xTaskCreate(Once_Task, "Once_Task", 4096, NULL, 1, &OnceHandle1);
	vTaskDelay(500/portTICK_PERIOD_MS);
	vTaskDelete(OnceHandle1);
	
	
	xTaskCreatePinnedToCore(ACCEL_Task, "ACCEL_Task", 4096, &device, 10, &ACCELHandle, 0);
	xTaskCreatePinnedToCore(GPS_Task, "GPS_Task", 4096, NULL, 10, &GPSHandle, 0);
	xTaskCreatePinnedToCore(SD_Task, "SD_Task", 4096, NULL, 9, &SDHandle, 1);
	
}
