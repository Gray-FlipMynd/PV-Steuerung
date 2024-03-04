#include <SD.h>
#include <SPI.h>
#include <Wire.h>
#include <RTClib.h>

#define SD_CS 5   // Chip select pin for the SD card
#define SD_MOSI 23 // MOSI pin for SPI communication
#define SD_MISO 19 // MISO pin for SPI communication
#define SD_SCK 18  // SCK pin for SPI communication

#define A0 32

RTC_DS3231 rtc;

File dataFile;

void setup() 
{
  Serial.begin(115200);
  delay(1000);

  Wire.begin(16,17);

  if (!rtc.begin()) 
  {
    Serial.println("Couldn't find RTC");
    while (1);
  }

  if (rtc.lostPower()) 
  {
    Serial.println("RTC lost power, let's set the time!");
    rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
  }
  
  
  // Initialize SPI communication and SD card
  SPI.begin(SD_SCK, SD_MISO, SD_MOSI, SD_CS);
  
  if (!SD.begin(SD_CS)) 
  {
    Serial.println("Card Mount Failed");
    return;
  }
  Serial.println("SD card initialized successfully.");
  
  // Open the file for appending
  dataFile = SD.open("/data.txt", FILE_APPEND);
  if (!dataFile) 
  {
    Serial.println("Error opening file for appending.");
  }
}

void loop() 
{

  DateTime now = rtc.now();

  int sensor_value = analogRead(A0);
  float voltage = sensor_value * (3.3 / 4095);


  if (dataFile) {
    dataFile.print(now.hour());
    dataFile.print(":");
    dataFile.print(now.minute());
    dataFile.print(":");
    dataFile.print(now.second());
    dataFile.print("  :  ");
    dataFile.print("V_bat = ");
    dataFile.print(voltage);
    dataFile.println(" V");
    dataFile.flush(); // Ensure data is written to the file
    Serial.println("Data written to file.");
  } else {
    Serial.println("Error writing to file.");
  }
  
  delay(1000);
}
