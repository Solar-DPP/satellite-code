#include <SPI.h>
#include <SD.h>
#include <GyverINA.h>
#include <MCP3008.h>

#define CS_PIN PB12
#define CLOCK_PIN PB13
#define MOSI_PIN PB15
#define MISO_PIN PB14

#define MOTOR_EN_PIN PB0

const uint8_t adr1 = 0x41;
const uint8_t adr2 = 0x44;

unsigned long tmr;

#define SD_PIN PA4

INA219 ina1(adr1);
INA219 ina2(adr2);
MCP3008 adc(CLOCK_PIN, MOSI_PIN, MISO_PIN, CS_PIN);


char *filename = "logs.txt";

void log(char *data) {
  File sdFile = SD.open(filename, FILE_WRITE);
  if (sdFile) {
    sdFile.println(data);
    sdFile.close();
  };

  Serial.println(data);
};


void setup() {
  Serial.begin(115200);
  SD.begin(SD_PIN);

  SD.remove(filename);

  if (ina1.begin()) Serial.println("Ina1 0x41 begin");
  else {
    Serial.println("Ina1 0x41 failed");
    while (1);
  }

  if (ina2.begin()) Serial.println("Ina2 0x44 begin");
  else {
    Serial.println("Ina2 0x44 failed");
    while (1);
  }

  Serial.print(F("Ina1 0x41 Calibration value: ")); Serial.println(ina1.getCalibration());
  Serial.print(F("Ina2 0x44 Calibration value: ")); Serial.println(ina2.getCalibration());
}

void loop() {
  //  float voltage_lenses = ina1.getVoltage(); // TODO: add read INA219
  //  float amperage_lenses = ina1.getCurrent(); // TODO: add read INA219
  //
  //  float voltage_standard = ina2.getVoltage(); // TODO: add read INA219
  //  float amperage_standard = ina2.getCurrent(); // TODO: add read INA219
  //
  //  int x_position = 0; // TODO: add read position x
  //  int y_position = 0; // TODO: add read position y
  //
  //  float li_top = 0.1;
  //  float li_bottom = 0.2;
  //  float li_left = 0.3;
  //  float li_right = 0.4;
  //
  //
  //
  //  ultoa(millis(), message + strlen(message), DEC);
  //  strcat(message, ";");
  //
  //  strcat(message, "wait");
  //  strcat(message, ";");
  //
  //  dtostrf(voltage_lenses, 2, 3, message + strlen(message));
  //  strcat(message, ";");
  //  dtostrf(amperage_lenses, 2, 3, message + strlen(message));
  //  strcat(message, ";");
  //
  //  dtostrf(voltage_standard, 2, 3, message + strlen(message));
  //  strcat(message, ";");
  //  dtostrf(amperage_standard, 2, 3, message + strlen(message));
  //  strcat(message, ";");
  //
  //  itoa(x_position, message + strlen(message), DEC);
  //  strcat(message, ";");
  //  itoa(y_position, message + strlen(message), DEC);
  //  strcat(message, ";");
  //  char message[512] = "";
  //
  //  dtostrf(li_top, 2, 3, message + strlen(message));
  //  strcat(message, ";");
  //  dtostrf(li_bottom, 2, 3, message + strlen(message));
  //  strcat(message, ";");
  //  dtostrf(li_left, 2, 3, message + strlen(message));
  //  strcat(message, ";");
  //  dtostrf(li_right, 2, 3, message + strlen(message));
  //
  //  log(message);
  //
  if (millis() - tmr > 1000) {
    tmr = millis();
    logger();
  }

}


void logger() {
  char msg[512];

  char v_lens[6];
  char a_lens[6];

  char v_stad[6];
  char a_stad[6];

  dtostrf(ina1.getVoltage(), 5, 3, v_lens);
  dtostrf(ina1.getCurrent(), 5, 3, a_lens);

  dtostrf(ina2.getVoltage(), 5, 3, v_stad);
  dtostrf(ina2.getCurrent(), 5, 3, a_stad);

  sprintf(msg, "%lu;%s;%s;%s;%s;%u;%u;%u;%u",
          millis(),
          v_lens, a_lens, v_stad, a_stad,
          getLeft_t(), getLeft_b(), getRight_t(), getRight_b()
         );

  log(msg);
}


int getLeft_t() {
  return map(adc.readADC(0), 0, 1023, 0, 100);
}


int getLeft_b() {
  return map(adc.readADC(3), 0, 1023, 0, 100);
}

int getRight_t() {
  return map(adc.readADC(1), 0, 1023, 0, 100);
}

int getRight_b() {
  return map(adc.readADC(2), 0, 1023, 0, 100);
}
