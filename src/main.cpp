#include <Arduino.h>
#include <ArduinoJson.h>
#include <HardwareSerial.h>
#include <PZEM004Tv30.h>
#include <PZEM017.h>
#include <SoftwareSerial.h>

// For Mega 2560 Pro Hardware Serial
// Hardware Serial0  =  1: RX0,   0: TX0
// Hardware Serial1  = 19: RX1,  18: TX1
// Hardware Serial2  = 17: RX2,  16: TX2
// Hardware Serial3  = 15: RX3,  14: TX3

PZEM017 pzem017(&Serial1);
PZEM004Tv30 pzem04t(&Serial2);

#define DEFAULT_BAUD_RATE 115200
#define HIGHT_VOLTAGE 15.10
#define LOW_VOLTAGE 10.50

void setup() {
    Serial.begin(DEFAULT_BAUD_RATE);

    Serial3.begin(57600);

    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
    delay(2000);

    pzem017.setCurrentShunt(0x0001);
    pzem017.setHIVoltageAlarm(HIGHT_VOLTAGE);
    pzem017.setLOWVoltageAlarm(LOW_VOLTAGE);

    pzem017.resetEnergy();
}

void loop() {
    // PZEM-017
    float DC_VOLTAGE = pzem017.voltage();
    float DC_CURRENT = pzem017.current();
    float DC_POWER = pzem017.power() * 10;
    float DC_ENERGY = pzem017.energy() * 10;

    float AC_VOLTAGE = pzem04t.voltage();
    float AC_CURRENT = pzem04t.current();
    float AC_POWER = pzem04t.power();
    float AC_ENERGY = pzem04t.energy();
    float AC_FREQUENCY = pzem04t.frequency();
    float AC_PF = pzem04t.pf();

    // if (!isnan(DC_VOLTAGE)) {
    //     Serial.println("########### PZEM-017 ###############");
    //     Serial.println("PZEM-017 V: " + String(DC_VOLTAGE));
    //     Serial.println("PZEM-017 A: " + String(DC_CURRENT));
    //     Serial.println("PZEM-017 W:" + String(DC_POWER));
    //     Serial.println("PZEM-017 E: " + String(DC_ENERGY));
    // }

    // if (!isnan(AC_VOLTAGE)) {
    //     Serial.println("########### PZEM-014 ###############");
    //     Serial.println("PZEM-004T VOLTAGE: " + String(AC_VOLTAGE));
    //     Serial.println("PZEM-004T CURRENT: " + String(AC_CURRENT));
    //     Serial.println("PZEM-004T POWER:" + String(AC_POWER));
    //     Serial.println("PZEM-004T ENERGY: " + String(AC_ENERGY));
    //     Serial.println("PZEM-004T FREQUENCY: " + String(AC_FREQUENCY));
    //     Serial.println("PZEM-004T PF: " + String(AC_PF));
    // }

    //Recived Data
    while (Serial3.available() > 0) {
        char inByte = Serial3.read();
        Serial.write(inByte);
    }

    //Send Data
    static uint32_t last_send = millis();
    if (millis() - last_send > 1000) {
        last_send = millis();

        DynamicJsonBuffer jsonBuffer;
        JsonObject &root = jsonBuffer.createObject();
        root["deviceName"] = "Mega 2560 Pro (Mini)";

        JsonObject &data = root.createNestedObject("sensor");
        // DHT11
        data["humidity"] = 20;
        data["temperature"] = 30;

        JsonObject &dc = data.createNestedObject("dc");
        // PZEM-017
        dc["voltage_usage"] = DC_VOLTAGE;
        dc["current_usage"] = DC_CURRENT;
        dc["active_power"] = DC_POWER;
        dc["active_energy"] = DC_ENERGY;

        JsonObject &ac = data.createNestedObject("ac");
        // PZEM-004T
        ac["voltage_usage"] = AC_VOLTAGE;
        ac["current_usage"] = AC_CURRENT;
        ac["active_power"] = AC_POWER;
        ac["active_energy"] = AC_ENERGY;
        ac["frequency"] = AC_FREQUENCY;
        ac["pf"] = AC_PF;

        JsonObject &deviceState = root.createNestedObject("deviceState");
        deviceState["LED1"] = "ON";
        deviceState["LED2"] = "OFF";

        String output;
        root.prettyPrintTo(output);

        Serial3.write(output.c_str(), output.length());
        Serial.print("Send to arduino - " + output);

        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }
}