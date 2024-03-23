#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

RTC_DS3231 rtc;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// Define sound speed in cm/uS
#define SOUND_SPEED 0.0343

// Maximum Motor speed
#define MAX_SPEED 255

// CONSTANTS for moving avg
#define NUM_MEASUREMENTS 100
#define TRIGGER_DELAY_MICROSECONDS 1000
#define MOVING_AVERAGE_WINDOW_SIZE 10

// Uninterruptible break via millis
const long INTERVALL = 1;
const long INTERVALL_2 = 5000;
unsigned long timecheck = 0.0;

// Distance measure variables
float distance = 0.0;
const float TARGET_DISTANCE_1 = 25.0;
const float TARGET_DISTANCE_2 = 17.0;
const float TARGET_DISTANCE_3 = 11.0;
const float TARGET_DISTANCE_4 = 34.0;

// Declaration for an SSD1306 display connected to I2C (SDA, SCL pins)
#define OLED_RESET -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

// Pin definitions, VCC = Pin 7, GRD = Pin 8
const int R_PWM = 23; // Pin 1
const int L_PWM = 19; // Pin 2
const int R_EN = 22;  // Pin 3
const int L_EN = 21;  // Pin 4

// Pin definitions for USS
const int TRIG_PIN = 17;
const int ECHO_PIN = 16;

void setup() {
  Serial.begin(115200);
  Wire.begin(5, 18);

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // Don't proceed, loop forever
  }

  // Initialize RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }
  // Failsafe RTC
  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  // Set Motor control Pins
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

  // Uninterruptible break
  unsigned long clock = millis();
  if (clock - timecheck >= INTERVALL_2) 
  {
    // Distance measure
    distance = distance_measure(TRIG_PIN, ECHO_PIN);
    // OLED Display
    oled(distance);
    timecheck = clock;
  }

  // Check if it's 12:00, 14:00, or 15:00
  if (now.hour() == 12  && now.minute() == 0) 
  {
    while (distance >= TARGET_DISTANCE_1) 
    {
      motor_R();
      // Distance measure
      distance = distance_measure(TRIG_PIN, ECHO_PIN);
      // OLED Display
      oled(distance);
      delay(5);
    }
    esp_deep_sleep(3400e6);
    // Turn off the motor
    motor_stop();
  }

  // Check if it's 12:00, 14:00, or 15:00
  if (now.hour() == 14  && now.minute() == 0) 
  {
    while (distance >= TARGET_DISTANCE_2) 
    {
      motor_R();
      // Distance measure
      distance = distance_measure(TRIG_PIN, ECHO_PIN);
      // OLED Display
      oled(distance);
      delay(5);
    }
    esp_deep_sleep(3400e6);
    // Turn off the motor
    motor_stop();
  }

  // Check if it's 12:00, 14:00, or 15:00
  if (now.hour() == 15  && now.minute() == 0) 
  {
    while (distance >= TARGET_DISTANCE_3) 
    {
      motor_R();
      // Distance measure
      distance = distance_measure(TRIG_PIN, ECHO_PIN);
      // OLED Display
      oled(distance);      
      delay(5);
    }
    esp_deep_sleep(14400e6);
    // Turn off the motor
    motor_stop();
  }

  // Check if it's 20:00
  if (now.hour() == 20 && now.minute() == 0) 
  {
    // Activate the motor for X minutes to turn it back until distance of 80cm is reached
    while (distance <= TARGET_DISTANCE_4) 
    {
      motor_L();  // Change the value (0-255) for different speeds on the left motor
      // Distance measure
      distance = distance_measure(TRIG_PIN, ECHO_PIN);
      // OLED Display
      oled(distance);
      delay(5);
    }
    // Turn off the motor
    motor_stop();
    esp_deep_sleep(54000e6);
  }
}

float distance_measure(const int TRIG_PIN, const int ECHO_PIN) 
{
  float total_distance = 0;
  float measurements[MOVING_AVERAGE_WINDOW_SIZE]; // Array to store recent measurements
  int measurement_index = 0;

  for (int i = 0; i < NUM_MEASUREMENTS; i++) {
    // Trigger Signal off
    digitalWrite(TRIG_PIN, LOW);
    delayMicroseconds(2);
    // Trigger Signal on
    digitalWrite(TRIG_PIN, HIGH);
    delayMicroseconds(TRIGGER_DELAY_MICROSECONDS);
    // Trigger Signal off
    digitalWrite(TRIG_PIN, LOW);
    // Receive Echo
    long duration = pulseIn(ECHO_PIN, HIGH);
    // Calculate distance
    float distance = duration * SOUND_SPEED / 2;
    
    // Store the measurement in the array
    measurements[measurement_index] = distance;
    measurement_index = (measurement_index + 1) % MOVING_AVERAGE_WINDOW_SIZE;

    // Calculate moving average
    total_distance = 0;
    for (int j = 0; j < MOVING_AVERAGE_WINDOW_SIZE; j++) {
      total_distance += measurements[j];
    }
    total_distance /= MOVING_AVERAGE_WINDOW_SIZE;

    delay(5); // Short pause between measurements
  }
  return total_distance;
} 

// Turn motor Clockwise
void motor_R() 
{
  analogWrite(R_PWM, MAX_SPEED);
  analogWrite(L_PWM, 0);
}

// Turn motor Counterclockwise
void motor_L() 
{
  analogWrite(R_PWM, 0);
  analogWrite(L_PWM, MAX_SPEED);
}

// Stop Motor
void motor_stop() {
  analogWrite(R_PWM, 0);
  analogWrite(L_PWM, 0);
}

// Func display on the OLED
void oled(float distance) 
{
  // Get the current time
  DateTime now = rtc.now();
  // Clear the display buffer
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
