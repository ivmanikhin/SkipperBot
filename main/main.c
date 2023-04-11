#include "ssd1306.h"
#include "driver/i2c.h"
#include "esp_timer.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include <stdlib.h>
#include <string.h>
#include "parser.c"





// define I2C display variables:
#define I2C_MASTER_SCL_IO 22        /*!< gpio number for I2C master clock */
#define I2C_MASTER_SDA_IO 21        /*!< gpio number for I2C master data  */
#define I2C_MASTER_NUM I2C_NUM_0    /*!< I2C port number for master dev */
#define I2C_MASTER_FREQ_HZ 200000   /*!< I2C master clock frequency */
#define I2C_HOST  0
static ssd1306_handle_t ssd1306_dev = NULL;

//char data_str_2[16] = {0};



#define TXD_PIN (GPIO_NUM_17)
#define RXD_PIN (GPIO_NUM_16)
#define UART UART_NUM_2
static const int RX_BUF_SIZE = 1024;
//#define USONIC_TRIGGER 5
//#define USONIC_ECHO 18
//#define SOUND_SPEED 0.0343
//double cm;
//int16_t start_time, stop_time, duration;




void setup()
{
//	esp_rom_gpio_pad_select_gpio(USONIC_TRIGGER);
//	gpio_set_direction(USONIC_TRIGGER, GPIO_MODE_OUTPUT);
//	esp_rom_gpio_pad_select_gpio(USONIC_ECHO);
//	gpio_set_direction(USONIC_ECHO, GPIO_MODE_INPUT);
//	gpio_install_isr_service(0);

// setup i2c display:
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = (gpio_num_t)I2C_MASTER_SDA_IO;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = (gpio_num_t)I2C_MASTER_SCL_IO;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_MASTER_FREQ_HZ;
    conf.clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL;
    i2c_param_config(I2C_MASTER_NUM, &conf);
    i2c_driver_install(I2C_MASTER_NUM, conf.mode, 0, 0, 0);
    ssd1306_dev = ssd1306_create(I2C_MASTER_NUM, SSD1306_I2C_ADDRESS);

//  const int UART = UART_2;

// setup UART:
    const uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.source_clk = UART_SCLK_DEFAULT,
//        .rx_flow_ctrl_thresh = 122,
    };

    uart_driver_install(UART, RX_BUF_SIZE*2, 0, 20, NULL, 0);
    uart_param_config(UART, &uart_config);
    uart_set_pin(UART, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_enable_pattern_det_baud_intr(UART, 0x0a, 1, 9, 0, 0);
//    uart_pattern_queue_reset(UART, 1, 20);
};




