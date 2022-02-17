
#include <IoT3.h>
#include <EEPROM.h>

int Repository::offset(void* field) {
    return (byte*)field - (byte*)this;
}

Repository::Repository() {
}

bool Repository::load() {
    EEPROM.begin(sizeof(*this));
    EEPROM.get(offset(ssid), ssid);
    EEPROM.get(offset(password), password);
    EEPROM.get(offset(name), name);
    EEPROM.get(offset(mqttServer), mqttServer);
    EEPROM.get(offset(mqttPort), mqttPort);
    EEPROM.get(offset(&ok), ok);
    return ok;
}

String Repository::getSSID() {
    return String(ssid);
}

String Repository::getPassword() {
    return String(password);
}

String Repository::getName() {
    return String(name);
}

String Repository::getMQTTServer() {
     return String(mqttServer);
}
String Repository::getMQTTPort() {
     return String(mqttPort);
}

void Repository::setSSID(const String& str) {
    str.toCharArray(ssid, str.length() + 1);
}

void Repository::setPassword(const String& str) {
    str.toCharArray(password, str.length() + 1);
}
void Repository::setName(const String& str) {
    str.toCharArray(name, str.length() + 1);
}

void Repository::setMQTTServer(const String& str) {
    str.toCharArray(mqttServer, str.length() + 1);
}

void Repository::setMQTTPort(const String& str) {
    str.toCharArray(mqttPort, str.length() + 1);
}

#ifdef FORMAT_FIRMWARE 
#ifndef WITH_DEFAULT
void Repository::save() {
    EEPROM.begin(sizeof(*this));
    EEPROM.put(offset(ssid), ssid);
    EEPROM.put(offset(password), password);
    EEPROM.put(offset(name), name);
    EEPROM.put(offset(mqttServer), mqttServer);
    EEPROM.put(offset(mqttPort), mqttPort);
    EEPROM.put(offset(&ok), false);
    EEPROM.commit();
    EEPROM.end();
}
#else
void Repository::save() {
    EEPROM.begin(sizeof(*this));
    EEPROM.put(offset(ssid), ssid);
    EEPROM.put(offset(password), password);
    EEPROM.put(offset(name), name);
    EEPROM.put(offset(mqttServer), mqttServer);
    EEPROM.put(offset(mqttPort), mqttPort);
    EEPROM.put(offset(&ok), true);
    EEPROM.commit();
    EEPROM.end();
}
#endif
#else
void Repository::save() {
    EEPROM.begin(sizeof(*this));
    EEPROM.put(offset(ssid), ssid);
    EEPROM.put(offset(password), password);
    EEPROM.put(offset(name), name);
    EEPROM.put(offset(mqttServer), mqttServer);
    EEPROM.put(offset(mqttPort), mqttPort);
    EEPROM.put(offset(&ok), true);
    EEPROM.commit();
    EEPROM.end();
}
#endif

String Repository::toString() {
    String string = 
                "{\n";
    string += "\t'SSID': '" + getSSID() + "',\n";
    string += "\t'Password': '" + getPassword() + "'\n";
    string += "\t'Name': '" + getName() + "'\n";
    string += "\t'MQTT server': '" + getMQTTServer() + "'\n";
    string += "\t'MQTT port': '" + getMQTTPort() + "'\n";
    string += "}\n\n";
    return string;
}
