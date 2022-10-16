#include <driver/i2s.h>

// you shouldn't need to change these settings
#define SAMPLE_BUFFER_SIZE 512
#define SAMPLE_RATE 8000
// most microphones will probably default to left channel but you may need to tie the L/R pin low
#define I2S_MIC_CHANNEL I2S_CHANNEL_FMT_ONLY_LEFT
// either wire your microphone to the same pins or change these to match your wiring
#define I2S_MIC_SERIAL_CLOCK GPIO_NUM_18
#define I2S_MIC_LEFT_RIGHT_CLOCK GPIO_NUM_22
#define I2S_MIC_SERIAL_DATA GPIO_NUM_21
#define DAC1 25


// don't mess around with this
i2s_config_t i2s_config = {
    .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
    .sample_rate = SAMPLE_RATE,
    .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
    .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
    .communication_format = I2S_COMM_FORMAT_I2S,
    .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
    .dma_buf_count = 4,
    .dma_buf_len = 1024,
    .use_apll = false,
    .tx_desc_auto_clear = false,
    .fixed_mclk = 0};

// and don't mess around with this
i2s_pin_config_t i2s_mic_pins = {
    .bck_io_num = I2S_MIC_SERIAL_CLOCK,
    .ws_io_num = I2S_MIC_LEFT_RIGHT_CLOCK,
    .data_out_num = I2S_PIN_NO_CHANGE,
    .data_in_num = I2S_MIC_SERIAL_DATA};

void setup()
{
  // we need serial output for the plotter
  Serial.begin(115200);
  // start up the I2S peripheral
  i2s_driver_install(I2S_NUM_0, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_NUM_0, &i2s_mic_pins);
}

int32_t raw_samples[SAMPLE_BUFFER_SIZE];
int32_t max_v = 1;
int32_t min_v = 0;
double dat_micro = 0;
char num_bits = 8;
double q = 1.0/(256.0-2.0);
int quant_sig  = 0;
double first;

void loop()
{
  
  // read from the I2S device
  size_t bytes_read = 0;
  i2s_read(I2S_NUM_0, raw_samples, sizeof(int32_t) * SAMPLE_BUFFER_SIZE, &bytes_read, portMAX_DELAY);
  int samples_read = bytes_read / sizeof(int32_t);
  // dump the samples out to the serial channel.
  for (int i = 0; i < samples_read; i++)
  {
    
    /*if (raw_samples[i] < min_v){
        min_v = raw_samples[i];
    }
    else if (raw_samples[i] > max_v){
        max_v = raw_samples[i];
    }
    dat_micro = (double)(raw_samples[i] - min_v)/(double)(max_v - min_v);
    //Serial.println(dat_micro,15);
    
    first = q*(double)round((double)dat_micro/q);
    first = first * (double)255.0;
    //Serial.print(first);
    quant_sig = round(first);
    Serial.println(quant_sig);
    dacWrite(DAC1, quant_sig);*/

    dacWrite(DAC1, raw_samples[i]/16);
    /*
    Serial.print(max_v - min_v);
    Serial.print("\t");
    Serial.println(raw_samples[i]);
    Serial.print("\t");*/
    
    //Serial.printf("%lf \t %d \t %d \n",dat_micro, min_v, max_v);
    //Serial.printf("%ld\n", raw_samples[i]);
  }
}
