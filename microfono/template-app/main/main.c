/* Hello World Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include <string.h>
#include "sdkconfig.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "esp_spi_flash.h"
#include <driver/adc.h>
#include "esp_adc_cal.h"
#include "driver/i2s.h"
#include "esp_now.h"
#include <driver/dac.h>

#define SAMPLE_BUFFER_SIZE 512
#define SAMPLE_RATE 8000
//#define SAMPLE_RATE 44100
// most microphones will probably default to left channel but you may need to tie the L/R pin low
#define I2S_MIC_CHANNEL I2S_CHANNEL_FMT_ONLY_LEFT
// #define I2S_MIC_CHANNEL I2S_CHANNEL_FMT_ONLY_RIGHT
#define I2S_MIC_SERIAL_CLOCK GPIO_NUM_18
#define I2S_MIC_LEFT_RIGHT_CLOCK GPIO_NUM_22
#define I2S_MIC_SERIAL_DATA GPIO_NUM_21

uint32_t raw_samples[SAMPLE_BUFFER_SIZE];
uint8_t raw_samples8bit[SAMPLE_BUFFER_SIZE * 4];
uint8_t dac_buffer[SAMPLE_BUFFER_SIZE];
uint8_t low;
uint8_t mid;
uint8_t msb;

int prev_val = 0;
int noise = 300;
esp_adc_cal_characteristics_t *adc_chars;

static const i2s_config_t i2s_mic_Config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_MIC_CHANNEL,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 1024,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0};

// i2s microphone pins
static const i2s_pin_config_t i2s_mic_pins = {
    .bck_io_num = I2S_MIC_SERIAL_CLOCK,
    .ws_io_num = I2S_MIC_LEFT_RIGHT_CLOCK,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_MIC_SERIAL_DATA};
/*
static const i2s_config_t i2s_config = {
        .mode = (I2S_MODE_MASTER | I2S_MODE_TX | 16),
        .sample_rate =  SAMPLE_RATE,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
	    .communication_format = (0x01 | 0x02),  //LSB for internal DAC!!!!
	    .channel_format =  I2S_CHANNEL_FMT_RIGHT_LEFT,   //Both channels
	    .intr_alloc_flags = 0,
	    .dma_buf_count = 16,
	    .dma_buf_len = 256,
	    .use_apll = false,
};*/

//incluir en alguna subrutina
//m_input = new I2SMEMSSampler(I2S_NUM_0, i2s_mic_pins, i2s_mic_Config,128);
//m_output = new DACOutput(I2S_NUM_0);

void setup_i2s() {
    if (ESP_OK != i2s_driver_install(I2S_NUM_0, &i2s_mic_Config, 0, NULL)) { //setup i2s for mic input
        printf("i2s_mic_driver_install: error");
    }

    if (ESP_OK != i2s_set_pin(I2S_NUM_0, &i2s_mic_pins)) {//setup i2s pins for input
        printf("i2s_mic_set_pin: error");
    }
/*
      //install and start i2s driver
    if (i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL) != ESP_OK) {
        printf("Failed installing DAC driver");
        while (true);
    }
    //for internal DAC, this will enable both of the internal channels
    if (i2s_set_pin(I2S_NUM_0, NULL) != ESP_OK) {
        printf("Failed setting DAC pin");
        while (true);
    }*/

}

void app_main(void)
{
    setup_i2s(); //initialize i2s and dac peripherals 
    dac_output_enable(DAC_CHANNEL_1);
    

    while(1){
        size_t bytes_read = 0;
        
        i2s_read(I2S_NUM_0, raw_samples, sizeof(uint32_t) * SAMPLE_BUFFER_SIZE, &bytes_read, portMAX_DELAY);
        int samples_read = bytes_read / sizeof(uint32_t);
        //i2s_read(I2S_NUM_0, &raw_samples8bit, sizeof(raw_samples8bit), &bytes_read, portMAX_DELAY);
        //int samples_read = bytes_read / 4;

        // dump the samples out to the serial channel.
        /*
        for (int i = 0; i < samples_read; i++)
        {

            //printf("Data: %u\n",raw_samples[i]);
            dac_buffer[i] = (uint8_t)raw_samples[i] >> 24;
            printf("Data: %u\n",dac_buffer[i]);
            //printf("Data: %u\n",raw_samples[i]);
            //vTaskDelay(500 / portTICK_PERIOD_MS);
            dac_output_voltage(DAC_CHANNEL_1, dac_buffer[i]);
        }*/
        for (int i=0; i<samples_read; i++) {
            int max = 0;
            int min = 0;
            float dat_micro = 0;

            if (raw_samples[i] < min){
                min = raw_samples[i];
            }
            if (raw_samples[i] > max){
                max = raw_samples[i];
            }
            dat_micro = (raw_samples[i] - min)/(max - min);
            printf("%f",dat_micro);
            /*
            low = raw_samples8bit[i * 4 + 1];
            mid = raw_samples8bit[i * 4 + 2];
            msb = raw_samples8bit[i * 4 + 3];
             printf("Data: %u\n",msb);
             vTaskDelay(125 / (portTICK_PERIOD_MS*1000));
             dac_output_voltage(DAC_CHANNEL_1, msb);*/


        }
    }
    /*
    size_t bytesRead = 0;
    uint8_t buffer32[ESP_NOW_MAX_DATA_LEN * 4] = {0};
    i2s_read(I2S_NUM_0, &buffer32, sizeof(buffer32), &bytesRead, 100);
    int samplesRead = bytesRead / 4;
    
    
    int16_t buffer16[ESP_NOW_MAX_DATA_LEN] = {0};
    for (int i=0; i<samplesRead; i++) {
        buffer32 = buffer32 + 32768;
        uint8_t mid = buffer32[i * 4 + 2];
        uint8_t msb = buffer32[i * 4 + 3];
        uint16_t raw = (((uint32_t)msb) << 8) + ((uint32_t)mid) ;
        memcpy(&buffer16[i], &raw, sizeof(raw)); // Copy so sign bits aren't interfered with somehow.
    }
    
    dac_output_enable(DAC_CHANNEL_1); //enable DAC1, pin GPIO25(ESP32)
    dac_output_enable(DAC_CHANNEL_2);//DAC channel 2 is GPIO26
    while (1) {
        for (int i = 0; i < ESP_NOW_MAX_DATA_LEN; i++){ //N defined in test.h
            //dac_output_voltage(DAC_CHANNEL_1, sine[i]);
            dac_output_voltage(DAC_CHANNEL_1, buffer16[i]);
            //vTaskDelay(Ts/portTICK_PERIOD_MS); //delay 1ms
            vTaskDelay(1/SAMPLE_RATE);
            dac_output_voltage(DAC_CHANNEL_2, buffer16[i]);
            vTaskDelay(1/SAMPLE_RATE);
        }
    }*/
    /*
    xTaskCreate(generate_data_onthefly, "generate Stereo Audio", 4096, NULL, -1, NULL);
    xTaskCreate(replay_onthefly, "Replay Stereo Audio", 4096, NULL, 5, NULL);
    
    while(1){
        for (int i = 0; i < SIGNAL_LENGTH; i++){ //N defined in test.h
            dac_output_voltage(DAC_CHANNEL_1, DacR[i]);
            vTaskDelay(1/portTICK_PERIOD_MS);
            //dac_output_voltage(DAC_CHANNEL_1, dataint[i]);
            //vTaskDelay(Ts/portTICK_PERIOD_MS); //delay 1ms
        }
    }
    */
}

