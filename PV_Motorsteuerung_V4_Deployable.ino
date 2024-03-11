#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>

RTC_DS3231 rtc;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

//define sound speed in cm/uS
#define SOUND_SPEED 0.034

//Maximum Motorspeed
#define MAXSPEED 255

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET  -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin definitions ,VCC = Pin 7 , GRD = Pin 8
const int R_PWM = 23; //Pin 1
const int L_PWM = 19; //Pin 2
const int R_EN = 22;  //Pin 3
const int L_EN = 21;  //Pin 4

// Pin definitions for USS
const int TRIG_PIN = 17;
const int ECHO_PIN = 16;

// Voltage Measuring Analog PIN
#define VOLTAGE_PIN 34 //A6

void setup() 
{
  Serial.begin(115200);
  Wire.begin(5,18);
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) 
  { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Initialize RTC
  if(!rtc.begin())
  {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  // Failsafe RTC
  if (rtc.lostPower())
  {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // Set Motorcontrol Pins
  pinMode(R_PWM, OUTPUT);
  pinMode(L_PWM, OUTPUT);
  pinMode(R_EN, OUTPUT);
  pinMode(L_EN, OUTPUT);

  digitalWrite(R_EN, HIGH);
  digitalWrite(L_EN, HIGH);

  // Set USS Pins
  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);

}

void loop() 
{
  // Get the current time
  DateTime now = rtc.now(); 
  // Measure Distance
  float distance = distance_measure(TRIG_PIN, ECHO_PIN);

  
  //OLED Display
  oled(distance);

  // Check if it's 12:00, 14:00, or 15:00
  if (now.hour() == 12 && now.minute() == 0)
  {
    float distance_3 = 0.0;
    if(distance >= 10.0)
    {
      // Activate the motor for 3 minutes
      while(distance_3 = distance_measure(TRIG_PIN, ECHO_PIN)>= 25)
      {
        motor_R();
        oled(distance_3);
      }
     
    }
    // Turn off the motor
    motor_stop();
  }

  // Check if it's 12:00, 14:00, or 15:00
  if (now.hour() == 14 && now.minute() == 0)
  {
    float distance_4 = 0.0;
    if(distance >= 10.0)
    {
      // Activate the motor for 3 minutes
      while(distance_4 = distance_measure(TRIG_PIN, ECHO_PIN)>= 17)
      {
        motor_R();
        oled(distance_4);
      }
     
    }
    // Turn off the motor
    motor_stop();  
  }

  // Check if it's 12:00, 14:00, or 15:00
  if (now.hour() == 15 && now.minute() == 0)
  {
    float distance_5 = 0.0;
    if(distance >= 10.0)
    {
      // Activate the motor for 3 minutes
      while(distance_5 = distance_measure(TRIG_PIN, ECHO_PIN)>= 11)
      {
        motor_R();
        oled(distance_5);
      }
     
    }
    // Turn off the motor
    motor_stop();  
  }

  // Check if it's 20:00
  if (now.hour() == 20 && now.minute() == 0) 
  {
    float distance_2 = 0.0;
    // Activate the motor for X minutes to turn it back until distance of 80cm is reached
    while(distance_2 = distance_measure(TRIG_PIN, ECHO_PIN)<= 35)
    {  
      motor_L();  // Change the value (0-255) for different speeds on the left motor
      oled(distance_2);
    }
    // Turn off the motor
    motor_stop();
  }
}

// Berechne den Abstand mittels USS
float distance_measure(const int TRIG_PIN, const int ECHO_PIN)
{
  //Trigger Signal aus
  digitalWrite(TRIG_PIN, LOW);
  delay(2);
  // Trigger Signal an
  digitalWrite(TRIG_PIN, HIGH);
  delay(10);
  //Trigger Signal aus
  digitalWrite(TRIG_PIN, LOW);
  //Receive Echo
  long duration = pulseIn(ECHO_PIN, HIGH);
  //Calculate distance
  float distance_cm = duration * SOUND_SPEED / 2;
  return distance_cm;
}

// Trun motor Clockwise
void motor_R()
{
  analogWrite(R_PWM, MAXSPEED);
  analogWrite(L_PWM, 0);
}

//Turn motor Counterclockwise
void motor_L()
{
  analogWrite(R_PWM, 0);
  analogWrite(L_PWM, MAXSPEED);
}

//Stop Motor
void motor_stop()
{
  analogWrite(R_PWM, 0);
  analogWrite(L_PWM, 0);
}

void oled(float distance)
{
  // Get the current time
  DateTime now = rtc.now(); 
  //Clear the display buffer
  display.clearDisplay();

  // Set text size and color
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);

  // Set cursor position (x, y) in pixels
  display.setCursor(0, 0);

  // Print date, time and distance to OLED
  display.print(now.year());
  display.print("/");
  display.print(now.month());
  display.print("/");
  display.print(now.day());
  display.print(" ");
  display.print(now.hour());
  display.print(":");
  display.print(now.minute());
  display.print(":");
  display.println(now.second());
  display.print("distance: ");
  display.print(distance);
  display.println(" cm");

  // Display text
  display.display();
}
