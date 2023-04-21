void set_x_one() {
  //  s_x_one = !s_x_one;
  //  Serial.print("x_one: ");
  //  Serial.println(s_x_one);
  x_one.tickISR();
//  digitalWrite(MOTOR_EN_PIN1, !x_one.state());
}
void set_x_two() {
  //  s_x_two = !s_x_two;
  //  Serial.print("x_two: ");
  //  Serial.println(s_x_two);
  x_two.tickISR();
//  digitalWrite(MOTOR_EN_PIN1, !x_two.state());
}

void set_y_one() {
  //  s_y_one = !s_y_one;
  //  Serial.print("y_one: ");
  //  Serial.println(s_y_one);
  y_one.tickISR();
//  digitalWrite(MOTOR_EN_PIN2, !y_one.state());
}

void set_y_two() {
  //  s_y_two = !s_y_two;
  //  Serial.print("y_two: ");
  //  Serial.println(s_y_two);
  y_two.tickISR();
//  digitalWrite(MOTOR_EN_PIN2, !y_two.state());
}
