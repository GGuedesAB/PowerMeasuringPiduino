//Carrega as bibliotecas
#include "EmonLib.h"
#include <SPI.h>

EnergyMonitor emon1;

//Tensao da rede eletrica
int rede = 110;

//Pino do sensor SCT
int pino_sct = A1;

//Pino switch
int pino_switch = A5;

//Send buffer
char serialBuf [11];
char serialHold [11];

//Potencia instantanea
double pot = 0;

char light = 'n';

int receivedOrder = 0;

String sendToRasp = "0";

String saveLastReading = "0";

int globalSwitch = 0;

int switchReading = 0;

void setup()
{
  Serial.begin(38400, SERIAL_8N1);
  pinMode(3, OUTPUT);
  pinMode(5, OUTPUT);
  pinMode(6, OUTPUT);
  pinMode(9, INPUT);
  //Pino, calibracao - Cur Const= Ratio/BurdenR. 2000/33 = 60
  emon1.current(pino_sct, 60);
}

void serialEvent() {
  saveLastReading = sendToRasp;
  saveLastReading.toCharArray(serialHold, 11);
  receivedOrder = 1;
  while (Serial.available()){
    light = Serial.read();
    //Light states are:
    //  0 -> All off
    //  1 -> 1 on
    //  2 -> 2 on
    //  3 -> 3 on
    if (light == 'o') {
      globalSwitch = 0;
      digitalWrite(3, LOW);
      digitalWrite(5, LOW);
      digitalWrite(6, LOW);
      //delay (500);
    }
    else if (light == 'i') {
      globalSwitch = 1;
      digitalWrite(3, HIGH);
      digitalWrite(5, LOW);
      digitalWrite(6, LOW);
      //delay(500);    
    }
    else if (light == 's') {
      globalSwitch = 1;
      digitalWrite(3, HIGH);
      digitalWrite(5, HIGH);
      digitalWrite(6, LOW);
      //delay(500);
    }
    else if (light == 'a') {
      globalSwitch = 1;
      digitalWrite(3, HIGH);
      digitalWrite(5, HIGH);
      digitalWrite(6, HIGH);
      //delay(500);
    }
    else {
      globalSwitch = 0;
      digitalWrite(3, LOW);
      digitalWrite(5, LOW);
      digitalWrite(6, LOW);
      //delay(500);
    }
    Serial.flush();
  } 
}

void loop()
{
  switchReading = analogRead(pino_switch);
  if (switchReading > 800 && globalSwitch == 0) {
    globalSwitch = 1;
    digitalWrite(3, HIGH);
    digitalWrite(5, HIGH);
    digitalWrite(6, HIGH);
  }
  else if (switchReading > 800 && globalSwitch == 1) {
    globalSwitch = 0;
    digitalWrite(3, LOW);
    digitalWrite(5, LOW);
    digitalWrite(6, LOW);
  }
  int i = 0;
  if (receivedOrder == 1){
    for (i = 0; i<5; i++){
      Serial.write(serialHold);
      delay(1000);
    }
    double Irms = emon1.calcIrms(1480);
    Irms = Irms - 0.1;
    pot = rede * Irms;
    sendToRasp = String(pot, 2); 
    sendToRasp.toCharArray(serialBuf, 11);
    Serial.write(serialHold);
    receivedOrder = 0;
    delay(1000);
  }
  //Calcula a corrente
  double Irms = emon1.calcIrms(1480);
  Irms = Irms - 0.1;
  pot = rede * Irms;
  sendToRasp = String(pot, 2); 
  sendToRasp = sendToRasp + ":";
  sendToRasp = sendToRasp + String(globalSwitch);
  sendToRasp.toCharArray(serialBuf, 11);
  Serial.write(serialBuf);
  
  //Mostra o valor da corrente no serial monitor e display
  //Serial.println("Corrente : ");
  //Serial.println(Irms); // Irms
  //Serial.println("Potencia:");
  //Serial.println(light);
  Serial.flush();
  delay(1000);
}
