#define _i2cAddress 0x60
#include <Wire.h>
  // CMPS11 compass registers
  #define BEARING_REGISTER  2 
  #define PITCH_REGISTER    4 
  #define ROLL_REGISTER     5
  
  #define ACCEL_X_REGISTER 12
  #define ACCEL_Y_REGISTER 14
  #define ACCEL_Z_REGISTER 16
  
  #define GYRO_X_REGISTER  18
  #define GYRO_Y_REGISTER  20
  #define GYRO_Z_REGISTER  22

  #define TEMP_REGISTER    24

  #define CONTROL_REGISTER  0

  #define ONE_BYTE 1
  #define TWO_BYTES 2
int y;
//---------------------------------

  int _bearing;
  int nReceived;
  byte _fine;
  byte _byteHigh;
  byte _byteLow;
  char _pitch;
  char _roll;

  float accelx = 0;
  float accely = 0;
  float accelz = 0;
  float _accelScale = 9.80665f * 2.0f/32768.0f;
  const int tpr = A2;

const int epr = A0;

const int tpm = A3; //13;
const int epm = A1;

const int tpl = 11;

const int epl = 10;

const int in1 = 9;

const int in2 = 8;

const int in3 = 4;

const int in4 = 3;

const int enA = 5;

const int enB = 6;

#define PWM 100
#define DIS 5
int tf;
void setup(){
  
  Serial.begin(9600);
  Wire.begin();
 pinMode(tpr, OUTPUT);
 pinMode(epr, INPUT);

 pinMode(tpm, OUTPUT);
 pinMode(epm, INPUT);

 pinMode(tpl, OUTPUT);
 pinMode(epl, INPUT);

 pinMode (in1, OUTPUT);
 pinMode (in2, OUTPUT);
 pinMode (in3, OUTPUT);
 pinMode (in4, OUTPUT);
 pinMode (enA, OUTPUT);
 pinMode (enB, OUTPUT);
 tf = 1;
}
void forward (){

 digitalWrite(in1, LOW);

 digitalWrite(in2, HIGH);

 digitalWrite(in3, LOW);

 digitalWrite(in4, HIGH);

 analogWrite(enA, PWM); 

 analogWrite(enB, PWM);

}

void turn_left () {

 digitalWrite(in1, HIGH);

 digitalWrite(in2, LOW);

 digitalWrite(in3, LOW);

 digitalWrite(in4, HIGH);

 analogWrite(enA, PWM); 

 analogWrite(enB, PWM);

}

void turn_right () {

 digitalWrite(in1, LOW); 

 digitalWrite(in2, HIGH);

 digitalWrite(in3, HIGH);

 digitalWrite(in4, LOW);

 analogWrite(enA, PWM);

 analogWrite(enB, PWM);

}

void reverse (){

 digitalWrite(in1, LOW);

 digitalWrite(in2, HIGH);

 digitalWrite(in3, LOW);

 digitalWrite(in4, HIGH);

 analogWrite(enA, PWM);

 analogWrite(enB, PWM);

}

void stop(){
  

 digitalWrite(in1, LOW);

 digitalWrite(in2, LOW);

 digitalWrite(in3, LOW);

 digitalWrite(in4, LOW);

 analogWrite(enA, LOW);

 analogWrite(enB, LOW);

}

long MiddleSensor (){

long dur;

 digitalWrite(tpm, LOW); 

 delayMicroseconds(5); // delays are required for a succesful sensor operation.

 digitalWrite(tpm, HIGH);

 delayMicroseconds(10); //this delay is required as well!

 digitalWrite(tpm, LOW);

 dur = pulseIn(epm, HIGH);

 return ((0.0343*dur)/2);// convert the distance to centimeters.

}

long RightSensor () 

{

long dur;

 digitalWrite(tpr, LOW);

 delayMicroseconds(5); // delays are required for a succesful sensor operation.

 digitalWrite(tpr, HIGH);

 delayMicroseconds(10); //this delay is required as well!

 digitalWrite(tpr, LOW);

 dur = pulseIn(epr, HIGH);

 return (0.0343*dur/2);// convert the distance to centimeters.

}

