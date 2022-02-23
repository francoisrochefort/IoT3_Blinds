#ifndef IOT3_H
#define IOT3_H

/**
 * @file IoT3.h
 * @author Francois Rochefort (francoisrochefort@hotmail.fr)
 * @brief 
 * @version 0.1
 * @date 2022-02-20
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <Servo.h>

/**
 * Define the board to be used. If ESP12E is ommited then the
 * ESP01-1M is used. As of now, the definition is only used to
 * define GPIO pins according to choosen board
 */
#define ESP12E

/**
 * Define FORMAT_FIRMWARE if you wish to upload the firmware
 * that will format the repository to factory default. By
 * default, the formating firmware will mark the 'ok' flag
 * to false.
 */
//#define FORMAT_FIRMWARE

/**
 * Define WITH_DEFAULT in pair with FORMAT_FIRMWARE if you 
 * want the formating firmware to write default values to the
 * repository and avoid having to configure the repository 
 * through the soft access point at http://192.168.4.1
 */
//#define WITH_DEFAULT

#define DEF_SSID        "VIDEOTRON6590"
#define DEF_PASSWORD    "A9UAYAHH9947N"
#define DEF_NAME        "XXXrycXXX"
#define DEF_MQTT_SERVER "test.mosquitto.org"
#define DEF_MQTT_PORT   "1883"
#define DEF_APSSID      "TheMATRIX"
#define DEF_APPSK       "thereisnospoon"

#define MAX_SSID        32
#define MAX_PASSWORD    63
#define MAX_NAME        32
#define MAX_MQTT_SERVER 32
#define MAX_MQTT_PORT   32

class Repository;
class BlindsObserver;
class Firmware;
class SoftAccessPoint;
class BlindsStub;
class Blinds;

enum BlindsMode {
    BM_MANUAL = 1,
    BM_AUTOMATIC = 2
};

enum BlindsEvent {
    BE_OPEN = 1,
    BE_CLOSE = 2,
    BE_TIMEOUT = 3,
    BE_DAYTIME = 4,
    BE_NIGHTTIME = 5
};

enum BlindsState {
    BS_OPENING = 1,
    BS_OPENED = 2,
    BS_CLOSING = 3,
    BS_CLOSED = 4
};

/**
 * class is responsable to abstract the media storage used by 
 * the firmware to load and save persistant informations 
 */
class Repository {
    char ssid[MAX_SSID];
    char password[MAX_PASSWORD];
    char name[MAX_NAME];
    char mqttServer[MAX_MQTT_SERVER];
    char mqttPort[MAX_MQTT_PORT];
    bool ok;
public:
    int offset(void* field);
    Repository();
    bool load();
    String getSSID();
    String getPassword();
    String getName();
    String getMQTTServer();
    String getMQTTPort();
    void setSSID(const String& str);
    void setPassword(const String& str);
    void setName(const String& str);
    void setMQTTServer(const String& str);
    void setMQTTPort(const String& str);
    void save();
    String toString();
};

class BlindsObserver {
public:
    virtual void onSetMode() = 0;
    virtual void onOpening() = 0;
    virtual void onOpened() = 0;
    virtual void onClosing() = 0;
    virtual void onClosed() = 0;
};

class Firmware {
public:
    virtual void setup() = 0;
    virtual void loop() = 0;
};

class SoftAccessPoint : public Firmware {
    static void handleRoot();
    static void handleForm();
    static void handleNotFound();
    static ESP8266WebServer server;
public:
    SoftAccessPoint();
    virtual void setup();
    virtual void loop();
};

class BlindsStub : 
    public Firmware, 
    public BlindsObserver {
    static void callback(char* topic, byte* payload, unsigned int length);
    static String clientName();
    static String homeTopic(const String& topic);
    static String objectTopic(const String& topic);
    static void publish();
public:
    BlindsStub();
    virtual void setup();
    virtual void loop();
    virtual void onSetMode();
    virtual void onOpening();
    virtual void onOpened();
    virtual void onClosing();
    virtual void onClosed();
};

class Blinds : public Firmware {
    BlindsObserver& observer;
    BlindsMode mode;
    BlindsState state;
    unsigned long startTime;
    Servo servo;
public:
    Blinds(BlindsObserver& observer);
    void setState(BlindsEvent event);
    void setMode(BlindsMode mode);
    void open();
    void close();
    BlindsMode getMode();
    BlindsState getState();
    bool isNightTime();
    virtual void setup();
    virtual void loop();
};

#endif