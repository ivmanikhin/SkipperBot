#include "ssd1306.h"
#include "driver/i2c.h"
#include "esp_timer.h"
#include "driver/uart.h"
#include "driver/gpio.h"
#include "esp_log.h"
#include <stdlib.h>
#include <string.h>
#include "parser.c"
#include <esp_heap_caps.h>
#include <qmc5883l.h>



// define I2C display consts:
#define I2C_DISPLAY_SCL_IO 19        /*!< gpio number for I2C master clock */
#define I2C_DISPLAY_SDA_IO 18        /*!< gpio number for I2C master data  */
#define I2C_DISPLAY_NUM I2C_NUM_1    /*!< I2C port number for master dev */
#define I2C_DISPLAY_FREQ_HZ 200000   /*!< I2C master clock frequency */
static ssd1306_handle_t ssd1306_dev = NULL;
#define I2C_COMPASS_SCL_IO 22        /*!< gpio number for I2C master clock */
#define I2C_COMPASS_SDA_IO 21        /*!< gpio number for I2C master data  */
#define I2C_COMPASS_NUM I2C_NUM_0    /*!< I2C port number for master dev */
#define I2C_COMPASS_FREQ_HZ 400000   /*!< I2C master clock frequency */
qmc5883l_t compass;
// define GPS module consts:
#define TXD_PIN (GPIO_NUM_17)
#define RXD_PIN (GPIO_NUM_16)
#define UART UART_NUM_2
static const int RX_BUF_SIZE = 1024;

// define compass consts:



void setup()
{
	//scan();
	i2cdev_init();

// setup i2c display:
    i2c_config_t display_conf;
    display_conf.mode = I2C_MODE_MASTER;
    display_conf.sda_io_num = (gpio_num_t)I2C_DISPLAY_SDA_IO;
    display_conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    display_conf.scl_io_num = (gpio_num_t)I2C_DISPLAY_SCL_IO;
    display_conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    display_conf.master.clk_speed = I2C_DISPLAY_FREQ_HZ;
    display_conf.clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL;
    i2c_param_config(I2C_DISPLAY_NUM, &display_conf);
    i2c_driver_install(I2C_DISPLAY_NUM, display_conf.mode, 0, 0, 0);
    ssd1306_dev = ssd1306_create(I2C_DISPLAY_NUM, SSD1306_I2C_ADDRESS);

// setup GPS module NEO-6M
    const uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
		.source_clk = UART_SCLK_DEFAULT,
    };
    uart_driver_install(UART, RX_BUF_SIZE*2, 0, 20, NULL, 0);
    uart_param_config(UART, &uart_config);
    uart_set_pin(UART, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
    uart_enable_pattern_det_baud_intr(UART, 0x0a, 1, 9, 0, 0);

// setup i2c compass:
    // i2c_config_t compass_conf;
    // compass_conf.mode = I2C_MODE_MASTER;
    // compass_conf.sda_io_num = (gpio_num_t)I2C_COMPASS_SDA_IO;
    // compass_conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    // compass_conf.scl_io_num = (gpio_num_t)I2C_COMPASS_SCL_IO;
    // compass_conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    // compass_conf.master.clk_speed = I2C_COMPASS_FREQ_HZ;
    // compass_conf.clk_flags = I2C_SCLK_SRC_FLAG_FOR_NOMAL;
    // i2c_param_config(I2C_COMPASS_NUM, &compass_conf);
    // i2c_driver_install(I2C_COMPASS_NUM, compass_conf.mode, 0, 0, 0);
	
	memset(&compass, 0, sizeof(qmc5883l_t));
	if (qmc5883l_init_desc(&compass, QMC5883L_I2C_ADDR_DEF, 0, I2C_COMPASS_SDA_IO, I2C_COMPASS_SCL_IO) == ESP_OK)
	{
		printf("compass init OK\n");
	};
	if (qmc5883l_set_config(&compass, QMC5883L_DR_50, QMC5883L_OSR_128, QMC5883L_RNG_2) == ESP_OK)
	{
		printf("compass comfig OK\n");
	};
	// if (hmc5883l_set_opmode(&compass, HMC5883L_MODE_CONTINUOUS) == ESP_OK)
	// {
	// 	printf("compass opmode has been set\n");
	// };
	// if (hmc5883l_set_samples_averaged(&compass, HMC5883L_SAMPLES_8) == ESP_OK)
	// {
	// 	printf("compass samples averaged have beed set\n");
	// };
	// if (hmc5883l_set_data_rate(&compass, HMC5883L_DATA_RATE_07_50) == ESP_OK)
	// {
	// 	printf("compass data rate has been set\n");
	// };
	// if (hmc5883l_set_gain(&compass, HMC5883L_GAIN_1090) == ESP_OK)
	// {
	// 	printf("compass gain has been set\n");
	// };

};