long LeftSensor ()    
{

long dur;

 digitalWrite(tpl, LOW); 

 delayMicroseconds(5); // delays are required for a succesful sensor operation.

 digitalWrite(tpl, HIGH);

 delayMicroseconds(10); //this delay is required as well!

 digitalWrite(tpl, LOW);

 dur = pulseIn(epl, HIGH);

 return (0.0343*dur/2);// convert the distance to centimeters.

}
void loop(){

  delay(500);
  int bearing = getBearing();
int x = bearing ;
  signed char pitch = getPitch();

  signed char roll = getRoll();
  
  // Read the accelerator
  accelx = getAcceleroX() * _accelScale;
  accely = getAcceleroY() * _accelScale;
  accelz = getAcceleroZ() * _accelScale;

    
  // Print data to Serial Monitor window
  Serial.print("$CMP,");
  Serial.print(bearing);
  Serial.print(",");
  Serial.print(pitch); 
  Serial.print(",");
  Serial.println(roll);
 Serial.println(MiddleSensor());
 Serial.println(RightSensor ());
 Serial.println(LeftSensor ());
 Serial.println("*********");
 if (tf == 1){
  y = bearing;
  Serial.println("yhnghhhhhhhhhhhhhh");
  tf = 0;
}
 if ( MiddleSensor() < DIS && RightSensor () <15 && LeftSensor ()>15){
        while(MiddleSensor ()<15){
          LeftSensor ();
          turn_left (); 
        }
        

 }

 else if (MiddleSensor() <DIS && RightSensor () >15 && LeftSensor ()<15){

        while(MiddleSensor () <15){
          RightSensor ();
          turn_right (); 
        }
        

 }
 Serial.println(y);
 Serial.println(x);

        if (x < y-2){
          getBearing();
          while(x < y-2){
            Serial.print("right");
            getBearing();
            x = getBearing();
            turn_right();
            

 }
        }
        if (x > y+2){
          getBearing();
        while(x > y+2){
          getBearing();
            x = getBearing();
            turn_left();
 }
  }
 forward ();    

}
int getBearing()
{
  // begin communication with CMPS11
  Wire.beginTransmission(_i2cAddress);

  // Tell register you want some data
  Wire.write(BEARING_REGISTER);

  // End transmission
  int nackCatcher = Wire.endTransmission();

  // Return if we have a connection problem  
  if(nackCatcher != 0){return 0;}
  
  // Request 2 bytes from CMPS11
  nReceived = Wire.requestFrom(_i2cAddress , TWO_BYTES);

  // Something has gone wrong
  if (nReceived != TWO_BYTES) return 0;
  
  // Read the values
  _byteHigh = Wire.read(); 
  _byteLow = Wire.read();

  // Calculate full bearing
  _bearing = ((_byteHigh<<8) + _byteLow) / 10;
  
  return _bearing;
}
signed char getPitch()
{
  // Begin communication with CMPS11
  Wire.beginTransmission(_i2cAddress);

  // Tell register you want some data
  Wire.write(PITCH_REGISTER);

  // End transmission
  int nackCatcher = Wire.endTransmission();

  // Return if we have a connection problem 
  if(nackCatcher != 0){return 0;}

  // Request 1 byte from CMPS11
  nReceived = Wire.requestFrom(_i2cAddress , ONE_BYTE);

  // Something has gone wrong
  if (nReceived != ONE_BYTE) return 0;
  
  // Read the values
  _pitch = Wire.read();

  return _pitch;
}
signed char getRoll()
{
  // Begin communication with CMPS11
  Wire.beginTransmission(_i2cAddress);

  // Tell register you want some data
  Wire.write(ROLL_REGISTER);

  // End transmission
  int nackCatcher = Wire.endTransmission();

  // Return if we have a connection problem 
  if(nackCatcher != 0){return 0;}

  // Request 1 byte from CMPS11
  nReceived = Wire.requestFrom(_i2cAddress , ONE_BYTE);

  // Something has gone wrong
  if (nReceived != ONE_BYTE) return 0;
  
  // Read the values
  _roll = Wire.read();

  return _roll;
}
int16_t getAcceleroX()
{
  // Begin communication with CMPS11
  Wire.beginTransmission(_i2cAddress);

  // Tell register you want some data
  Wire.write(ACCEL_X_REGISTER);

  // End the transmission
  int nackCatcher = Wire.endTransmission();

  // Return if we have a connection problem 
  if(nackCatcher != 0){return 0;}
  
  // Request 2 bytes from CMPS11
  nReceived = Wire.requestFrom(_i2cAddress , TWO_BYTES);

  // Something has gone wrong
  if (nReceived != TWO_BYTES) return 0;

  // Read the values
  _byteHigh = Wire.read(); 
  _byteLow = Wire.read();

  // Calculate Accelerometer
  return (((int16_t)_byteHigh <<8) + (int16_t)_byteLow);
}
int16_t getAcceleroY()
{
  // Begin communication with CMPS11
  Wire.beginTransmission(_i2cAddress);

  // Tell register you want some data
  Wire.write(ACCEL_Y_REGISTER);

  // End the transmission
  int nackCatcher = Wire.endTransmission();

  // Return if we have a connection problem 
  if(nackCatcher != 0){return 0;}
  
  // Request 2 bytes from CMPS11
  nReceived = Wire.requestFrom(_i2cAddress , TWO_BYTES);

  // Something has gone wrong
  if (nReceived != TWO_BYTES) return 0;

  // Read the values
  _byteHigh = Wire.read(); 
  _byteLow = Wire.read();

  // Calculate Accelerometer
  return (((int16_t)_byteHigh <<8) + (int16_t)_byteLow);
}
int16_t getAcceleroZ()
{
  // Begin communication with CMPS11
  Wire.beginTransmission(_i2cAddress);

  // Tell register you want some data
  Wire.write(ACCEL_Z_REGISTER);

  // End the transmission
  int nackCatcher = Wire.endTransmission();

  // Return if we have a connection problem 
  if(nackCatcher != 0){return 0;}
  
  // Request 2 bytes from CMPS11
  nReceived = Wire.requestFrom(_i2cAddress , TWO_BYTES);

  // Something has gone wrong
  if (nReceived != TWO_BYTES) return 0;

  // Read the values
  _byteHigh = Wire.read(); 
  _byteLow = Wire.read();

  // Calculate Accelerometer
  return (((int16_t)_byteHigh <<8) + (int16_t)_byteLow);
}
