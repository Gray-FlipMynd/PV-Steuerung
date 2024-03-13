#include <Wire.h>
#include <RTClib.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <SPI.h>

RTC_DS3231 rtc;

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

#define SOUND_SPEED 0.034 // Sound speed in cm/uS

#define MAXSPEED 255 // Maximum Motorspeed

#define OLED_RESET  -1 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

const int R_PWM = 23; // Pin 1
const int L_PWM = 19; // Pin 2
const int R_EN = 22;  // Pin 3
const int L_EN = 21;  // Pin 4

const int TRIG_PIN = 17; // USS Trigger Pin
const int ECHO_PIN = 16; // USS Echo Pin

#define VOLTAGE_PIN 34 // Analog Pin for voltage measurement

// Distance thresholds for motor activation and deactivation
const float ACTIVATION_THRESHOLD = 15.0;
const float DEACTIVATION_THRESHOLD = 35.0;

// Retry delay in milliseconds
const unsigned long RETRY_DELAY = 1000;

// Number of readings for averaging
const int NUM_READINGS = 5;

void setup() {
  Serial.begin(115200);
  Wire.begin(5, 18);

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;) {} // Don't proceed, loop forever
  }

  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  pinMode(R_PWM, OUTPUT);
  pinMode(L_PWM, OUTPUT);
  pinMode(R_EN, OUTPUT);
  pinMode(L_EN, OUTPUT);

  digitalWrite(R_EN, HIGH);
  digitalWrite(L_EN, HIGH);

  pinMode(TRIG_PIN, OUTPUT);
  pinMode(ECHO_PIN, INPUT);
}

void loop() {
  DateTime now = rtc.now();
  float distance = averageDistanceMeasure(TRIG_PIN, ECHO_PIN);

  oled(distance);

  if (now.hour() == 12 && now.minute() == 0) 
  {
    activateMotorIfDistanceInRange(distance, 25, 3);
  }

  if (now.hour() == 14 && now.minute() == 0) 
  {
    activateMotorIfDistanceInRange(distance, 17, 3);
  }

  if (now.hour() == 15 && now.minute() == 0) 
  {
    activateMotorIfDistanceInRange(distance, 11, 3);
  }

  if (now.hour() == 20 && now.minute() == 0) 
  {
    deactivateMotorIfDistanceInRange(distance, 35);
  }
}

float distanceMeasure(const int TRIG_PIN, const int ECHO_PIN) {
  digitalWrite(TRIG_PIN, LOW);
  delayMicroseconds(2);
  digitalWrite(TRIG_PIN, HIGH);
  delayMicroseconds(10);
  digitalWrite(TRIG_PIN, LOW);
  long duration = pulseIn(ECHO_PIN, HIGH);
  float distance_cm = duration * SOUND_SPEED / 2;
  return distance_cm;
}

float averageDistanceMeasure(const int TRIG_PIN, const int ECHO_PIN) {
  float total = 0;
  for (int i = 0; i < NUM_READINGS; i++) {
    total += distanceMeasure(TRIG_PIN, ECHO_PIN);
    delay(10); // Delay between readings to stabilize the sensor
  }
  return total / NUM_READINGS;
}

void activateMotorIfDistanceInRange(float distance, float threshold, int durationMinutes) {
  if (distance >= ACTIVATION_THRESHOLD) {
    unsigned long startTime = millis();
    while (distance >= threshold && (millis() - startTime) < (durationMinutes * 60 * 1000)) {
      motor_R();
      distance = averageDistanceMeasure(TRIG_PIN, ECHO_PIN);
      oled(distance);
      delay(10); // Delay between readings to stabilize the sensor
    }
    motor_stop();
  }
}

void deactivateMotorIfDistanceInRange(float distance, float threshold) {
  if (distance <= DEACTIVATION_THRESHOLD) {
    unsigned long startTime = millis();
    while (distance <= threshold && (millis() - startTime) < RETRY_DELAY) {
      motor_L();
      distance = averageDistanceMeasure(TRIG_PIN, ECHO_PIN);
      oled(distance);
      delay(10); // Delay between readings to stabilize the sensor
    }
    motor_stop();
  }
}

void motor_R() {
  analogWrite(R_PWM, MAXSPEED);
  analogWrite(L_PWM, 0);
}

void motor_L() {
  analogWrite(R_PWM, 0);
  analogWrite(L_PWM, MAXSPEED);
}

void motor_stop() {
  analogWrite(R_PWM, 0);
  analogWrite(L_PWM, 0);
}

void oled(float distance) {
  DateTime now = rtc.now();
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
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
  display.display();
}
