//AS5600 coding is based on https://curiousscientist.tech/blog/as5600-magnetic-position-encoder and the Arduino 5600 library
//Loadcell Code is inspired by https://randomnerdtutorials.com/arduino-load-cell-hx711/
//SD-card reader is based on The official Arduino Library

#include <Wire.h> 
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include "HX711.h"
#include <SD.h>

// HX711
#define LOADCELL_PIN1 5
#define LOADCELL_PIN2 6
HX711 scale;
int readweight;
int lastread;
#define CALIBRATION_FACTOR -400

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define stepPin 3 
#define dirPin 2
#define MS1 9
#define MS2 8
#define MS3 7

#define BUTTON_PIN 4 //Extra Tare Function
#define FULLROTATION 200
#define FULLROTQUARTER 800

// Declaration of OLED
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);


//AS5600
int magnetStatus = 0; 
int lowbyte; //raw angle 7:0
word highbyte; //raw angle 7:0 and 11:8
int rawAngle; //final raw angle 
float degAngle; //raw angle in degrees (360/4096 * [value between 0-4095])
int quadrantNumber, previousquadrantNumber; //quadrant IDs
float numberofTurns = 0; //number of turns
float correctedAngle = 0; //tared angle - based on the startup value
float startAngle = 0; //starting angle
float totalAngle = 0; //total absolute angular displacement
float previoustotalAngle = 0; 
//

//Library
#define VALUESIZE 1000 //Store values for 20 unique isokinetic tests
#define WEIGHTANGLE 3 //Store weight + angle
#define chipSelect 10 //Where the SD-card module communicates from
File myFile;
float lib[VALUESIZE][WEIGHTANGLE];

void setup()
{
  Serial.begin(115200); //start serial - tip: don't use serial if you don't need it (speed considerations)
  Wire.begin(); //start i2C  
	Wire.setClock(800000L); //fast clock

  pinMode(stepPin,OUTPUT); 
  pinMode(dirPin,OUTPUT);
  pinMode(MS1, OUTPUT);
  pinMode(MS2, OUTPUT);
  pinMode(MS3, OUTPUT);
  digitalWrite(dirPin,HIGH); //Set inital rotation

  digitalWrite(MS1, LOW);
  digitalWrite(MS2, LOW);
  digitalWrite(MS3, LOW); //LOW-HIGH-LOW: Quarter step mode!
 
  checkMagnetPresence(); //check the magnet (blocks until magnet is found)
  ReadRawAngle(); //make a reading so the degAngle gets updated
  startAngle = degAngle; //update startAngle with degAngle - for taring

  //------------------------------------------------------------------------------
  //OLED part
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));}
    
  delay(1000);
  display.clearDisplay();
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0, 0);
  display.println("Welcome...");
  display.display();
  //delay(2000);

  Serial.println("Starting load cell...");
  scale.begin(LOADCELL_PIN1, LOADCELL_PIN2);

  scale.set_scale(CALIBRATION_FACTOR);   // this value is obtained by calibrating the scale with known weights
  scale.tare();               // reset the scale to 0

  ////// SD-CARD PART
    Serial.print("Initializing SD card...");

  if (!SD.begin(chipSelect)) {
    Serial.println("initialization failed!");
  }
 
  SD.remove("test2.txt"); //Remove previous file/values. Remove this line if you want to keep previous values
  myFile = SD.open("test2.txt", FILE_WRITE);
    if (myFile) {
    Serial.print("Writing to test2.txt...");
    myFile.println("Weight (g), Angle (degrees)"); // Write the header
    myFile.close();
    Serial.println("Writing header...");
  } else {
    Serial.println("Error opening test2.txt");   // if the file didn't open, print an error:
  }
}

void loop()
{    
    ReadRawAngle(); //ask the value from the sensor
    correctAngle(); //tare the value
    checkQuadrant(); //check quadrant, check rotations, check absolute angular position
    loadcell(); //Weight
    refreshDisplay(); //Show Angle and Weight on the OLED
    checkforce(); //Check if enough force has been applied to start the system
}

void motor() { //Test function for motor
  digitalWrite(dirPin, HIGH);
    for(int x = 0; x < (FULLROTATION/4); x++) { 
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(500); 
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(500);  
    }
    //delay(1000);
}

void motor_goin90() //Turn 90 degrees while measuring force/angle and writing to library
{
  digitalWrite(dirPin, HIGH);
    for(int x = 0; x < (FULLROTATION/4); x++) { 
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(50000); 
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(50000);  

    ReadRawAngle(); 
    correctAngle(); 
    checkQuadrant(); 
    loadcell();
    refreshDisplay();

    lib[x][0] = readweight;
    lib[x][1] = totalAngle;
    }

    delay(1500);
    warningretract(); //Warn user to remove finger from device
    writetofile(); //Write library values to file
    delay(1500);

    digitalWrite(dirPin, LOW); //Turn back to original position
    for(int x = 0; x < (FULLROTATION/4); x++) { 
    digitalWrite(stepPin,HIGH); 
    delayMicroseconds(10000); 
    digitalWrite(stepPin,LOW); 
    delayMicroseconds(10000);  
    ReadRawAngle(); 
    correctAngle(); 
    checkQuadrant(); 
    }
}

