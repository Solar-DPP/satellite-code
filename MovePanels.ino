void initPanels() {

  pinMode(S_x_one_PIN, INPUT_PULLDOWN);
  pinMode(S_x_two_PIN, INPUT_PULLDOWN);
  pinMode(S_y_one_PIN, INPUT_PULLDOWN);
  pinMode(S_y_two_PIN, INPUT_PULLDOWN);

  attachInterrupt(S_x_one_PIN, set_x_one, CHANGE);
  attachInterrupt(S_x_two_PIN, set_x_two, CHANGE);
  attachInterrupt(S_y_one_PIN, set_y_one, CHANGE);
  attachInterrupt(S_y_two_PIN, set_y_two, CHANGE);

  pinMode(MOTOR_EN_PIN1, OUTPUT);
  pinMode(DIR_PIN1, OUTPUT);
  pinMode(STEP_PIN1, OUTPUT);

  pinMode(MOTOR_EN_PIN2, OUTPUT);
  pinMode(DIR_PIN2, OUTPUT);
  pinMode(STEP_PIN2, OUTPUT);

  digitalWrite(MOTOR_EN_PIN1, 0);
  digitalWrite(MOTOR_EN_PIN2, 0);
}


void rotate(uint8_t dir = 0, uint16_t count = 1600, uint8_t step_pin = 0, uint8_t dir_pin = 0) {
  digitalWrite(dir_pin, dir);
  for (int i = 0; i <= count; i++) {
    digitalWrite(step_pin, 1);
    delay(speed_time);
    digitalWrite(step_pin, 0);
    delay(speed_time);
  }
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
  AbsolutePosition();

  if (num_photores == 0) {
    bool _f = false;

    while (!_f) {
      if (ina1.getPower() > last_power) {
        last_power = ina1.getPower();
        //        stepper1.setTarget(800, RELATIVE);
        //        stepper2.setTarget(-800, RELATIVE);
        rotate(1, 1000, STEP_PIN1, DIR_PIN1);
        rotate(1, 1000, STEP_PIN2, DIR_PIN2);
      } else {
        //        stepper1.setTarget(-800, RELATIVE);
        //        stepper2.setTarget(800, RELATIVE);
        rotate(0, 1000, STEP_PIN1, DIR_PIN1);
        rotate(0, 1000, STEP_PIN2, DIR_PIN2);
        _f = true;
      }
      delay(100);
    }
  }
  else if (num_photores == 1) {
    bool _f = false;
    //    if (!stepper1.tick()) stepper1.setTarget(800, ABSOLUTE);
    //    stepper2.setTarget(800, ABSOLUTE);

    while (!_f) {
      if (ina1.getPower() > last_power) {
        last_power = ina1.getPower();
        rotate(1, 1000, STEP_PIN1, DIR_PIN1);
        rotate(0, 1000, STEP_PIN2, DIR_PIN2);
      } else {
        //        stepper1.setTarget(-800, RELATIVE);
        //        stepper2.setTarget(-800, RELATIVE);
        rotate(0, 1000, STEP_PIN1, DIR_PIN1);
        rotate(1, 1000, STEP_PIN2, DIR_PIN2);
        _f = true;
      }
      delay(100);
    }
  }
  else if (num_photores == 2) {
    bool _f = false;
    while (!_f) {
      if (ina1.getPower() > last_power) {
        last_power = ina1.getPower();
        rotate(0, 1000, STEP_PIN1, DIR_PIN1);
        rotate(1, 1000, STEP_PIN2, DIR_PIN2);
      } else {
        rotate(1, 1000, STEP_PIN1, DIR_PIN1);
        rotate(0, 1000, STEP_PIN2, DIR_PIN2);
        _f = true;
      }
      delay(100);
    }
  }
  else if (num_photores == 3) {
    bool _f = false;

    while (!_f) {
      if (ina1.getPower() > last_power) {
        last_power = ina1.getPower();
        rotate(0, 1000, STEP_PIN1, DIR_PIN1);
        rotate(0, 1000, STEP_PIN2, DIR_PIN2);

      } else {
        rotate(1, 1000, STEP_PIN1, DIR_PIN1);
        rotate(1, 1000, STEP_PIN2, DIR_PIN2);

        _f = true;
      }
      delay(100);
    }
  }
}
