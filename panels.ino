#define RIGHT 1
#define LEFT 0
#define DOWN 1
#define UP 0
#define DIFFERENCE_PRECENT 10

enum MoveDirection {
  LEFT_TOP,
  LEFT_CENTER,
  LEFT_BOTTOM,

  CENTER_TOP,
  CENTER_CENTER,
  CENTER_BOTTOM,

  RIGHT_TOP,
  RIGHT_CENTER,
  RIGHT_BOTTOM,
};

MoveDirection currentDirection; 

// a crutch for the arduino ide (see https://stackoverflow.com/questions/62683653/variable-or-field-declared-void-error-on-ardunio-compiler)
void movePanelsTo(MoveDirection direction);
void stepMotor(uint8_t dir, uint8_t step_pin, uint8_t dir_pin);

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

void updatePanelPosition() {
  uint8_t _matrix[4];
  getLightMatrix(_matrix);

  uint8_t num_photo_1;
  uint8_t max_photo_1;

  uint8_t num_photo_2;
  uint8_t max_photo_2;

  uint8_t num_photo_3;
  uint8_t max_photo_3;

  uint8_t num_photo_4;
  uint8_t max_photo_4;


  for (int i = 0; i < 4; i++) {
    if (_matrix[i] > max_photo_1) { max_photo_1 = _matrix[i]; num_photo_1 = i; }
  }
  _matrix[num_photo_1] = 0;

  for (int i = 0; i < 4; i++) {
    if (_matrix[i] > max_photo_2) { max_photo_2 = _matrix[i]; num_photo_2 = i; }
  }
  _matrix[num_photo_2] = 0;

  for (int i = 0; i < 4; i++) {
    if (_matrix[i] > max_photo_3) { max_photo_3 = _matrix[i]; num_photo_3 = i; }
  }
  _matrix[num_photo_3] = 0;

  for (int i = 0; i < 4; i++) {
    if (_matrix[i] > max_photo_4) { max_photo_4 = _matrix[i]; num_photo_4 = i; }
  }
  _matrix[num_photo_4] = 0;

  if (max_photo_1 - max_photo_4 < DIFFERENCE_PRECENT) {
    movePanelsTo(CENTER_CENTER);
  } else {
    uint8_t count = 0;

    if (max_photo_1 - max_photo_2 > DIFFERENCE_PRECENT) {
      count = 1;
    } else {
      count = 2;
    }

    if (count == 1) {
      switch (num_photo_1) {
        case 0:
          movePanelsTo(RIGHT_BOTTOM);
          break;
        case 1:
          movePanelsTo(RIGHT_TOP);
          break;
        case 2:
          movePanelsTo(LEFT_BOTTOM);
          break;
        case 3:
          movePanelsTo(LEFT_TOP);
          break;
      }
    } else if (count == 2) {
      if ((num_photo_1 == 0 && num_photo_2 == 1) || (num_photo_1 == 1 && num_photo_2 == 0)) {
        movePanelsTo(RIGHT_CENTER);
      } else if ((num_photo_1 == 0 && num_photo_2 == 2) || (num_photo_1 == 2 && num_photo_2 == 0)) {
        movePanelsTo(CENTER_BOTTOM);
      } else if ((num_photo_1 == 2 && num_photo_2 == 3) || (num_photo_1 == 3 && num_photo_2 == 2)) {
        movePanelsTo(LEFT_CENTER);
      } else if ((num_photo_1 == 3 && num_photo_2 == 1) || (num_photo_1 == 1 && num_photo_2 == 2)) {
        movePanelsTo(CENTER_TOP);
      }
    }
  }
};



void movePanelsTo(MoveDirection direction) {
  currentDirection = direction;

  _moveToLeftTop();
  unsigned long start_time;

  switch (direction) {
    case LEFT_TOP:
      break;

    case LEFT_CENTER:
      start_time = millis();
      while (millis() - start_time <= y_interval / 2) {
        stepMotor(DOWN, STEP_PIN2, DIR_PIN2);
      }
      break;

    case LEFT_BOTTOM:
      while (y_down_end.state() == true) {
        stepMotor(DOWN, STEP_PIN2, DIR_PIN2);
      }
      break;

    case CENTER_TOP:
      start_time = millis();
      while (millis() - start_time <= x_interval / 2) {
        stepMotor(RIGHT, STEP_PIN1, DIR_PIN1);
      }
      break;

    case CENTER_CENTER:
      start_time = millis();
      while (millis() - start_time <= x_interval / 2) {
        stepMotor(RIGHT, STEP_PIN1, DIR_PIN1);
      }

      start_time = millis();
      while (millis() - start_time <= y_interval / 2) {
        stepMotor(DOWN, STEP_PIN2, DIR_PIN2);
      }
      break;

    case CENTER_BOTTOM:
      start_time = millis();
      while (millis() - start_time <= x_interval / 2) {
        stepMotor(RIGHT, STEP_PIN1, DIR_PIN1);
      }

      while (y_down_end.state() == true) {
        stepMotor(DOWN, STEP_PIN2, DIR_PIN2);
      }
      break;

    case RIGHT_TOP:
      while (x_right_end.state() == true) {
        stepMotor(RIGHT, STEP_PIN1, DIR_PIN1);
      }
      break;

    case RIGHT_CENTER:
      while (x_right_end.state() == true) {
        stepMotor(RIGHT, STEP_PIN1, DIR_PIN1);
      }

      start_time = millis();
      while (millis() - start_time <= y_interval / 2) {
        stepMotor(DOWN, STEP_PIN2, DIR_PIN2);
      }
      break;

    case RIGHT_BOTTOM:
      while (x_right_end.state() == true) {
        stepMotor(RIGHT, STEP_PIN1, DIR_PIN1);
      }

      while (y_down_end.state() == true) {
        stepMotor(DOWN, STEP_PIN2, DIR_PIN2);
      }
      break;
  }
}

void _moveToLeftTop() {
  while (x_left_end.state() == true) {
    stepMotor(LEFT, STEP_PIN1, DIR_PIN1);
  }

  while (y_up_end.state() == true) {
    stepMotor(UP, STEP_PIN2, DIR_PIN2);
  }
}

void stepMotor(uint8_t dir = 0, uint8_t step_pin = 0, uint8_t dir_pin = 0) {
  digitalWrite(dir_pin, dir);
  digitalWrite(step_pin, 1);
  delay(speed_time);
  digitalWrite(step_pin, 0);
  delay(speed_time);
}




/*
  motor1 = x
  motor2 = y
 * 1 - Right / Down
 * 0 - Left / Up
 */
