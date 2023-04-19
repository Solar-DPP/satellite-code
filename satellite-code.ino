#include <SPI.h>
#include <SD.h>
#include <GyverINA.h>
#include <MCP3008.h>
#include <microDS18B20.h>
#include <GyverStepper.h>

#define DS_PIN PA0

#define CS_PIN PB12
#define CLOCK_PIN PB13
#define MOSI_PIN PB15
#define MISO_PIN PB14

#define MOTOR_EN_PIN PB0
#define DIR_PIN1   PA15
#define STEP_PIN1  PA12

#define DIR_PIN2   PA11
#define STEP_PIN2  PA8

#define SD_PIN PA4

uint8_t s1_addr[] = {0x28, 0xFF, 0xCD, 0x59, 0x51, 0x17, 0x4, 0xFE};
uint8_t s2_addr[] = {0x28, 0xFF, 0x36, 0x94, 0x65, 0x15, 0x2, 0x80};

const uint8_t adr1 = 0x41;
const uint8_t adr2 = 0x44;

unsigned long tmr;
unsigned long tmr1;

char *filename = "logs.txt";

bool ina1_work_flag = false;
bool ina2_work_flag = false;

uint8_t max_photores = 0;

MicroDS18B20<DS_PIN, s1_addr> sensor1;
MicroDS18B20<DS_PIN, s2_addr> sensor2;

INA219 ina1(adr1);
INA219 ina2(adr2);

MCP3008 adc(CLOCK_PIN, MOSI_PIN, MISO_PIN, CS_PIN);

GStepper<STEPPER2WIRE> stepper1(1600, STEP_PIN1, DIR_PIN1, MOTOR_EN_PIN);
GStepper<STEPPER2WIRE> stepper2(1600, STEP_PIN2, DIR_PIN2, MOTOR_EN_PIN);

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

  if (ina1.begin()) {
    log("Ina1 0x41 begin");
    ina1_work_flag = true;
  }
  else log("Ina1 0x41 failed");

  if (ina2.begin()) {
    log("Ina2 0x44 begin");
    ina2_work_flag = true;
  }
  else log("Ina2 0x44 failed");

  if (ina1_work_flag == true && ina2_work_flag == true) {
    log("Ina1 0x41 Calibration value:"); log(ina1.getCalibration());
    log("Ina2 0x44 Calibration value:"); log(ina2.getCalibration());
  }

  stepper1.setRunMode(FOLLOW_POS);
  stepper2.setRunMode(FOLLOW_POS);

  stepper1.setAcceleration(0);
  stepper1.setMaxSpeed(300);
  stepper1.enable();
  
  stepper2.setAcceleration(0);
  stepper2.setMaxSpeed(300);
  stepper2.enable();
}

void loop() {
  if (millis() - tmr > 1000) {
    tmr = millis();
    logger();

    sensor1.requestTemp();
    sensor2.requestTemp();

  }
  if (millis() - tmr1 > 15000) {
    tmr1 = millis();
    calcChangeLight(getLeft_t());
    calcChangeLight(getLeft_b());

    calcChangeLight(getRight_t());
    calcChangeLight(getRight_b());
  }
  stepper1.tick();
  stepper2.tick();
}

void calcChangeLight(uint8_t sensor) {
  if (abs(max_photores - sensor) >= 10) {
    Serial.println(abs(max_photores - sensor));
    MovePanels();
  }
}


void logger() {
  char msg[512];
  
  uint8_t x = stepper1.getCurrent();
  uint8_t y = stepper1.getCurrent();
 
  char temp1[5] = "0.0";
  char temp2[5] = "0.0";

  char v_lens[5] = "0.0";
  char a_lens[5] = "0.0";

  char v_stad[5] = "0.0";
  char a_stad[5] = "0.0";


  if (ina1_work_flag == true && ina2_work_flag == true) {
    dtostrf(ina1.getVoltage(), 5, 3, v_lens);
    dtostrf(ina1.getCurrent(), 5, 3, a_lens);

    dtostrf(ina2.getVoltage(), 5, 3, v_stad);
    dtostrf(ina2.getCurrent(), 5, 3, a_stad);
  }
  if (sensor1.readTemp()) dtostrf(sensor1.getTemp(), 4, 2, temp1);
  if (sensor2.readTemp()) dtostrf(sensor2.getTemp(), 4, 2, temp2);

  sprintf(msg, "%lu;%s;%s;%s;%s;%i;%i;%u;%u;%u;%u;%s;%s",
          millis(),
          v_lens, a_lens, v_stad, a_stad,
          x, y,
          getLeft_t(), getLeft_b(), getRight_t(), getRight_b(),
          temp1, temp2
         );

  log(msg);
}


