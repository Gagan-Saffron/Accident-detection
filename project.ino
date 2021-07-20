// declaring pins for panic switch and shock sensor inputs 
#define panic_switch 12
#define shock_input 11

//wire library to acces MEMS sensor
#include <Wire.h>

long accelX, accelY, accelZ;
float gForceX, gForceY, gForceZ;

long gyroX, gyroY, gyroZ;
float rotX, rotY, rotZ;


void setup() {

  // defining panic switch and shock input pins
  pinMode(panic_switch, INPUT_PULLUP);
  pinMode(shock_input, INPUT);

  Wire.begin();
  Serial.begin(9600);


  //Setting up hardware

  Serial.println("------Setting_up_Hardwares------");
  
  if(mems_setup())
    Serial.println("[+] MEMS_setup");
  else
    Serial.println("[-] MEMS_setup");


  if(gsm_setup())
    Serial.println("[+] GSM_setup");
  else
    Serial.println("[-] GSM_setup");

  if(gps_setup())
    Serial.println("[+] GPS_setup");
  else
    Serial.println("[-] GPS_setup");
  
}

void loop() {
  // put your main code here, to run repeatedly:

  if(digitalRead(panic_switch) == LOW)
  {
    Serial.println("[!] Panic switch activated");
    if(send_panic_msg())
      Serial.println("--->[+] Panic message sent");
    else
      Serial.println("---> [-] Error in sending panic message");
  }

  if(digitalRead(shock_input) == LOW)
  {
    Serial.println("[!] Impact detected");
    if(send_accident_msg())
      Serial.println("--->[+] Accident message sent");
    else
      Serial.println("---> [-] Error in sending accident message");
    delay(5000);
  }

  if(check_abnormal_position())
  {
    Serial.println("[!] Abnormal position detected");
    if(send_accident_msg())
      Serial.println("--->[+] Accident message sent");
    else
      Serial.println("---> [-] Error in sending accident message");
    delay(5000);
  }
  else
    Serial.println("[*] position OK");

}

//setup functions

int mems_setup()
{
  Wire.beginTransmission(0b1101000); //This is the I2C address of the MPU (b1101000/b1101001 for AC0 low/high datasheet sec. 9.2)
  Wire.write(0x6B); //Accessing the register 6B - Power Management (Sec. 4.28)
  Wire.write(0b00000000); //Setting SLEEP register to 0. (Required; see Note on p. 9)
  Wire.endTransmission();  
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x1B); //Accessing the register 1B - Gyroscope Configuration (Sec. 4.4) 
  Wire.write(0x00000000); //Setting the gyro to full scale +/- 250deg./s 
  Wire.endTransmission(); 
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x1C); //Accessing the register 1C - Acccelerometer Configuration (Sec. 4.5) 
  Wire.write(0b00000000); //Setting the accel to +/- 2g
  Wire.endTransmission(); 
  return 1;
}

int gsm_setup()
{
  return 0;
}

int gps_setup()
{
  return 0;
}

//Accident checking functions

int check_abnormal_position()
{
  Serial.println("\n------checking position------");
  Wire.beginTransmission(0b1101000); //I2C address of the MPU
  Wire.write(0x3B); //Starting register for Accel Readings
  Wire.endTransmission();
  Wire.requestFrom(0b1101000,6); //Request Accel Registers (3B - 40)
  while(Wire.available() < 6);
  accelX = Wire.read()<<8|Wire.read(); //Store first two bytes into accelX
  accelY = Wire.read()<<8|Wire.read(); //Store middle two bytes into accelY
  accelZ = Wire.read()<<8|Wire.read(); //Store last two bytes into accelZ
  processAccelData();
  if(gForceX >= 0.5 || gForceY >= 0.5 || gForceZ <= 0.5)
    return 1; 
  else 
    return 0;
}

void processAccelData(){
  gForceX = accelX / 16384.0;
  gForceY = accelY / 16384.0; 
  gForceZ = accelZ / 16384.0;
}


//Messagin functions
int send_panic_msg()
{
  return 1;
}

int send_accident_msg()
{
  return 1;
}
