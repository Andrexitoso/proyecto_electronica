#include <Arduino.h>
//Parameters
const int micPin  = 35;
const int micPin2  = 34;
const int speakerPin = 25;

//Variables
int micVal  = 0;
int micValNoise = 0;

int32_t max_v = 1;
int32_t min_v = 0;
double dat_micro = 0;
char num_bits = 8;
double q = 1.0/(256.0-2.0);
int quant_sig  = 0;
double first;
void readMicrophone();

void setup() {
  //Init Serial USB
  Serial.begin(9600);
  Serial.println(F("Initialize System"));
  //Init Microphone
  pinMode(micPin, INPUT);
  pinMode(micPin2, INPUT);
}

void loop() {
  readMicrophone();
}

void readMicrophone( ) { /* function readMicrophone */
  ////Test routine for Microphone
  micVal = analogRead(micPin);
  micValNoise = analogRead(micPin2);
  //micVal = micVal - micValNoise;
  //Serial.printf("%d \t %d \n",micVal, micValNoise);
  dacWrite(speakerPin,micVal/16);

 /*if (micVal < min_v){
      min_v = micVal;
  }
  else if (micVal > max_v){
      max_v = micVal;
  }
  dat_micro = (double)(micVal - min_v)/(double)(micVal - min_v);
  first = q*(double)round((double)dat_micro/q);
  first = first * (double)255.0;
  quant_sig = round(first);
  dacWrite(speakerPin, quant_sig);*/
  //Serial.print(F("mic val ")); Serial.println(micVal);
}