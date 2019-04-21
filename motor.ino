
/**************************************************

  モーター制御に関する関数群

 **************************************************/
/**
  左右2つのモーターに接続されるピンを初期化
*/
void motorSetup(const int m1_l, const int m1_r, const int m2_l, const int m2_r) {
  pinMode(m1_l, OUTPUT);
  pinMode(m1_r, OUTPUT);
  pinMode(m2_l, OUTPUT);
  pinMode(m2_r, OUTPUT);

  digitalWrite(m1_l, LOW);
  digitalWrite(m1_r, LOW);
  digitalWrite(m2_l, LOW);
  digitalWrite(m2_r, LOW);

  analogWriteFreq(5);
}

/**
  モーターをPWM制御する関数
*/
void motorDrive(int16_t pwmVal, const int m_l, const int m_r) {
  pwmVal = constrain(pwmVal, -1024, 1024);

  if (pwmVal >= 0) {
    analogWrite(m_l, pwmVal);
    digitalWrite(m_r, LOW);
  } else {
    pwmVal = abs(pwmVal);

    digitalWrite(m_l, LOW);
    analogWrite(m_r, pwmVal);
  }
}

/**
  stop
*/
void stopMotor() {
  digitalWrite(M1_l, LOW);
  digitalWrite(M1_r, LOW);
  digitalWrite(M2_l, LOW);
  digitalWrite(M2_r, LOW);
}

/**
  前進
*/
void goForward(int16_t velocity) {
  motorDrive(velocity, M1_l, M1_r);
  motorDrive(velocity, M2_l, M2_r);
}

/**
  後進
*/
void goBack(int16_t velocity) {
  motorDrive(-velocity, M1_l, M1_r);
  motorDrive(-velocity, M2_l, M2_r);
}

/**
  右旋回
*/
void turnRight(int16_t velocity) {
  motorDrive(velocity, M1_l, M1_r);
  motorDrive(-velocity, M2_l, M2_r);
}

/**
  右旋回
*/
void turnLeft(int16_t velocity) {
  motorDrive(-velocity, M1_l, M1_r);
  motorDrive(velocity, M2_l, M2_r);
}
