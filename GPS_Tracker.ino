#include <Arduino.h>
#include <SPI.h>
#include <SD.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL345_U.h>
#include <Adafruit_GPS.h>

/* Assign a unique ID to this sensor at the same time */
Adafruit_ADXL345_Unified accel = Adafruit_ADXL345_Unified(12345);
const int chipSelect = 10;

// what's the name of the hardware serial port?
#define GPSSerial Serial

// Connect to the GPS on the hardware port
Adafruit_GPS GPS(&GPSSerial);

// Set GPSECHO to 'false' to turn off echoing the GPS data to the Serial console
// Set to 'true' if you want to debug and listen to the raw GPS sentences
#define GPSECHO  false

int fix_led = 2;
int switch_pin = 4;
int record_led = 3;

float x=0;
float y=0;
float z=0;
float counterAccell=0;
float temp;
float spoed;
float conversion=1.852;
char c;

File mySensorData;

uint32_t timer1 = millis();
uint32_t timer2 = millis();

int testCounter;
bool newTest;

float myMinutes;
int myDegrees;
float myLocation;

void readGPS() {

  while (!GPS.newNMEAreceived()) {
    c = GPS.read();
  }
  GPS.parse(GPS.lastNMEA());
  

  while (!GPS.newNMEAreceived()) {
    c = GPS.read();
  }
  GPS.parse(GPS.lastNMEA());


}

void setup(void)
{

while (!Serial); // for Leonardo/Micro/Zero

  Serial.begin(9600);
  pinMode(fix_led, OUTPUT);
  pinMode(record_led, OUTPUT);
  pinMode(switch_pin, INPUT_PULLUP);

  testCounter=1;
  newTest=true;

  //digitalWrite(7, HIGH); //turns on LED to show power is received
  timer1 = millis();
  timer2 = millis();

  accel.setRange(ADXL345_RANGE_16_G);

  GPS.begin(9600);

}

void loop(void)
{
  if (GPS.fix){
    digitalWrite(fix_led, HIGH);
  }
  else{
    digitalWrite(fix_led, LOW);
  }

  readGPS(); //ensures 2 newly parsed sentences

  if (digitalRead(switch_pin) == LOW) {

    
    
  
    while(newTest){//calculates the test number
      if(SD.exists((String)"Test"+testCounter+".CSV")){
      testCounter++;
        }
        else{ 
          newTest = false;
          Serial.println((String) "New Test Detected: TEST"+testCounter);
          mySensorData = SD.open((String)"Test"+testCounter+".CSV", FILE_WRITE );
          
          mySensorData.print("Date,Time,Latitude,Longitude,Altitude,Speed,Heading,X-Axis Acceleration,Y-Axis Acceleration,Z-Axis Acceleration,");
          mySensorData.println("Number of Satellites Connected");
          
          mySensorData.close();
     
        }
    }

    if(millis() - timer2 > 100){

      timer2=millis();

      sensors_event_t event;
      accel.getEvent(&event);

      //add measurement to x
      x = x + event.acceleration.x;
      //add measurement to y
      y = y + event.acceleration.y;
      //add measurement to z
      z = z + event.acceleration.z;

      counterAccell = counterAccell + 1;  

         
    }

  if (millis() - timer1 > 1000) {
    timer1=millis();
    
      mySensorData = SD.open((String) "Test"+testCounter+".CSV", FILE_WRITE );
      
          if((mySensorData)){ //if file succesfully openened
            Serial.println((String)"File "+testCounter+" opened");
            digitalWrite(8, HIGH);

                //DATE
                mySensorData.print(GPS.day, DEC); mySensorData.print('/');
                mySensorData.print(GPS.month, DEC); mySensorData.print("/20");
                mySensorData.print(GPS.year, DEC);

                mySensorData.print(",");

                //TIME
                if (GPS.hour < 8) { mySensorData.print('0'); }
                mySensorData.print(GPS.hour+2, DEC); mySensorData.print(':');
                if (GPS.minute < 10) { mySensorData.print('0'); }
                mySensorData.print(GPS.minute, DEC); mySensorData.print(':');
                if (GPS.seconds < 10) { mySensorData.print('0'); }
                mySensorData.print(GPS.seconds, DEC);

                mySensorData.print(",");
                
                
                
                if(GPS.fix){

                  //latitude
                  myDegrees = GPS.latitude/100;
                  myMinutes = (float) GPS.latitude - myDegrees*100.0;
                  myLocation = (float) myDegrees + (myMinutes/60.0) ;
                  if (GPS.lat == 'S'){
                    myLocation = (float) myLocation*-1.0;
                  }
                  mySensorData.print(myLocation, 5);


                  mySensorData.print(",");
                  
                  //longitude                 
                  myDegrees = GPS.longitude/100;
                  myMinutes = (float) GPS.longitude - myDegrees*100.0;
                  myLocation = (float) myDegrees + (myMinutes/60.0) ;
                  if (GPS.lon == 'W'){
                    myLocation = (float) myLocation*-1.0;
                  }
                  mySensorData.print(myLocation, 5);

                  mySensorData.print(",");
                  
                  //altitude
                  mySensorData.print(GPS.altitude);

                  Serial.println((String)"Altitude: "+GPS.altitude);

                  mySensorData.print(",");

                  //speed
                  if(GPS.speed == NULL){
                    mySensorData.print(" 0 ");
                  }
                  else{
                  spoed = (float)  GPS.speed*conversion;
                  mySensorData.print(spoed);
                  }

                  mySensorData.print(",");

                  //heading
                  if(GPS.angle == NULL){
                    mySensorData.print(" 0 ");
                  }
                  else{
                  mySensorData.print(GPS.angle, 4);
                  }

                  mySensorData.print(",");
                
               
                }
                else{\
                  //no fix
                  //latitude 
                  mySensorData.print(" 0 ");

                  mySensorData.print(",");
                  
                  //longitude
                  mySensorData.print(" 0 ");

                  mySensorData.print(",");

                  
                  //altitude
                  mySensorData.print(" 0 ");

                  mySensorData.print(",");

                  //speed
                  mySensorData.print(" 0 ");

                  mySensorData.print(",");

                  //heading
                  mySensorData.print(" 0 ");

                  mySensorData.print(",");
                }

                //x axis
                temp = (float) (x/counterAccell)/9.80665;
                mySensorData.print(temp, 4);
                mySensorData.print(",");
                //y axis
                temp = (float) (y/counterAccell)/9.80665;
                mySensorData.print(temp, 4);
                mySensorData.print(",");
                //z axis
                temp = (float) (z/counterAccell)/9.80665;
                mySensorData.print(temp, 4);
               

                x=0;
                y=0;
                z=0;
                counterAccell=0;


                //# satellites connected
                mySensorData.print((int)GPS.satellites);
                mySensorData.print(",");


                mySensorData.println("");
                mySensorData.close();
        
                }
                else{
            //File could not be opened
            newTest = true;
            digitalWrite(8, LOW);
            Serial.println("file could not be opened not recording");
           }
  }       
      }
  else{
          newTest = true;
          digitalWrite(record_led, LOW);
          Serial.println("Not Saving... ");
  }

}
