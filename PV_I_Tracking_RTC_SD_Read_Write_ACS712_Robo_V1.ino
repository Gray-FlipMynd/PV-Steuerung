#include <SPI.h>
#include <Wire.h>
#include <SD.h>
#include <RTClib.h>
#include <Robojax_AllegroACS_Current_Sensor.h>

RTC_DS3231 rtc;

#define MODEL 2

const int numSensors = 4; // Number of sensors
Robojax_AllegroACS_Current_Sensor sensors[numSensors] = {
  Robojax_AllegroACS_Current_Sensor(MODEL, A0),
  Robojax_AllegroACS_Current_Sensor(MODEL, A1),
  Robojax_AllegroACS_Current_Sensor(MODEL, A2),
  Robojax_AllegroACS_Current_Sensor(MODEL, A3)
};

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

void loop() 
{
  // Open the file for writing
  File dataFile = SD.open("data.txt", FILE_WRITE);
  if (!dataFile) {
    Serial.println("Error opening file for writing.");
    return;
  }

  // Read the current time
  DateTime now = rtc.now();

  // Only 2 Sensors in use atm
  for (int i = 0; i < numSensors-2; i++) 
  {
    if (sensors[i].getCurrent() >= 800)
    {
      dataFile.print(now.hour(), DEC);
      dataFile.print(':');
      dataFile.print(now.minute(), DEC);
      dataFile.print(':');
      dataFile.print(now.second(), DEC);
      dataFile.print(':');
      dataFile.print(now.day(), DEC);
      dataFile.print(':');
      dataFile.print(now.month(), DEC);
      dataFile.print(':');
      dataFile.print(now.year(), DEC);
      dataFile.print(" , ");
      dataFile.print("I_Modul_");
      dataFile.print(i + 1);
      dataFile.print(",");
      dataFile.print(sensors[i].getCurrent(), 3);
      dataFile.println(", mA");
    }
  }

  // Close the file
  dataFile.close();

  // Delay for 30 seconds before the next iteration
  delay(30000);
}