uint8_t getLeft_t() {
  return map(adc.readADC(0), 0, 1023, 0, 100);
}


uint8_t getLeft_b() {
  return map(adc.readADC(3), 0, 1023, 0, 100);
}

uint8_t getRight_t() {
  return map(adc.readADC(1), 0, 1023, 0, 100);
}

uint8_t getRight_b() {
  return map(adc.readADC(2), 0, 1023, 0, 100);
}

void getLightMatrix(uint8_t *matrix) {
  matrix[0] = getLeft_t();
  matrix[1] = getLeft_b();
  matrix[2] = getRight_t();
  matrix[3] = getRight_b();
}


void MovePanels() {
  uint8_t _matrix[4];
  getLightMatrix(_matrix);
  uint8_t num_photores;
  uint8_t max_photo = 0;

  float last_power = 0.0;

  for (int i = 0; i < 4; i++) {
    if (_matrix[i] > max_photo) {
      max_photo = _matrix[i];
      num_photores = i;
    }
  }
  max_photores = max_photo;
  Serial.println(num_photores);
  Serial.println(ina1.getPower());
  if (num_photores == 0) {
    bool _f = false;
    stepper1.setTarget(800, ABSOLUTE);
    stepper2.setTarget(-800, ABSOLUTE);

    while (!_f) {
      if (ina1.getPower() > last_power) {
        last_power = ina1.getPower();
        stepper1.setTarget(800, RELATIVE);
        stepper2.setTarget(-800, RELATIVE);
      } else {
        stepper1.setTarget(-800, RELATIVE);
        stepper2.setTarget(800, RELATIVE);
        _f = true;
      }
      delay(100);
    }
  }
  else if (num_photores == 1) {
    bool _f = false;
    if (!stepper1.tick()) stepper1.setTarget(800, ABSOLUTE);
    stepper2.setTarget(800, ABSOLUTE);

    while (!_f) {
      if (ina1.getPower() > last_power) {
        last_power = ina1.getPower();
        stepper1.setTarget(800, RELATIVE);
        stepper2.setTarget(800, RELATIVE);
      } else {
        stepper1.setTarget(-800, RELATIVE);
        stepper2.setTarget(-800, RELATIVE);
        _f = true;
      }
      delay(100);
    }
  }
  else if (num_photores == 2) {
    bool _f = false;
    if (!stepper1.tick()) stepper1.setTarget(-800, ABSOLUTE);
    stepper2.setTarget(-800, ABSOLUTE);

    while (!_f) {
      if (ina1.getPower() > last_power) {
        last_power = ina1.getPower();
        stepper1.setTarget(-800, RELATIVE);
        stepper2.setTarget(-800, RELATIVE);
      } else {
        stepper1.setTarget(800, RELATIVE);
        stepper2.setTarget(800, RELATIVE);
        _f = true;
      }
      delay(100);
    }
  }
  else if (num_photores == 3) {
    bool _f = false;
    if (!stepper1.tick()) stepper1.setTarget(-800, ABSOLUTE);
    stepper2.setTarget(800, ABSOLUTE);

    while (!_f) {
      if (ina1.getPower() > last_power) {
        last_power = ina1.getPower();
        stepper1.setTarget(-800, RELATIVE);
        stepper2.setTarget(800, RELATIVE);
      } else {
        stepper1.setTarget(800, RELATIVE);
        stepper2.setTarget(-800, RELATIVE);
        _f = true;
      }
      delay(100);
    }
  }
}

//void algo(uint8_t x, uint8_t y) {
//  bool _f = false;
//  stepper1.setTarget(x, ABSOLUTE);
//  stepper2.setTarget(y, ABSOLUTE);
//
//  while (!_f) {
//    if (ina1.getPower() > last_power) {
//      last_power = ina1.getPower();
//      stepper1.setTarget(!x, RELATIVE);
//      stepper2.setTarget(y, RELATIVE);
//    } else {
//      stepper1.setTarget(x, RELATIVE);
//      stepper2.setTarget(!y, RELATIVE);
//      _f = true;
//    }
//  }
//}
