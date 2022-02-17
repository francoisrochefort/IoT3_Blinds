
#include <IoT3.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>

/** Topics */
#define TOPIC_COMMANDS "/IOT3/COMMANDS"
#define TOPIC_STATES   "/IOT3/STATES"

/** Commands */
#define CMD_OPEN          "open"
#define CMD_CLOSE         "close"
#define CMD_SET_MODE      "set_mode"
#define CMD_QUERY_OBJECTS "query_objects"

/** Debug message */
#define STATE_DEVICE_READY "READY"

/** States */
#define STATE_OPENING      "opening"
#define STATE_OPENED       "opened"
#define STATE_CLOSING      "closing"
#define STATE_CLOSED       "closed"

/** Modes */
#define MODE_MANUAL        "manual"
#define MODE_AUTOMATIC     "automatic"

/** Limits */
#define MAX_PAYLOAD 256
#define MAX_MAC     6

/** Program variables */
WiFiClient wifiClient;
PubSubClient client(wifiClient);
extern Blinds blinds;

/**
 * MQTT callback function implementation
 */
void BlindsStub::callback(char* topic, byte* payload, unsigned int length) 
{
    StaticJsonDocument<MAX_PAYLOAD> doc;
    deserializeJson(doc, payload, length);
    String cmd = doc["cmd"];
    if (String(topic) == objectTopic(TOPIC_COMMANDS)) {
        if (cmd == CMD_OPEN) {
            blinds.open();
        } else if (cmd == CMD_CLOSE) {
            blinds.close();
        } else if (cmd == CMD_SET_MODE) {
            blinds.setMode(doc["mode"]);
        }
    } else if (String(topic) == homeTopic(TOPIC_COMMANDS)) {
        if (cmd == CMD_QUERY_OBJECTS) {
            publish();
        }
    }
}

String BlindsStub::clientName() {
    String name = WiFi.localIP().toString();
    return name;
}

String BlindsStub::homeTopic(const String& topic) {
    String string = topic;
    return string;
}

String BlindsStub::objectTopic(const String& topic) {
    String string = topic;
    string += "/";
    string += WiFi.localIP().toString();
    return string;
}

void BlindsStub::publish() {
    Repository repos; 
    repos.load();
    StaticJsonDocument<MAX_PAYLOAD> doc;
    doc["ip"] = WiFi.localIP();
    doc["name"] = repos.getName();
    JsonObject object = doc.createNestedObject("objects");
    switch (blinds.getState()){
        case openingState:
            object["state"] = STATE_OPENING;
            break;
        case openedState:
            object["state"] = STATE_OPENED;
            break;
        case closingState:
            object["state"] = STATE_CLOSING;
            break;
        case closedState:
            object["state"] = STATE_CLOSED;
            break;
    }
    switch (blinds.getMode()) {
        case manualMode:
            object["mode"] = MODE_MANUAL;
            break;
        case automaticMode:
            object["mode"] = MODE_AUTOMATIC;
            break;
    }
    String json;
    serializeJsonPretty(doc, json);
    client.publish(homeTopic(TOPIC_STATES).c_str(), json.c_str());
}

BlindsStub::BlindsStub() {}

void BlindsStub::setup() {
    
    Serial.begin(9600);

    /** Load required information from repository */
    Repository repos;
    repos.load();
    String hostname = repos.getName();
    String domain = (repos.getMQTTServer().c_str());
    String port = (repos.getMQTTPort().c_str());

    /** Init. WiFi client */
    Serial.print("Connecting to WiFi");
    WiFi.mode(WIFI_STA);
    WiFi.hostname(hostname);
    WiFi.begin(repos.getSSID(), repos.getPassword());
    while (WiFi.status() != WL_CONNECTED) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("");
    Serial.print("Connected to WiFi: ");
    Serial.print(repos.getSSID());
    Serial.print(" using password: ");
    Serial.println(repos.getPassword());

    /** Init. MQTT client */
    Serial.print("Connecting to MQTT");
    client.setServer(domain.c_str(), port.toInt());
    client.setCallback(callback);
    while (!client.connect(clientName().c_str())) {
        Serial.print(".");
        delay(500);
    }
    Serial.println("");
    Serial.print("Connected to MQTT: ");
    Serial.print(repos.getMQTTServer());
    Serial.print(":");
    Serial.println(repos.getMQTTPort());

    /**  Subscribe to topics */
    client.subscribe(homeTopic(TOPIC_COMMANDS).c_str());
    client.subscribe(objectTopic(TOPIC_COMMANDS).c_str());

    /** Send debug signal 'READY' to mosquitto_sub */
    client.publish(homeTopic(TOPIC_STATES).c_str(), STATE_DEVICE_READY);

    /** Init. the blinds */
    blinds.setup();
}

void BlindsStub::loop() {
    client.loop();
    blinds.loop();
}

void BlindsStub::onSetMode(){publish();}
void BlindsStub::onOpening(){publish();}
void BlindsStub::onOpened(){publish();}
void BlindsStub::onClosing(){publish();}
void BlindsStub::onClosed(){publish();}
