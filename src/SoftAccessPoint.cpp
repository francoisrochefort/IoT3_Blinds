/**
 * @file SoftAccessPoint.cpp
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
 * Program variables
 */
ESP8266WebServer SoftAccessPoint::server(80);

void SoftAccessPoint::handleRoot() {
    const String form = "<!DOCTYPE html>\
    <html>\
    <head>\
    <style>\
    body {\
        font-family: 'Roboto', sans-serif;\
    }\
    .form-row {\
        display: flex;\
    }\
    .flex-container > div {\
        margin: 10px;\
        padding: 20px;\
    }\
    label {\
        width: 100%\
    }\
    article {\
        background-color: white;\
        width: 280px;\
        padding: 50px;\
        margin: auto;\
        box-shadow: 0px -2px 20px 2px rgba(0, 0, 0, 0.4);\
	}\
	.inputs {\
		border-radius: 7px;\
		padding: 5px;\
		margin: 2px;\
	}\
        </style>\
    </head>\
    <body>\
	<article>\
        <form method='post' enctype='application/x-www-form-urlencoded' action='/postform/'>\
            <h1>Object Repository</h1>\
            <h3>WiFi Credentials</h1>\
            <p>Please, enter the SSID and the password that will be used to connect to the Internet</p>\
            <div class='form-row'>\
                <label for='ssid'>SSID:</label>\
                <input class='inputs' type='text' id='ssid' name='ssid' value='VIDEOTRON6590'>\
            </div>\
            <div class='form-row'>\
                <label for='password'>Password:</label>\
                <input class='inputs' type='text' id='password' name='password' value='A9UAYAHH9947N'>\
            </div>\
            <h3>Blinds identification</h1>\
            <p>Enter a small description of the object such as <em>Kitchen</em> for example</p>\
            <div class='form-row'>\
                <label for='name'>Name:</label>\
                <input class='inputs' type='text' id='name' name='name' value='Kitchen'>\
            </div>\
            <p>Your router will also list this device using the name you have entered above</p>\
            <h3>MQTT message broker</h1>\
            <p>The information above should not be changed except for debugging purposes</p>\
            <div class='form-row'>\
                <label for='mqtt_server'>URI:</label>\
                <input class='inputs' type='text' id='mqtt_server' name='mqtt_server' value='test.mosquitto.org'>\
            </div>\
            <div class='form-row'>\
                <label for='mqtt_port'>Port:</label>\
                <input class='inputs' type='text' id='mqtt_port' name='mqtt_port' value='1883'>\
            </div>\
            <br/>\
            <input type='submit' value='Save to EEPROM'>\
        </form>\
	</article>\
    </body>\
    </html>";
    Serial.println(form);
    server.send(200, "text/html", form);
}

void SoftAccessPoint::handleForm() {
    if (server.method() != HTTP_POST) {

        /**
         * Only HTTP_POST is allowed
         */
        server.send(405, "text/plain", "Method Not Allowed");

    } else {

        /**
         * Save WiFi credentials
         */
        Repository repos;
        repos.setSSID(server.arg("ssid"));
        repos.setPassword(server.arg("password"));
        repos.setName(server.arg("name"));
        repos.setMQTTServer(server.arg("mqtt_server"));
        repos.setMQTTPort(server.arg("mqtt_port"));
        repos.save();

        /**
         * Send success message to user
         */
        String resp = "Object configuration successfully saved. Please, reset to continue\n";
        resp += repos.toString();
        server.send(200, "text/plain", resp);
    }
}

void SoftAccessPoint::handleNotFound() {
    String resp = "File Not Found\n\n";
    resp += "URI: ";
    resp += server.uri();
    resp += "\nMethod: ";
    resp += (server.method() == HTTP_GET) ? "GET" : "POST";
    resp += "\nArguments: ";
    resp += server.args();
    resp += "\n";
    for (uint8_t i = 0; i < server.args(); i++) {
        resp += " " + server.argName(i) + ": " + server.arg(i) + "\n";
    }
    server.send(404, "text/plain", resp);
}

SoftAccessPoint::SoftAccessPoint() {}

void SoftAccessPoint::setup() {

    /**
     * Initialize serial communication baud rate
     */
    Serial.begin(9600);

    /**
     * Init. Soft access point
     */
    WiFi.softAP(DEF_APSSID, DEF_APPSK);
    IPAddress myIP = WiFi.softAPIP();
    server.on("/", handleRoot);
    server.on("/postform/", handleForm);
    server.onNotFound(handleNotFound);
    server.begin();
    Serial.println("Soft access point started");
}

void SoftAccessPoint::loop() {
    server.handleClient();
}
