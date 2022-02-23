
/**
 * @file Repository.cpp
 * @author Francois Rochefort (francoisrochefort@hotmail.fr)
 * @brief 
 * @version 0.1
 * @date 2022-02-19
 * 
 * @copyright Copyright (c) 2022
 * 
 */

#include <IoT3.h>
#include <EEPROM.h>

/**
 * @brief Computes the offset of an attribute into the EEPROM
 * 
 * @param field 
 * @return int 
 */
int Repository::offset(void* field) {
    return (byte*)field - (byte*)this;
}

/**
 * @brief Construct a new Repository:: Repository object
 */
Repository::Repository() {
}

bool Repository::load() {
    EEPROM.begin(sizeof(*this));
    EEPROM.get(offset(ssid), ssid);
    EEPROM.get(offset(password), password);
    EEPROM.get(offset(name), name);
    EEPROM.get(offset(mqttServer), mqttServer);
    EEPROM.get(offset(mqttPort), mqttPort);

#ifdef FEATURE_1
    EEPROM.get(offset(&state), state);
    EEPROM.get(offset(&mode), mode);
#endif

    EEPROM.get(offset(&ok), ok);
    return ok;
}

/**
 * @brief All getters
 */
String Repository::getSSID() {return String(ssid);}
String Repository::getPassword() {return String(password);}
String Repository::getName() {return String(name);}
String Repository::getMQTTServer() {return String(mqttServer);}
String Repository::getMQTTPort() {return String(mqttPort);}

#ifdef FEATURE_1
blindsState Repository::getState() {
    return state;
}    
blindsMode Repository::getMode() {
    return mode;
}
#endif

/**
 * @brief All setters
 */
void Repository::setSSID(const String& str) {str.toCharArray(ssid, str.length() + 1);}
void Repository::setPassword(const String& str) {str.toCharArray(password, str.length() + 1);}
void Repository::setName(const String& str) {str.toCharArray(name, str.length() + 1);}
void Repository::setMQTTServer(const String& str) {str.toCharArray(mqttServer, str.length() + 1);}
void Repository::setMQTTPort(const String& str) {str.toCharArray(mqttPort, str.length() + 1);}

#ifdef FEATURE_1
void Repository::setState(const blindsState state) {
    this->state = state;
}    
void Repository::setMode(const blindsMode mode) {
    this->mode = mode;
}
#endif

/**
 * @brief Saves all attributes into the EEPROM
 */
void Repository::save() {
    EEPROM.begin(sizeof(*this));
    EEPROM.put(offset(ssid), ssid);
    EEPROM.put(offset(password), password);
    EEPROM.put(offset(name), name);
    EEPROM.put(offset(mqttServer), mqttServer);
    EEPROM.put(offset(mqttPort), mqttPort);

#ifdef FEATURE_1
    EEPROM.put(offset(&state), state);
    EEPROM.put(offset(&mode), mode);
#endif

#ifdef FORMAT_FIRMWARE 
#ifdef WITH_DEFAULT
    EEPROM.put(offset(&ok), true);
#else
    EEPROM.put(offset(&ok), false);
#endif
#else
     EEPROM.put(offset(&ok), true);
#endif

    EEPROM.commit();
    EEPROM.end();
}

String Repository::toString() {
    String string = 
                "{\n";
    string += "\t'SSID': '" + getSSID() + "',\n";
    string += "\t'Password': '" + getPassword() + "',\n";
    string += "\t'Name': '" + getName() + "',\n";
    string += "\t'MQTT server': '" + getMQTTServer() + "',\n";
    string += "\t'MQTT port': '" + getMQTTPort() + "',\n";

#ifdef FEATURE_1
    string += "\t'Mode': '" + String(getMode()) + "',\n";
    string += "\t'State': '" + String(getState()) + "'\n";
#endif

    string += "}\n\n";
    return string;
}
