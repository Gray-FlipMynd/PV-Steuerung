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

//Uninteruptable break via millis
const int long INTERVALL = 1000;
unsigned long timecheck = 0.0;

//distance measure var
float distance = 0.0;

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
  
  //Uninteruptable break
  unsigned long clock = millis();  
  if (clock - timecheck >= INTERVALL)
  {
    //distance measure
    distance = distance_measure(TRIG_PIN, ECHO_PIN);
    //OLED Display
    oled(distance);
    timecheck = clock;
  }

  // Check if it's 12:00, 14:00, or 15:00
  if (now.hour() == 12 && now.minute() == 0)
  {
      // Activate the motor for 3 minutes
    while(distance >= 25)
    {
      motor_R();
      if (clock - timecheck >= INTERVALL)
      {
        //distance measure
        distance = distance_measure(TRIG_PIN, ECHO_PIN);
        //OLED Display
        oled(distance);
        timecheck = clock;
      }
      delay(5);
    }
    // Turn off the motor
    motor_stop();
  }

  // Check if it's 12:00, 14:00, or 15:00
  if (now.hour() == 14 && now.minute() == 0)
  {
    // Activate the motor for 3 minutes
    while(distance >= 17)
    {
      motor_R();
      if (clock - timecheck >= INTERVALL)
      {
        //distance measure
        distance = distance_measure(TRIG_PIN, ECHO_PIN);
        //OLED Display
        oled(distance);
        timecheck = clock;
      }
      delay(5);
    }
    // Turn off the motor
    motor_stop();  
  }

  // Check if it's 12:00, 14:00, or 15:00
  if (now.hour() == 15 && now.minute() == 0)
  {
      // Activate the motor for 3 minutes
    while(distance >= 11)
    {
      motor_R();
      if (clock - timecheck >= INTERVALL)
      {
        //distance measure
        distance = distance_measure(TRIG_PIN, ECHO_PIN);
        //OLED Display
        oled(distance);
        timecheck = clock;
      }
      delay(5);
    }
    // Turn off the motor
    motor_stop();  
  }

  // Check if it's 20:00
  if (now.hour() == 20 && now.minute() == 0) 
  {
    // Activate the motor for X minutes to turn it back until distance of 80cm is reached
    while((distance = distance_measure(TRIG_PIN, ECHO_PIN))<= 35)
    {  
      motor_L();  // Change the value (0-255) for different speeds on the left motor
      delay(5);
    }
    // Turn off the motor
    motor_stop();
  }
}

//func distance measure with 100 points avg
float distance_measure(const int TRIG_PIN, const int ECHO_PIN)
{
  float total_distance = 0;
  int num_measurements = 10; // Anzahl der Messungen

  for(int i = 0; i < num_measurements; i++)
  {
    //Trigger Signal aus
    digitalWrite(TRIG_PIN, LOW);
    delay(2);
    // Trigger Signal an
    digitalWrite(TRIG_PIN, HIGH);
    delay(5);
    //Trigger Signal aus
    digitalWrite(TRIG_PIN, LOW);
    //Receive Echo
    long duration = pulseIn(ECHO_PIN, HIGH);
    //Calculate distance
    float distance = duration * SOUND_SPEED / 2;
    total_distance += distance;
    delay(5); // kurze Pause zwischen den Messungen
  }
  return total_distance / num_measurements; // Durchschnittliche Entfernung zurückgeben
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

//func display on the OLED
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
