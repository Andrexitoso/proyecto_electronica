//Parameters
const int micPin  = 35;
const int micPin2  = 34;
const int speakerPin = 25;

hw_timer_t *timer = NULL;

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

int8_t vector[100];
int16_t count_sample = 0;

void IRAM_ATTR onTimer(){
  micVal = analogRead(micPin);
  dacWrite(speakerPin,micVal/16);
  while(count_sample < 100{
    vector[count_sample] = micVal/16;
    count_sample++;
  }
  if(count_sample == 100){
    //send udp packet
    count_sample = 1
    vector[0] = micVal/16;
  }
  
}

void setup() {
  //Init Serial USB
  Serial.begin(9600);
  Serial.println(F("Initialize System"));
  //Init Microphone
  pinMode(micPin, INPUT);
  pinMode(micPin2, INPUT);
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &onTimer, true);
  timerAlarmWrite(timer, 100, true);
  timerAlarmEnable(timer);
}

void loop() {
  //readMicrophone();
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
