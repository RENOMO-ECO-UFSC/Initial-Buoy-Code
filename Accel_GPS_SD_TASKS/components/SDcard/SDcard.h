#ifndef SDcard_H
#define SCcard_H

#include <string.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include "esp_vfs_fat.h"
#include "sdmmc_cmd.h"
#include <iostream>
#include <fstream>

#define EXAMPLE_MAX_CHAR_SIZE    64

static const char *TAG = "example";

#define MOUNT_POINT "/sdcard"

// Pin assignments can be set in menuconfig, see "SD SPI Example Configuration" menu.
// You can also change the pin assignments here by changing the following 4 lines.
#define PIN_NUM_MISO  GPIO_NUM_19
#define PIN_NUM_MOSI  GPIO_NUM_23
#define PIN_NUM_CLK   GPIO_NUM_18
#define PIN_NUM_CS    GPIO_NUM_5
#define SDMMC_FREQ_DEFAULT 15000

class SDcard
{
    private: 
        const char *path;
        char *data;
        sdmmc_card_t *card;
        FILE *f;

    public:
        SDcard();
        esp_err_t SDcard_init();
        esp_err_t s_example_open_file(const char *path);
        esp_err_t s_example_write_file(const char *path, char *data);
        esp_err_t s_example_read_file(const char *path);
        esp_err_t SDcard_close();


};



#endif
