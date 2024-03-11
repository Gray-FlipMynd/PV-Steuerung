#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include <RTClib.h>
#include <Robojax_AllegroACS_Current_Sensor.h>

RTC_DS3231 rtc;

#define MODEL 2

Robojax_AllegroACS_Current_Sensor robojax_1(MODEL, A0);
Robojax_AllegroACS_Current_Sensor robojax_2(MODEL, A1);
Robojax_AllegroACS_Current_Sensor robojax_3(MODEL, A2);
Robojax_AllegroACS_Current_Sensor robojax_4(MODEL, A3);


const int chipSelect = 4; // Chip select pin for the SD card

void setup() {
  Serial.begin(9600);

  // Initialize the RTC
  if (!rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  // If the RTC lost power and isn't configured, set it to the date and time of compiling
  if (rtc.lostPower()) {
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }

  if (!SD.begin(chipSelect)) {
    Serial.println("Card Mount Failed");
    return;
  }
  Serial.println("SD card initialized successfully.");
}

void loop() {
  // Open the file for writing
  File dataFile = SD.open("data.txt", FILE_WRITE);
  if (!dataFile) {
    Serial.println("Error opening file for writing.");
    return;
  }

  // Read the current time
  DateTime now = rtc.now();

  Serial.print("I_Modul_1,");
  Serial.print(robojax_1.getCurrent(),3);
  Serial.print(", ");
  Serial.println("A");
  Serial.print("I_Modul_2,");
  Serial.print(robojax_2.getCurrent(),3);
  Serial.print(", ");
  Serial.println("A");
  Serial.print("I_Modul_3,");
  Serial.print(robojax_3.getCurrent(),3);
  Serial.print(", ");
  Serial.println("A");
  Serial.print("I_Modul_4,");
  Serial.print(robojax_4.getCurrent(),3);
  Serial.print(", ");
  Serial.println("A");

  // Print the current time
  dataFile.print(now.hour(), DEC);
  dataFile.print(':');
  dataFile.print(now.minute(), DEC);
  dataFile.print(':');
  dataFile.print(now.second(), DEC);
  dataFile.print(" , ");
  dataFile.print("I_Modul_1,");
  dataFile.print(robojax_1.getCurrent(),3);
  dataFile.print(", A");
  dataFile.print(" , ");
  dataFile.print("I_Modul_2,");
  dataFile.print(robojax_2.getCurrent(),3);
  dataFile.print(", A");
  dataFile.print(" , ");
  dataFile.print("I_Modul_3,");
  dataFile.print(robojax_3.getCurrent(),3);
  dataFile.print(", A");
  dataFile.print(" , ");
  dataFile.print("I_Modul_4,");
  dataFile.print(robojax_4.getCurrent(),3);
  dataFile.println(", A");

  // Close the file
  dataFile.close();

  // Delay for 60 seconds before the next iteration
  delay(30000);
}
