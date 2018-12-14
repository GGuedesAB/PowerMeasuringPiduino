
/*
 *  The circuit in the schematic multiplies every 5v by 0.666, so it goes down to 3.3v
 * and the ESP32 can safely measure analog signals acording to it's ADC specs.
 */

byte offsetPin = 35;
byte measurePin = 34;
int scaleFactor = 185;

int getOffset (byte pin){
  int offsetRead = analogRead(pin);
  return offsetRead;
}

int getSensorReading (byte pin){
  int i = 0;
  int sensorReading = 0;
  for (i = 0; i<999; i++){
    sensorReading += analogRead(pin);
  }
  sensorReading = sensorReading/1000;
  return sensorReading;
}

float getCurrent () {
  int ACOffset = (getOffset(offsetPin))/2;
  int measure = getSensorReading(measurePin);
  //Serial.print(ACOffset);
  //Serial.print("|");
  //Serial.println(measure);
  /*
   *  voltageShift is a value that starts from 0 (wich is when there is no current
   * running throgh the sensor) and goes on according to the variation on the sensor's
   * terminals.
   */
  float voltageShift = (measure - ACOffset)*(3.3/4096);
  //I'm not very sure why there is this negative offset, so we take it off.
  float current = voltageShift*(1000/185) + 0.69;
  Serial.println(current);
  if (current > 0){
    current = 0;
  }
  //Not sure why my results were half of those I saw on the internet
  current = -2000*current;
  return current;
}

float calculatePower (){
  int sampleVoltage = getOffset(offsetPin)/2;
  float voltageIn = sampleVoltage*(5.05/2047);
  //Serial.print(voltageIn);
  //Serial.println(" V");
  float currentIn = getCurrent();
  //Serial.print(currentIn);
  //Serial.println(" mA");
  float instantPower = (voltageIn*currentIn)/1000;
  return instantPower;
}

void setup (){
  Serial.begin(9600);
}

void loop (){
  float aux = calculatePower();
  Serial.println(aux);
}