static void rx_task(void *arg)
{
	static qmc5883l_data_t compass_data;
	static char display_text_1[32];
	static char display_text_2[32];
	static char display_text_3[32];
	static char display_text_4[32];
	static char display_text_5[32];
	static double latitude;
	static char lat_direction;
	static double longitude;
	static char long_direction;
	static float time;
	static int satellites_num;
	static float height;
	static float direction;
	static float speed;
	static char lat_degs[3];
	static char lat_mins[9];
	static char long_degs[4];
	static char long_mins[9];

	uint8_t* data = (uint8_t*) heap_caps_malloc(RX_BUF_SIZE+1, MALLOC_CAP_8BIT);
	char** gpgga = (char**) heap_caps_malloc(sizeof(char*) * 16, MALLOC_CAP_8BIT);
	for (int i=0; i<16; i++)
	{
		gpgga[i] = (char*) heap_caps_malloc(sizeof(char) * 129, MALLOC_CAP_8BIT);
	}
	char** gpvtg = (char**) heap_caps_malloc(sizeof(char*) * 16, MALLOC_CAP_8BIT);
	for (int i=0; i<16; i++)
	{
		gpvtg[i] = (char*) heap_caps_malloc(sizeof(char) * 129, MALLOC_CAP_8BIT);
	}
	char** raw_coords = (char**) heap_caps_malloc(sizeof(char*) * 16, MALLOC_CAP_8BIT);
	for (int i=0; i<16; i++)
	{
		raw_coords[i] = (char*) heap_caps_malloc(sizeof(char) * 129, MALLOC_CAP_8BIT);
	}
	char** raw_speed = (char**) heap_caps_malloc(sizeof(char*) * 16, MALLOC_CAP_8BIT);
	for (int i=0; i<16; i++)
	{
		raw_speed[i] = (char*) heap_caps_malloc(sizeof(char) * 129, MALLOC_CAP_8BIT);
	}
	while (1)
	{
		
		if (qmc5883l_get_data(&compass, &compass_data) == ESP_OK)
		{
			snprintf(display_text_4, sizeof(display_text_4), "compass data: %.2f, %.2f, %.2f\n", compass_data.x, compass_data.y, compass_data.z);
		}
		else
		{
			snprintf(display_text_4, sizeof(display_text_4), "can't read compass data");
		};
		printf("%s", display_text_4);
		const int rxBytes = uart_read_bytes(UART, data, RX_BUF_SIZE, pdMS_TO_TICKS(200));
		if (rxBytes > 0)
		{
			data[rxBytes] = '\0'; 

			split_by_2_separators(gpgga, (char*)data, "$GPGGA", "*", 0, 0);
			split_by_2_separators(gpvtg, (char*)data, "$GPVTG", "*", 0, 0);

			split_to_sentences(raw_coords, gpgga[0], ",", 0, 0);
			split_to_sentences(raw_speed, gpvtg[0], ",", 0, 0);

			get_substring(raw_coords[2], lat_degs, 0, 2);
			get_substring(raw_coords[2], lat_mins, 2, 8);
			get_substring(raw_coords[4], long_degs, 0, 3);
			get_substring(raw_coords[4], long_mins, 3, 8);
			
			latitude = (float) atof(lat_degs) + (float) atof(lat_mins)*0.016666667;
			lat_direction = raw_coords[3][0];
			longitude = (float) atof(long_degs) + (float) atof(long_mins)*0.016666667;
			long_direction = raw_coords[5][0];

			time = (float)atof(raw_coords[1]);
			satellites_num = (int)atoi(raw_coords[7]);
			height = (float)atof(raw_coords[9]);
			direction = (float)atof(raw_speed[1]);
			speed = (float)atof(raw_speed[7]);

			snprintf(display_text_1, sizeof(display_text_1), "%.5f%c  %.5f%c", latitude, lat_direction, longitude, long_direction);
			snprintf(display_text_2, sizeof(display_text_2), "Time: %.2f", time);
			snprintf(display_text_3, sizeof(display_text_3), "%i satellites", satellites_num);
			snprintf(display_text_5, sizeof(display_text_5), "Dir: %.1f Spd: %.1f", direction, speed);

			ssd1306_clear_screen(ssd1306_dev, 0x00);
			ssd1306_draw_string(ssd1306_dev, 2, 2, (uint8_t*)display_text_1, 12, 1);
			ssd1306_draw_string(ssd1306_dev, 2, 14, (uint8_t*)display_text_2, 12, 1);
			ssd1306_draw_string(ssd1306_dev, 2, 26, (uint8_t*)display_text_3, 12, 1);
			ssd1306_draw_string(ssd1306_dev, 2, 38, (uint8_t*)display_text_4, 12, 1);
			ssd1306_draw_string(ssd1306_dev, 2, 50, (uint8_t*)display_text_5, 12, 1);
			ssd1306_refresh_gram(ssd1306_dev);
		
		}
	}
	free(gpgga);
	free(gpvtg);
	free(raw_coords);
	free(raw_speed);
	free(data);
}




void app_main(void)
{
	setup();

    xTaskCreate(rx_task, "uart_rx_task", 1024 * 2, NULL, configMAX_PRIORITIES, NULL);

}
