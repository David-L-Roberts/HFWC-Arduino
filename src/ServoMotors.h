#include <Arduino.h>
#include <Servo.h>

#pragma once

Servo servoLeft;
Servo servoRight;

#define PIN_SERVO_RIGHT 5
#define PIN_SERVO_LEFT  6

// directions for left motor
#define L_FWRD  0 
#define L_BCK   178   // prevent jitter by not going to max
#define L_STOP  90

// directions for right motor
#define R_FWRD  178
#define R_BCK   0
#define R_STOP  90


// Initialise servo motors. Call me in `setup()`
void initServos()
{
    servoLeft.attach(PIN_SERVO_LEFT);
    servoRight.attach(PIN_SERVO_RIGHT);
}

void moveForwards()
{
    servoLeft.write(L_FWRD);
    servoRight.write(R_FWRD);
}

void moveBackwards()
{
    servoLeft.write(L_BCK);
    servoRight.write(R_BCK);
}

void moveLeft()
{
    servoLeft.write(L_BCK);
    servoRight.write(R_FWRD);
}

void moveRight()
{
    servoLeft.write(L_FWRD);
    servoRight.write(R_BCK);
}

void stopMotors()
{
    servoLeft.write(L_STOP);
    servoRight.write(R_STOP);
}
