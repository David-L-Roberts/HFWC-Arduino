#include <Arduino.h>
#include "Sensors.h"
#include "ServoMotors.h"

// Function prototypes
void EMR_button_pressed();
void startupMsgSeq();
void processData(byte dataByte);
void autobreaking(float distance);
void testMotors();

// Message codes
#define HELLO   0xD1
#define ACK     0xD2
#define FORWARD 0xE1
#define BACKWARD 0xE2
#define LEFT    0xE3
#define RIGHT   0xE4
#define STOP    0xE0
#define DATA    0xFD
#define BREAK_EN    0xFB
#define BREAK_DIS   0xFC
#define STOP_EN     0xFA
#define STOP_DIS    0xFE

// defines pin numbers
#define PIN_ESTOP 2
#define PIN_ESTOP_LED 13

// distance for breaking (cm)
#define BREAK_DIST  50
// distance reading interval
#define DIST_READ_MILLI 3000

// Global variables
volatile long lastDebounce = 0; // last change
#define T_DEBOUNCE 50           // Debounce time (ms)

int buttonState = HIGH;         // Current  button
int lastButtonState = HIGH;     // Previous button

unsigned long tDistReading = 0; // record time for reading distance sensor

bool breakingActive = false;    // flag for automatic breaking 
volatile bool eStopActive = false;  // flag for emergency stop



/**************************************************************************/
void setup()
{
    pinMode(PIN_ESTOP_LED, OUTPUT);
    digitalWrite(PIN_ESTOP_LED, LOW);
    pinMode(PIN_ESTOP, INPUT_PULLUP);
    attachInterrupt(digitalPinToInterrupt(PIN_ESTOP), EMR_button_pressed, FALLING);

    begin_UDS();
    initServos();
    Serial.begin(9600); // open serial comms port
    testMotors();
    startupMsgSeq();
}

void loop()
{
    // motor control functions
    if (Serial.available() > 0) {
        // read a byte from the serial
        byte rxDataByte = Serial.read();
        // don't process byte if E-stop is enabled
        if (!eStopActive) {
            processData(rxDataByte);
        }
    }

    // only read dist sensor intermitently
    unsigned long tNow = millis();
    if (tNow - tDistReading > DIST_READ_MILLI) {
        float distance = read_UDS();
        autobreaking(distance);

        tDistReading = tNow;    // update last reading time
    }
}

/**************************************************************************/
void EMR_button_pressed()
{
    // debounce button check
    if ((millis() - lastDebounce) < T_DEBOUNCE) {
        return;
    }
    // toggle eStop Flag
    eStopActive = !eStopActive;

    if (eStopActive) {
        stopMotors();
        // send E-Stop enable notification
        Serial.write(STOP_EN);
        // Turn on LED
        digitalWrite(PIN_ESTOP_LED, HIGH);
    }
    else {
        // send E-Stop disable notification
        Serial.write(STOP_DIS);
        // Turn off LED
        digitalWrite(PIN_ESTOP_LED, LOW);
    }

    lastDebounce = millis();
}

/**************************************************************************/
void startupMsgSeq()
{
    byte rxData;
    // Wait for 'hello' from PC
    while (true)
    {
        // wait for data
        while (!Serial.available()){}
        rxData = Serial.read();

        // check if data is 'hello' code
        if (rxData == HELLO) {
            break;
        } else {
            continue;
        }
    }
    
    // Send ACK to PC
    Serial.write(ACK);
}

/**************************************************************************/
// Read and process an incoming byte from the PC
void processData(byte dataByte) 
{
    if ((dataByte == FORWARD) && (!breakingActive))
    {
        moveForwards();
    }
    else if (dataByte == BACKWARD)
    {
        moveBackwards();
    }
    else if (dataByte == LEFT)
    {
        moveLeft();
    }
    else if (dataByte == RIGHT)
    {
        moveRight();
    }
    else if (dataByte == STOP)
    {
        stopMotors();
    }
}

/**************************************************************************/
void autobreaking(float distance)
{
    int dataInt = static_cast<int>(distance/2);
    if (dataInt > 255) {
        dataInt = 255;
    }
    byte dataByte = dataInt;
    
    // write distance to serial
    Serial.write(DATA);
    Serial.write(dataByte);
    
    // check if auto breaking should be enabled
    if (distance <= BREAK_DIST) {
        if (!breakingActive) {
            stopMotors();
        }
        // set breaking flag
        breakingActive = true;
        // send activation notification
        Serial.write(BREAK_EN);

    } else if (breakingActive) {
        // deactivate breaking flag
        breakingActive = false;

        // send deactivation notification
        Serial.write(BREAK_DIS);
    }
}

/**************************************************************************/
/*!
    @brief  Test motor functions. Cycle through all movement options.
*/
/**************************************************************************/
void testMotors(){
    moveForwards();
    delay(1500);
    moveBackwards();
    delay(1500);
    moveLeft();
    delay(1500);
    moveRight();
    delay(1500);
    stopMotors();
    delay(1500);
}