void ReadRawAngle()
{ 
  //7:0 - bits
  Wire.beginTransmission(0x36); //connect to the sensor
  Wire.write(0x0D); //figure 21 - register map: Raw angle (7:0)
  Wire.endTransmission(); //end transmission
  Wire.requestFrom(0x36, 1); //request from the sensor
  
  while(Wire.available() == 0); //wait until it becomes available 
  lowbyte = Wire.read(); //Reading the data after the request
 
  //11:8 - 4 bits
  Wire.beginTransmission(0x36);
  Wire.write(0x0C); //figure 21 - register map: Raw angle (11:8)
  Wire.endTransmission();
  Wire.requestFrom(0x36, 1);
  
  while(Wire.available() == 0);  
  highbyte = Wire.read();
  
  highbyte = highbyte << 8; //shifting to left
  rawAngle = highbyte | lowbyte; //int is 16 bits (as well as the word)
  degAngle = rawAngle * 0.087890625; //AS5600 accuracy of the model used
}

void correctAngle()
{
  correctedAngle = degAngle - startAngle; //this tares the position

  if(correctedAngle < 0) //if the calculated angle is negative, we need to "normalize" it
  {
  correctedAngle = correctedAngle + 360; //correction for negative numbers (i.e. -15 becomes +345)
  }
  else
  {
    //do nothing
  }
}

void checkQuadrant()
{
  //Quadrant 1
  if(correctedAngle >= 0 && correctedAngle <=90)
  {
    quadrantNumber = 1;
  }
  //Quadrant 2
  if(correctedAngle > 90 && correctedAngle <=180)
  {
    quadrantNumber = 2;
  }
  //Quadrant 3
  if(correctedAngle > 180 && correctedAngle <=270)
  {
    quadrantNumber = 3;
  }
  //Quadrant 4
  if(correctedAngle > 270 && correctedAngle <360)
  {
    quadrantNumber = 4;
  }
  if(quadrantNumber != previousquadrantNumber) //if we changed quadrant
  {
    if(quadrantNumber == 1 && previousquadrantNumber == 4)
    {
      numberofTurns++; // 4 --> 1 transition: CW rotation
    }

    if(quadrantNumber == 4 && previousquadrantNumber == 1)
    {
      numberofTurns--; // 1 --> 4 transition: CCW rotation
    }
    //this could be done between every quadrants so one can count every 1/4th of transition

    previousquadrantNumber = quadrantNumber;  //update to the current quadrant
  
  }  
  totalAngle = (numberofTurns*360) + correctedAngle; //number of turns (+/-) plus the actual angle within the 0-360 range
}

void checkMagnetPresence() //Only run during setup
{  
  //This function runs in the setup() and it locks the MCU until the magnet is not positioned properly
  Serial.print("Checking magnet presence...");

  while((magnetStatus & 32) != 32) //while the magnet is not adjusted to the proper distance - 32: MD = 1
  {
    magnetStatus = 0; //reset reading
    Serial.println("Magnet found!");

    Wire.beginTransmission(0x36); //connect to the sensor
    Wire.write(0x0B); //figure 21 - register map: Status: MD ML MH
    Wire.endTransmission(); //end transmission
    Wire.requestFrom(0x36, 1); //request from the sensor

    while(Wire.available() == 0); //wait until it becomes available 
    magnetStatus = Wire.read(); //Reading the data after the request   
  }      
}

void refreshDisplay()
{
  //Serial.print("Display refreshed...");
  //Serial.println(totalAngle);
  //float anglediff = 0;
  //anglediff = abs(totalAngle-previoustotalAngle);
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 30);
    display.println("Weight:");
    display.setCursor(0, 40);
    display.setTextSize(2);
    display.print(readweight);

    display.setTextSize(1);
    display.setTextColor(WHITE);
    display.setCursor(0, 0);
    display.println("totalAngle:");
    display.setCursor(0, 10);
    display.setTextSize(2);
    display.print(totalAngle);
    display.display();  
    
    previoustotalAngle = totalAngle; //update the previous value	
}

void loadcell()
{
  int button = 0;
  button = digitalRead(BUTTON_PIN);
  //int weightdiff = 0;
  
  if (button == 1){ //In case of load cell problems, reset the scale
    Serial.print("Scale tare...");
    scale.tare();
  }
    readweight = round(scale.get_units());
    //weightdiff = abs(readweight - lastread);
    lastread = readweight;
}

void checkforce()
{
  //readweight = round(scale.get_units());
  if ((round(scale.get_units())) > 50){ //Motor starts when it detects a weight of 100 grams or more
      Serial.println("Weight detected");
      motor_goin90();
    }
}

void warningretract()
{
   display.clearDisplay();
   display.setTextSize(2);
   display.setTextColor(WHITE);
   display.setCursor(0, 0);
   display.println("Warning!");
   display.setCursor(0, 30);
   display.println("Retract!");
   display.display();
}

void writetofile()
{
  myFile = SD.open("test2.txt", FILE_WRITE);
  if (myFile) {
    for (int n; n < (FULLROTATION/4); n++) //Write to file and save
    {
    myFile.print(lib[n][0]);
    myFile.print("      ");
    myFile.println(lib[n][1]);
    }
    // close the file:
    myFile.close();
    Serial.println("done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening test2.txt (writetofile function failed)");
  }
}