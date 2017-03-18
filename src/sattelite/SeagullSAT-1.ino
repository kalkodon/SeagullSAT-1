/* SeagullSAT-1 Source Code v1.1 
 * ©2016 kalkodon(); 
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301  USA
 */
#include <SPI.h> // library for SD card communication
#include <SD.h> // SD card library
#include <SoftwareSerial.h> // we can use any 2 pins for serial communication with this library
#include <DS1302.h> // library for rtc
#include <Wire.h> // i2c library (for BMP180 and MPU9255)
#include <Adafruit_BMP085.h> // library for BMP180
#include "I2Cdev.h" // enchanted i2c library
#include "MPU9250.h" // MPU9255 library
//MPU9250 acc;
const int kCePin   = 9;  // Chip Enable for RTC
const int kIoPin   = 6;  // Input/Output for RTC
const int kSclkPin = 5;  // Serial Clock for RTC
DS1302 rtc(kCePin, kIoPin, kSclkPin); // identify RTC
Adafruit_BMP085 bmp; // identify barometric pressure and temperature sensor
int16_t ax, ay, az; // identify accerelometer's xyz
int16_t gx, gy, gz; // identify gyroscope's xyz
int16_t mx, my, mz; // identify magnetometer's xyz
const int chipSelect = 4; // chip select pin for SD card
unsigned long timerPrevious = 0; // time interval variables
const long timer = 1000; 
SoftwareSerial Xbee(0,1); // Arduino Leonardo has 2 serial communication types. Communication over 0, 1 is normally controlled by "Serial1" library. Serial1 can confuse us so we used software serial.

String getTime() { // a function for RTC's time
  Time t = rtc.time();  // getting time and date info from DS1302
  char buf[50]; // we use a char array for time and date
  snprintf(buf, sizeof(buf), "%04d-%02d-%02d %02d:%02d:%02d", // in here, we use C functions for adding integers in a string
           t.yr, t.mon, t.date,
           t.hr, t.min, t.sec);
  return buf; // returning "buf". now when we wrote getTime() it returns buf value.
}
//float temp() { 
//  float sensorData = bmp.readTemperature();
//  return sensorData;
//}
//float altitude() {
//  float alt = bmp.readAltitude();
//  return alt;
//}
void setup() {
  Wire.begin(); // initialize i2c
  Xbee.begin(9600); // initialize Xbee software serial with 9600 baud rate
//  acc.initialize(); 
  while (!Xbee) {
    ; // wait for serial port to connect.
  }
  if (!bmp.begin()) { // it's an try-catch function actually. it checks bmp; if it's ok, begins it. else shows an error and interrupts program.
  Xbee.println("BMP180 hatasi/error");
  while (1) {}
  }
  if (!SD.begin(chipSelect)) { // same as bmp
    Serial.println("SD error/hatasi");
    return;
  }
  Xbee.println("SEAGULLSAT-1 FIRMWARE V1.0");
  Xbee.println("--------------------------");
  Xbee.println("SD card => OK"); // debugging
  Xbee.println("Sensors => OK");
  Xbee.println("Communications => OK");
}

void loop() {
  //acc.getMotion9(&ax, &ay, &az, &gx, &gy, &gz, &mx, &my, &mz); // writing accerelometer, gyroscope and magnetometer's values to the integers which we identifyed at the beginning.
  unsigned long timerNow = millis(); // timer function. it keeps miliseconds from the beginning of the program. 

  if (timerNow - timerPrevious >= timer) { // timer function. the code in this if block runs once per second. if we change interval variable, it runs once per interval ms.
    timerPrevious = timerNow; // timer functions
  String dataString = ""; // create a string to keep data
  dataString += String(getTime());  // time info
  dataString += String(" Sicaklik: "); 
  dataString += String(bmp.readTemperature()); //temp 
  dataString += String(" C Yukseklik: ");
  dataString += String(bmp.readAltitude()); // altitude from BMP180
  dataString += String(" m Hava Basinci: "); 
  dataString += String(bmp.readPressure()); // air pressure
  dataString += String(" Pa "); 
//  dataString += String(ax);
//  dataString += String(" Ivme (y): ");
//  dataString += String(ay);
 // dataString += String(" Ivme (z): ");
//  dataString += String(az);
  Xbee.println(dataString); // send data to ground station
  File logFile = SD.open("satlog.txt", FILE_WRITE); // opening log file in SD card
    if (logFile) { // if file exist or created
    logFile.println(dataString); // writing data to file
    logFile.close(); //  closing log file
  } else { // if there is an error about file
    Serial.println("SD karta yazilamadi / SD card writing error"); // show error
  }
  }
  
}









