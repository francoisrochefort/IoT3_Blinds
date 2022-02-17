#include <IoT3.h>

#define SPIN_DELAY   2000

/**
 * GPIO pin definitions
 */
#ifdef ESP12E
#define SERVO_PIN               15
#define PHOTOCELL_PIN           A0 //10
#else
#define SERVO_PIN      LED_BUILTIN
#define PHOTOCELL_PIN           A0
#endif

/**
 * Servo spin directions
 */
#define SPIN_FORWARD    0
#define SPIN_REVERSE  150

/**
 * @brief Construct a new Blinds:: Blinds object
 * 
 * @param observer 
 */
Blinds::Blinds(BlindsObserver& observer) : observer(observer), mode(manualMode), state(closedState) {}

/**
 * @brief Set the state of the blinds according to a given event
 * 
 * @param event 
 */
void Blinds::setState(blindsEvent event) {
    switch (event) {
        case openEvent:
            if (mode == manualMode && state == closedState) {

                /** 
                 * User is opening the blinds from the cellphone app
                 */
                servo.attach(SERVO_PIN);
                servo.write(SPIN_REVERSE);
                startTime = millis();
                state = openingState;
                observer.onOpening();
            }
            break;
        case closeEvent:
            if (mode == manualMode && state == openedState) {

                /** 
                 * User is closing the blinds from the cellphone app
                 */
                servo.attach(SERVO_PIN);
                servo.write(SPIN_FORWARD);
                startTime = millis();
                state = closingState;
                observer.onClosing();
            }
            break;
        case timeoutEvent:
            if (state == openingState) {

                /** 
                 * Blinds are completely opened
                 */
                servo.detach();
                state = openedState;
                observer.onOpened();

            } else if (state == closingState) {

                /** 
                 * Blinds are completely closed
                 */
                servo.detach();
                state = closedState;
                observer.onClosed();
            }
            break;
        case dayTimeEvent:
            if (mode == automaticMode && state == closedState) {

                /**
                 * It is day time; start opening the blinds
                 */
                servo.attach(SERVO_PIN);
                servo.write(SPIN_REVERSE);
                startTime = millis();
                state = openingState;
                observer.onOpening();
            }
            break;
        case nightTimeEvent:
            if (mode == automaticMode && state == openedState) {

                /**
                 * It is night time; start opening the blinds
                 */
                servo.attach(SERVO_PIN);
                servo.write(SPIN_FORWARD);
                startTime = millis();
                state = closingState;
                observer.onClosing();
            }
            break;
    }
}

/**
 * @brief Set the mode of operation of the blinds
 * 
 * @param mode automaticMode
 *             manualMode
 */
void Blinds::setMode(blindsMode mode) {
    this->mode = mode;
    observer.onSetMode();
}

/**
 * @brief Start opening the blinds 
 */
void Blinds::open(){setState(openEvent);}

/**
 * @brief Start closing the blinds
 */
void Blinds::close(){setState(closeEvent);}

/**
 * @brief Get the mode of operation of the blinds
 * 
 * @return automaticMode
 *         manualMode 
 */
blindsMode Blinds::getMode() {return mode;}

/**
 * @brief Determines the state of the blinds 
 * 
 * @return  openingState
 *          openedState
 *          closingState
 *          closedState
 */
blindsState Blinds::getState(){return state;}

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
     *  Initialize GPIO pins
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
    if (millis() - startTime > SPIN_DELAY) setState(timeoutEvent);

    /**
     * Update the state of the blinds according to the light
     */
    if (isNightTime()) {
        setState(nightTimeEvent);
    }
    else {
        setState(dayTimeEvent);
    }

    /**
     * Required for MQTT
     */
    delay(10);
}
