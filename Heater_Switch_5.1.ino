///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//    Chinese Diesel Heater 433MHz Thermosat with OLED Display                                                                               //

//    Created by Mark Neech                                                                                                                  //

//    21/12/2023                                                                                                                             //

//    Version 5.1                                                                                                                            //

//    Code uses screen test from AdaFruit ssd1306 I2C example for screen failure                                                             //

//    Disabled Wire.begin(); and remove AdaFruit splash screen and continuosly update temperature                                            // 

//    Add latching push button for manual heating override to continuosly call for heat                                                      //

//    Change TEMP_THRESHOLD_LOWER and TEMP_THRESHOLD_UPPER to const float for more accuracy                                                  //

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SeeedOLED.h>
#include <DHT.h>
#include <RCSwitch.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     4 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define DI5 5 //button pin
const int buttonPin = 5;
int buttonState = 0;

RCSwitch mySwitch = RCSwitch();

#define DHTPIN 2 // Arduino pin connected to DHT sensor
#define DHTTYPE DHT22

const float TEMP_THRESHOLD_LOWER = 19.4; // lower threshold of temperature, change to your desire value
const float TEMP_THRESHOLD_UPPER = 19.5; // upper threshold of temperature, change to your desire value

DHT dht(DHTPIN, DHTTYPE);

float temperature;    // temperature in Celsius

void setup() {
  Serial.begin(9600);
  pinMode(buttonPin, INPUT_PULLUP);

  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Clear the buffer
  display.clearDisplay();
  
  //Wire.begin();               //initialize I2C in master mode
  SeeedOled.init();             //initialize the OLED
  SeeedOled.clearDisplay();     //clear the screen and set start position to top left corner
  SeeedOled.setNormalDisplay(); //Set display to normal mode (i.e non-inverse mode)
  SeeedOled.setPageMode();      //Set addressing mode to Page Mode
  dht.begin();                  // initialize the sensor
  mySwitch.enableTransmit(3);   // Using Pin #3
}

void loop() {

    buttonState = digitalRead(buttonPin);
   
    while (isnan(temperature));

    // wait a few seconds between measurements.
    delay(5000);

    temperature = dht.readTemperature();;  // read temperature for Control
        
        SeeedOled.setTextXY(3, 1);
        SeeedOled.putString(String(temperature).c_str());  //print temperature data converted to a c string
        SeeedOled.putString("C");
        Serial.print(temperature); Serial.println("C");
        Serial.println(buttonState);
        
    if (isnan(temperature)) {
        Serial.println("Failed!");
    
    } else {
      if((temperature > TEMP_THRESHOLD_UPPER) and (buttonState == HIGH))  { // HIGH = OFF (button floating)
        SeeedOled.setTextXY(0, 0);
        SeeedOled.putString("");
        SeeedOled.setTextXY(1, 0);
        SeeedOled.putString(" Heater: OFF");
        Serial.println("Heater: OFF");
        mySwitch.send("00000111000000010000010100001000");
        delay(1000); //00000111000000010000010100001000

      } else if ((temperature < TEMP_THRESHOLD_LOWER) or (buttonState == LOW)) { // LOW = ON (button grounded)
        SeeedOled.setTextXY(0, 0);
        SeeedOled.putString("");
        SeeedOled.setTextXY(1, 0);
        SeeedOled.putString(" Heater: ON  "); //Spaced after ON to clear F from OFF
        Serial.println("Heater: ON");
        mySwitch.send("00000111000000010001000110110000");
        delay(1000); //00000111000000010001000110110000

      }
    }
  }
