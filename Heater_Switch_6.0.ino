///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//    Chinese Diesel Heater 433MHz Thermosat with OLED Display                                                                               //

//    Created by Mark Neech                                                                                                                  //

//    04/01/2024                                                                                                                             //

//    Version 6.0                                                                                                                            //                                              // 

//    Add latching push button for manual heating override to continuosly call for heat                                                      //

//    Change TEMP_THRESHOLD_LOWER and TEMP_THRESHOLD_UPPER to const float for more accuracy                                                  //                 

//    Optimise code and use AdaFruit_SSD1306.h                                                                                               //

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


#include <SPI.h>
#include <Wire.h>
#include <Adafruit_SSD1306.h>
#include <DHT.h>
#include <RCSwitch.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define DI5 5 //button pin
const int buttonPin = 5;
int buttonState = 0;

RCSwitch mySwitch = RCSwitch();

#define DHTPIN 2 // Arduino pin connected to DHT sensor
#define DHTTYPE DHT22

const float TEMP_THRESHOLD_LOWER = 19.1; // lower threshold of temperature, change to your desire value
const float TEMP_THRESHOLD_UPPER = 19.2; // upper threshold of temperature, change to your desire value

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
display.clearDisplay();             // Clear Screen Buffer
display.setTextSize(1);             // Normal 1:1 pixel scale
display.setTextColor(WHITE);        // Draw white text
display.setCursor(0,0);             // Start at top-left corner
display.println(F("Hello, world!")); 
Wire.begin();                       //initialize I2C in master mode
       
dht.begin();                        // initialize the sensor
mySwitch.enableTransmit(3);         // Using Pin #3
}

void loop() {

buttonState = digitalRead(buttonPin);
   
while (isnan(temperature));

// wait a few seconds between measurements.
delay(5000);

temperature = dht.readTemperature();;  // read temperature for Control
        
if (isnan(temperature)) {
Serial.println("Failed!");
    
} else {
if((temperature > TEMP_THRESHOLD_UPPER) and (buttonState == HIGH))  { // HIGH = OFF (button floating)
display.clearDisplay(); 
display.setTextSize(1);             
display.setTextColor(WHITE);        
display.setCursor(0,0); 
display.println(F("Heater OFF")); 
display.setCursor(0,15);   
display.print(String(temperature ).c_str());
display.println(F(" C"));
display.display();
mySwitch.send("00000111000000010000010100001000");
delay(1000); //00000111000000010000010100001000

} else if ((temperature < TEMP_THRESHOLD_LOWER) or (buttonState == LOW)) { // LOW = ON (button grounded)
display.clearDisplay(); 
display.setTextSize(1);             
display.setTextColor(WHITE);        
display.setCursor(0,0); 
display.println(F("Heater ON")); 
display.setCursor(0,15);   
display.print(String(temperature ).c_str());
display.println(F(" C"));
display.display();
mySwitch.send("00000111000000010001000110110000");
delay(1000); //00000111000000010001000110110000

      }
    }
  }
