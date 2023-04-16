#include <SPI.h>
#include <SD.h>

#define SD_PIN PA4


void init_log() {
  Serial.begin(9600);
  SD.begin(SD_PIN);
}

void log(char data[]) {
  File sdFile = SD.open("logs.txt", FILE_WRITE);
  if (sdFile) {
    sdFile.println(data);
    sdFile.close();
  };    

  Serial.println(data);
};


void setup() {
  init_log();
}

void loop() {
  float voltage_lenses = 0.1; // TODO: add read INA219
  float amperage_lenses = 0.1; // TODO: add read INA219
  
  float voltage_standard = 0.01; // TODO: add read INA219
  float amperage_standard = 0.01; // TODO: add read INA219

  int x_position = 0; // TODO: add read position x
  int y_position = 0; // TODO: add read position y

  float li_top = 0.1;
  float li_bottom = 0.2;
  float li_left = 0.3;
  float li_right = 0.4;


  char message[] = "";

  ultoa(millis(), message + strlen(message), DEC);
  strcat(message, ";");

  strcat(message, "wait");
  strcat(message, ";");

  dtostrf(voltage_lenses, 2, 3, message + strlen(message));
  strcat(message, ";");
  dtostrf(amperage_lenses, 2, 3, message + strlen(message));
  strcat(message, ";");

  dtostrf(voltage_standard, 2, 3, message + strlen(message));
  strcat(message, ";");
  dtostrf(amperage_standard, 2, 3, message + strlen(message));
  strcat(message, ";");

  itoa(x_position, message + strlen(message), DEC);
  strcat(message, ";");
  itoa(y_position, message + strlen(message), DEC);
  strcat(message, ";");

  dtostrf(li_top, 2, 3, message + strlen(message));
  strcat(message, ";");
  dtostrf(li_bottom, 2, 3, message + strlen(message));
  strcat(message, ";");
  dtostrf(li_left, 2, 3, message + strlen(message));
  strcat(message, ";");
  dtostrf(li_right, 2, 3, message + strlen(message));

  log(message);

  static bool enable_sail = false;
  if (enable_sail) {};
}


