/**
 * @file Blinds.cpp
 * @author Francois Rochefort (francoisrochefort@hotmail.fr)
 * @brief 
 * @version 0.1
 * @date 2022-02-20
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <IoT3.h>

/**
 * GPIO pin definitions
 */
#ifdef ESP12E
#define SERVO_PIN     15
#define PHOTOCELL_PIN A0
#else
#define SERVO_PIN      2
#define PHOTOCELL_PIN A0
#endif

/**
 * Servo spin directions
 */
#define SPIN_FORWARD    0
#define SPIN_REVERSE  150
#define SPIN_DELAY   2000

/**
 * @brief Construct a new Blinds:: Blinds object
 * 
 * @param observer 
 */
Blinds::Blinds(BlindsObserver& observer) :
    observer(observer), mode(BM_MANUAL), state(BS_CLOSED) {}

/**
 * @brief Set the state of the blinds according to a given event
 * 
 * @param event 
 */
void Blinds::setState(BlindsEvent event) {
    switch (event) {
        case BE_OPEN:
            if (mode == BM_MANUAL && state == BS_CLOSED) {

                /** 
                 * User is opening the blinds from the cellphone app
                 */
                servo.attach(SERVO_PIN);
                servo.write(SPIN_REVERSE);
                startTime = millis();
                state = BS_OPENING;
                observer.onOpening();
            }
            break;
        case BE_CLOSE:
            if (mode == BM_MANUAL && state == BS_OPENED) {

                /** 
                 * User is closing the blinds from the cellphone app
                 */
                servo.attach(SERVO_PIN);
                servo.write(SPIN_FORWARD);
                startTime = millis();
                state = BS_CLOSING;
                observer.onClosing();
            }
            break;
        case BE_TIMEOUT:
            if (state == BS_OPENING) {

                /** 
                 * Blinds are completely opened
                 */
                servo.detach();
                state = BS_OPENED;
                observer.onOpened();

            } else if (state == BS_CLOSING) {

                /** 
                 * Blinds are completely closed
                 */
                servo.detach();
                state = BS_CLOSED;
                observer.onClosed();
            }
            break;
        case BE_DAYTIME:
            if (mode == BM_AUTOMATIC && state == BS_CLOSED) {

                /**
                 * It is day time; start opening the blinds
                 */
                servo.attach(SERVO_PIN);
                servo.write(SPIN_REVERSE);
                startTime = millis();
                state = BS_OPENING;
                observer.onOpening();
            }
            break;
        case BE_NIGHTTIME:
            if (mode == BM_AUTOMATIC && state == BS_OPENED) {

                /**
                 * It is night time; start opening the blinds
                 */
                servo.attach(SERVO_PIN);
                servo.write(SPIN_FORWARD);
                startTime = millis();
                state = BS_CLOSING;
                observer.onClosing();
            }
            break;
    }
}

/**
 * @brief Set the mode of operation of the blinds
 * 
 * @param mode BM_AUTOMATIC
 *             BM_MANUAL
 */
void Blinds::setMode(BlindsMode mode) {
    this->mode = mode;
    observer.onSetMode();
}

/**
 * @brief Start opening the blinds 
 */
void Blinds::open(){setState(BE_OPEN);}

/**
 * @brief Start closing the blinds
 */
void Blinds::close(){setState(BE_CLOSE);}

/**
 * @brief Get the mode of operation of the blinds
 * 
 * @return BM_AUTOMATIC
 *         BM_MANUAL 
 */
BlindsMode Blinds::getMode() {return mode;}

/**
 * @brief Determines the state of the blinds 
 * 
 * @return  BS_OPENING
 *          BS_OPENED
 *          BS_CLOSING
 *          BS_CLOSED
 */
BlindsState Blinds::getState(){return state;}

/**
 * @brief Determines whether or not it is the night by performing 
 *        an analog reading of the photocell pin
 * 
 * @return true 
 * @return false 
 */
bool Blinds::isNightTime(){
    return analogRead(PHOTOCELL_PIN) < 150;
}

/**
 * @brief Initialise the firmware
 */
void Blinds::setup() {

    /**
     * Initialize serial communication baud rate
     */
    Serial.begin(9600);

    /**
     * Initialize GPIO pins
     */
    pinMode(SERVO_PIN, OUTPUT);
    pinMode(PHOTOCELL_PIN, INPUT);
}

/**
 * @brief Main loop
 */
void Blinds::loop() {

    /**
     * Update the state of the blinds according to time
     */
    if (millis() - startTime > SPIN_DELAY) setState(BE_TIMEOUT);

    /**
     * Update the state of the blinds according to the light
     */
    if (isNightTime()) {
        setState(BE_NIGHTTIME);
    }
    else {
        setState(BE_DAYTIME);
    }

    /**
     * Required for MQTT
     */
    delay(50);
}
