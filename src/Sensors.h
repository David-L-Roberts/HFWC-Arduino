#include <Arduino.h>

#pragma once

void begin_UDS();
float read_UDS();

// Distance sensor pins
#define PIN_TRIG 11
#define PIN_ECHO 10


// call me in setup()
/**************************************************************************/
/*!
    @brief  Connect the ultrasonic distance sensor
    @return Void
*/
/**************************************************************************/
void begin_UDS() {
    pinMode(PIN_TRIG, OUTPUT);
    pinMode(PIN_ECHO,  INPUT);
}


// Call me in loop()
/**************************************************************************/
/*!
    @brief  retrieve information from the ultrasonic distance sensor
    @return float describing the distance read by sensor (in cm)
*/
/**************************************************************************/
float read_UDS() {
    //start new measurement
    digitalWrite(PIN_TRIG, HIGH);
    delayMicroseconds(10);
    digitalWrite(PIN_TRIG, LOW);

    //read result
    float duration = pulseIn(PIN_ECHO, HIGH);
    float distance = duration / 58; //distance in cm

    return distance;
}