static void rx_task(void *arg)
{

	uint8_t* data = (uint8_t*) malloc (RX_BUF_SIZE+1);
	// static char** gpgga = (char**)malloc(sizeof(char*));
	// gpgga[0] = (char*)malloc(sizeof(char)*129);
	// static char** gpvtg = (char**)malloc(sizeof(char*));
	// gpvtg[0] = (char*)malloc(sizeof(char)*129);
	//static const char *RX_TASK_TAG = "RX_TASK";
	//esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
	static char display_text_1[64];
	static char display_text_2[64];
	static char display_text_3[64];
	static char display_text_4[64];
	static char display_text_5[64];
	while (1)
	{
		const int rxBytes = uart_read_bytes(UART, data, RX_BUF_SIZE, pdMS_TO_TICKS(200));
		if (rxBytes > 0)
		{
			data[rxBytes] = '\0';
			//int coords_num = 0;
			printf("%s\n\n", data);

			char** gpgga = split_by_2_separators((char*)data, "$GPGGA", "*", 0, 0);
			char** gpvtg = split_by_2_separators((char*)data, "$GPVTG", "*", 0, 0);


			//split_by_2_separators(gpgga, (char*)data, "$GNGGA", "*", 0);


			char** raw_coords = split_to_sentences(gpgga[0], ",", 0, 0);
			char** raw_speed = split_to_sentences(gpvtg[0], ",", 0, 0);
		    // for (int i = 0; i < 10; i++)
		    // {
		    //     memmove(raw_coords[i], raw_coords[i]+1, strlen(raw_coords[i]));
		    // }
			// for (int i = 0; i < 10; i++)
		    // {
		    //     memmove(raw_speed[i], raw_speed[i]+1, strlen(raw_speed[i]));
		    // }

					    //get_substring((char*) data, display_text_1, 0, 70);

			// char lat_degs[2];
			// char lat_mins[7];
			// get_substring(raw_coords[2], lat_degs, 0, 2);
			// get_substring(raw_coords[2], lat_mins, 2, 7);

			// char long_degs[3];
			// char long_mins[7];
			// get_substring(raw_coords[4], long_degs, 0, 3);
			// get_substring(raw_coords[4], long_mins, 3, 7);

			// float latitude = (float) atoll(lat_degs) + (float) atoll(lat_mins)*0.166667;
			// char lat_direction = raw_coords[3][0];
			// float longitude = (float) atoll(long_degs) + (float) atoll(long_mins)*0.166667;
			// char long_direction = raw_coords[5][0];

			float latitude = (float) atoll(raw_coords[2]);
			char lat_direction = raw_coords[3][0];
			float longitude = (float) atoll(raw_coords[4]);
			char long_direction = raw_coords[5][0];


			float time = (float) atoll(raw_coords[1]);
			int satellites_num = (int) atoi(raw_coords[7]);
			float height = (float)atoll(raw_coords[9]);
			float direction = (float)atoll(raw_speed[1]);
			float speed = (float)atoll(raw_speed[7]);
			snprintf(display_text_1, sizeof(display_text_1), "%.2f.%c  %.2f.%c", latitude, lat_direction, longitude, long_direction);
			snprintf(display_text_2, sizeof(display_text_2), "Time: %.2f", time);
			snprintf(display_text_3, sizeof(display_text_3), "%i satellites", satellites_num);
			snprintf(display_text_4, sizeof(display_text_4), "Height:%.1f", height);
			snprintf(display_text_5, sizeof(display_text_5), "Dir: %.1f Spd: %.1f", direction, speed);

//		    else
//		    {
//		    	strcpy(display_text, "Processing...");
//		    }


			//ESP_LOGI(RX_TASK_TAG, "Read %d bytes: '%s'", rxBytes, data);
			ssd1306_clear_screen(ssd1306_dev, 0x00);
			ssd1306_draw_string(ssd1306_dev, 2, 2, (uint8_t*)display_text_1, 12, 1);
			ssd1306_draw_string(ssd1306_dev, 2, 14, (uint8_t*)display_text_2, 12, 1);
			ssd1306_draw_string(ssd1306_dev, 2, 26, (uint8_t*)display_text_3, 12, 1);
			ssd1306_draw_string(ssd1306_dev, 2, 38, (uint8_t*)display_text_4, 12, 1);
			ssd1306_draw_string(ssd1306_dev, 2, 50, (uint8_t*)display_text_5, 12, 1);
			ssd1306_refresh_gram(ssd1306_dev);
			//vTaskDelay(pdMS_TO_TICKS(50));
		}
	}
	free(data);
}




void app_main(void)
{
	setup();
//	vTaskDelay(pdMS_TO_TICKS(3000));

    ssd1306_refresh_gram(ssd1306_dev);
//    sprintf(data_str_1, "Distance:");
//    int i;
    xTaskCreate(rx_task, "uart_rx_task", 1024 * 2, NULL, configMAX_PRIORITIES, NULL);
//    while(1)
//    {
////    	cm = 0;
////    	for (i=0; i<10; i++)
////    	{
////    		cm = mesure_distance();
////    		vTaskDelay(50 / portTICK_PERIOD_MS);
////    	}
////    	uart_read
////    	cm =  0.1 * cm;
////        sprintf(data_str_2, "%0.02f cm", cm);
//    }
}
