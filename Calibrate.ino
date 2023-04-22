void calibrate() {
  // Едем до упора по x
  digitalWrite(DIR_PIN1, 1);
  while (x_right_end.state() == true) {
    digitalWrite(STEP_PIN1, 1);
    delay(speed_time);
    digitalWrite(STEP_PIN1, 0);
    delay(speed_time);
  }

  // Едем до упора по x в другую сторону
  unsigned long start_time = millis();
  digitalWrite(DIR_PIN1, 0);
  while (x_left_end.state() == true) {
    digitalWrite(STEP_PIN1, 1);
    delay(speed_time);
    digitalWrite(STEP_PIN1, 0);
    delay(speed_time);
  }
  unsigned long end_time = millis();

  x_interval = end_time - start_time;
  Serial.println(x_interval);

  // Едем до упора по y
  digitalWrite(DIR_PIN2, 1);
  while (y_down_end.state() == true) {
    digitalWrite(STEP_PIN2, 1);
    delay(speed_time);
    digitalWrite(STEP_PIN2, 0);
    delay(speed_time);
  }

  // Едем до упора по y в другую сторону
  start_time = millis();
  digitalWrite(DIR_PIN2, 0);
  while (y_up_end.state() == true) {
    digitalWrite(STEP_PIN2, 1);
    delay(speed_time);
    digitalWrite(STEP_PIN2, 0);
    delay(speed_time);
  }
  end_time = millis();

  y_interval = end_time - start_time;
  Serial.println(y_interval);
  
  AbsolutePosition();
}


void AbsolutePosition(){
  unsigned long start_time = millis();
  digitalWrite(DIR_PIN1, 1);
  while (millis() - start_time <= x_interval / 2) {
    digitalWrite(STEP_PIN1, 1);
    delay(speed_time);
    digitalWrite(STEP_PIN1, 0);
    delay(speed_time);
  }

  start_time = millis();
  digitalWrite(DIR_PIN2, 0);
  while (millis() - start_time <= y_interval / 2) {
    digitalWrite(STEP_PIN2, 1);
    delay(speed_time);
    digitalWrite(STEP_PIN2, 0);
    delay(speed_time);
  }
}

/*
 * 1 - Right / Down
 * 0 - Left / Up
 */
