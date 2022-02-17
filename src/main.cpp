
#include <IoT3.h>

#ifndef FORMAT_FIRMWARE

/** Blinds firmware */
SoftAccessPoint softAccessPoint;
BlindsStub blindsStub;
Blinds blinds(blindsStub);
Firmware* firmware = nullptr;
void setup() {
    Repository repos;
    firmware = repos.load() ? (Firmware*)&blindsStub : (Firmware*)&softAccessPoint;
    firmware->setup();
}
void loop() {
    firmware->loop();
}
#else

/** Formating firmware */
void setup() {
    Serial.begin(9600);
    Serial.println("Erasing repository...");

    Repository repos;
    repos.setSSID(DEF_SSID);
    repos.setPassword(DEF_PASSWORD);
    repos.setName(DEF_NAME);
    repos.setMQTTServer(DEF_MQTT_SERVER);
    repos.setMQTTPort(DEF_MQTT_PORT);
    repos.save();

    Serial.println("Repository erased.");
    Serial.print("You may want to remove FORMAT_FIRMWARE from 'IoT3.h' and restart the upload process");
}
void loop() {
}
#endif
