#include <SPI.h>
#include <SD.h>
#include <GyverINA.h>
#include <MCP3008.h>
#include <microDS18B20.h>

#define DS_PIN PA0

#define CS_PIN PB12
#define CLOCK_PIN PB13
#define MOSI_PIN PB15
#define MISO_PIN PB14

#define MOTOR_EN_PIN PB0

#define SD_PIN PA4

uint8_t s1_addr[] = {0x28, 0xFF, 0xCD, 0x59, 0x51, 0x17, 0x4, 0xFE};
uint8_t s2_addr[] = {0x28, 0xFF, 0x36, 0x94, 0x65, 0x15, 0x2, 0x80};

const uint8_t adr1 = 0x41;
const uint8_t adr2 = 0x44;

unsigned long tmr;


MicroDS18B20<DS_PIN, s1_addr> sensor1;
MicroDS18B20<DS_PIN, s2_addr> sensor2;

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
  Serial.begin(115200, SERIAL_8E1);
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
  if (millis() - tmr > 1000) {
    tmr = millis();
    logger();

    sensor1.requestTemp();
    sensor2.requestTemp();
  }

}


void logger() {
  char msg[512];

  char v_lens[5];
  char a_lens[5];

  char v_stad[5];
  char a_stad[5];

  char temp1[4];
  char temp2[4];

  dtostrf(ina1.getVoltage(), 5, 3, v_lens);
  dtostrf(ina1.getCurrent(), 5, 3, a_lens);

  dtostrf(ina2.getVoltage(), 5, 3, v_stad);
  dtostrf(ina2.getCurrent(), 5, 3, a_stad);

  if (sensor1.readTemp()) dtostrf(sensor1.getTemp(), 4, 2, temp1);
  if (sensor2.readTemp()) dtostrf(sensor2.getTemp(), 4, 2, temp2);
  

  sprintf(msg, "%lu;%s;%s;%s;%s;%u;%u;%u;%u;%i;%i",
          millis(),
          v_lens, a_lens, v_stad, a_stad,
          getLeft_t(), getLeft_b(), getRight_t(), getRight_b(),
          temp1, temp2
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
