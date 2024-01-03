#include <TheThingsNetwork.h>
//#include "SerialTransfer.h"

// Set your AppEUI and AppKey
const char *appEui = "0001020304050607";
const char *appKey = "6FA7ABF852CC96D1CEF336CF4CEB4B82";

long instance1=0, timer;
double hrv =0, hr = 72, interval = 0;
int value = 0, count = 0;  
bool flag = 0;
bool filtre = 0;
double cpt = 0;
double cpt2 = 0;

#define shutdown_pin 10
#define threshold 100 // to identify R peak
#define timer_value 10000 // 10 seconds timer to calculate hr

#define loraSerial Serial1
#define freqPlan TTN_FP_EU868
TheThingsNetwork ttn(loraSerial, Serial, freqPlan);

//SerialTransfer myTransfer;

void setup() {
  loraSerial.begin(57600);
  Serial.begin(9600);
  //myTransfer.begin(Serial);
  pinMode(8, INPUT); // Setup for leads off detection LO +
  pinMode(9, INPUT); // Setup for leads off detection LO -
 
  // Wait a maximum of 10s for Serial Monitor
  while (!Serial && millis() < 10000)
    ;
  //Serial.println("-- STATUS");
  ttn.showStatus();
  //Serial.println("-- JOIN");
  ttn.join(appEui, appKey);
  ttn.showStatus();
  //Serial.println("Setup for The Things Network complete");

}

void loop() {
  cpt +=1;
  cpt2+=1;
  if((digitalRead(8) == 1)||(digitalRead(9) == 1)){
    //Serial.println("leads off!");
    digitalWrite(shutdown_pin, LOW); //standby mode
    instance1 = micros();
    timer = millis();
  }

  else {
    digitalWrite(shutdown_pin, HIGH); //normal mode
    value = analogRead(A0);
    value = map(value, 250, 400, 0, 100); //to flatten the ecg values a bit
    if((value > threshold) && (!flag)) {
      count++;  
      //Serial.println("in");
      flag = 1;
      interval = micros() - instance1; //RR interval
      instance1 = micros();
    }

    else if((value < threshold)) {
      flag = 0;
    }

    if ((millis() - timer) > 10000) {
      hr = count*6;
      timer = millis();
      count = 0;
    }

    hrv = hr/60 - interval/1000000;
    if(cpt2>15000 && !(filtre)){
      Serial.println(hr);
      //Serial.println(cpt);
      //Serial.print(",");
      //Serial.print(hrv);
      //Serial.print(",");
      //Serial.println(value);
      cpt2=0;
    }
    else{
      filtre = 0;
    }

    byte payload[1];
    if (cpt>=150000){
      payload[0] = hr;
      ttn.sendBytes(payload, sizeof(payload));
      cpt=0;
      filtre = 1;
      delay(10);
    }
  }
}