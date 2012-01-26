#include <i2cmaster.h>
#include <ADXL345.h>
#include <Wire.h>


ADXL345 accel; //variable adxl is an instance of the ADXL345 library
int statusPin = 33;

void setup(){
    Serial.begin(9600);
    Serial.println("Initializing...");
    Wire.begin();   
    pinMode(statusPin, OUTPUT);
    // I2C used by IR temp sensor
    i2c_init(); 
    accel = ADXL345();
    
    // make sure that the accelerometer is connected...
    if(accel.EnsureConnected())
    {
      // the accelerometer is connected
      Serial.println("Accelerometer connected!");
      digitalWrite(statusPin, HIGH);
      accel.SetRange(15, true);
      accel.EnableMeasurements();    
    } 
    else
    {
     Serial.println("Accelerometer not found!");
     digitalWrite(statusPin, LOW);
    }  
}

void loop(){
  byte IRTemp = 0xb4; // factory default address for the MLX90614 sensor
  byte AccelAddress = 0x5a;
  byte responseSize = 0x03; // we expect to receive 3 bytes of data from the sensor
  byte command = 0x07; // command to request temp data
  int data_low = 0;
  int data_high = 0;
  int pec = 0;
  
  i2c_start_wait(IRTemp + I2C_WRITE);
  i2c_write(command);
  i2c_rep_start(IRTemp + I2C_READ);
  data_low = i2c_readAck();
  data_high = i2c_readAck();
  pec = i2c_readNak();
  i2c_stop();
  
  double tempData = 0x00;
  
  tempData = ((double)(((data_high & 0x007F) << 8) + data_low)) * 0.02; // 0.02 is the temp factor (0.02 LSB (measurement resolution of the sensor))
  float fahrenheit = ((tempData - 273.15) * 1.8) + 32;
  
  //Serial.println(fahrenheit);
  
  /*
   * Done with thermometer readings.
   * Move on to the accelerometer....
   */
   if(accel.IsConnected)
   {
      AccelerometerRaw raw = accel.ReadRawAxis(); 
      int xAxisRawData = raw.XAxis;
      AccelerometerScaled scaled = accel.ReadScaledAxis();
      float xAxisGs = scaled.XAxis;


      Serial.print(scaled.XAxis);
      Serial.print("G   ");   
      Serial.print(scaled.YAxis);
      Serial.print("G   ");   
      Serial.print(scaled.ZAxis);
      Serial.print("G   ");
      Serial.print(fahrenheit);
      Serial.println("F");

   }

   delay(20);

}

