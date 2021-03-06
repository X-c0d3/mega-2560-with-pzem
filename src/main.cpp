#include <Arduino.h>
#include <ArduinoJson.h>
#include <DHTesp.h>
#include <HardwareSerial.h>
#include <PZEM004Tv30.h>
#include <PZEM017.h>
#include <SoftwareSerial.h>
#include <UTFTGLUE.h>

#include "config.h"
#include "lcdDisplay.h"

// For Mega 2560 Pro Hardware Serial
// Hardware Serial0  =  1: RX0,   0: TX0
// Hardware Serial1  = 19: RX1,  18: TX1
// Hardware Serial2  = 17: RX2,  16: TX2
// Hardware Serial3  = 15: RX3,  14: TX3

// https://community.platformio.org/t/how-to-increase-the-arduino-serial-buffer-size/122

PZEM017 pzem017(&Serial1);
PZEM004Tv30 pzem04t(&Serial2);
DHTesp dht;

//WireSerial (RX/TX) - Cable
SoftwareSerial wireSerial(11, 10);  // RX/TX Define hardware

void setup() {
    Serial.begin(DEFAULT_BAUD_RATE);
    wireSerial.begin(9600);
    Serial3.begin(9600);

    dht.setup(DHTpin, DHTesp::DHT11);  //for DHT11 Connect DHT sensor to GPIO 15

    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
    delay(2000);

    pzem017.setCurrentShunt(0x0001);
    // pzem017.setHIVoltageAlarm(HIGHT_VOLTAGE);
    // pzem017.setLOWVoltageAlarm(LOW_VOLTAGE);

    pzem017.resetEnergy();
    setupDisplay();
}

int i = 0;
void loop() {
    //Recived Data
    //static String jsonResponse = "";
    // while (wireSerial.available() > 0) {
    //     String jsonResponse = wireSerial.readString();
    //     //char inChar = wireSerial.read();
    //     //jsonResponse += inChar;
    //     //if (inChar == '\r') {

    //     Serial.println(".");
    //     DynamicJsonBuffer jsonBuffer;
    //     JsonObject &root = jsonBuffer.parseObject(jsonResponse);
    //     //Verify Json Format
    //     if (!root.success()) {
    //         Serial.println("Failed to parse json");
    //         Serial.println(jsonResponse);
    //         Serial.println("--------------------");
    //         jsonResponse = "";
    //         return;
    //     }

    //     if (root["command"]["action"] == "ENERGY_RESET") {
    //         pzem017.resetEnergy();
    //         Serial.println("Reset Energy");
    //     }
    //     String deviceName = root["deviceName"];
    //     String lastUpdate = root["time"];
    //     Serial.println(deviceName + " LastUpdate:" + lastUpdate);
    //     if (deviceName == "SOLAR_CHARGER") {
    //         displayControlChargerInfo(root);
    //     }

    //     //  Cleanup
    //     // jsonResponse = "";
    //     //}
    // }

    //Send Data
    static uint32_t last_send = millis();
    if (millis() - last_send > 2000) {
        last_send = millis();

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

        // DHT11
        float HUMIDITY = !isnan(dht.getHumidity()) ? dht.getHumidity() : 0;
        float TEMPERATURE = !isnan(dht.getTemperature()) ? dht.getTemperature() : 0;

        //Random Data
        if (ENABLE_TEST_MODE) {
            DC_VOLTAGE = random(11.2, 15.4);
            DC_CURRENT = random(2.1, 7.6);
            DC_POWER = random(90.5, 200.3);
            DC_ENERGY = random(400.4, 600.9);

            AC_VOLTAGE = random(200, 250);
            AC_CURRENT = random(8, 15);
            AC_POWER = random(300, 400);
            AC_ENERGY = random(300, 500);
            AC_FREQUENCY = random(49, 54);
            AC_PF = random(20, 30);

            HUMIDITY = random(50, 100);
            TEMPERATURE = random(20, 50);
        }

        if (ENABLE_DEBUG_MODE) {
            if (!isnan(DC_VOLTAGE)) {
                Serial.println("########### PZEM-017 ###############");
                Serial.println("PZEM-017 V: " + String(DC_VOLTAGE));
                Serial.println("PZEM-017 A: " + String(DC_CURRENT));
                Serial.println("PZEM-017 W:" + String(DC_POWER));
                Serial.println("PZEM-017 E: " + String(DC_ENERGY));
            }

            if (!isnan(AC_VOLTAGE)) {
                Serial.println("########### PZEM-014 ###############");
                Serial.println("PZEM-004T VOLTAGE: " + String(AC_VOLTAGE));
                Serial.println("PZEM-004T CURRENT: " + String(AC_CURRENT));
                Serial.println("PZEM-004T POWER:" + String(AC_POWER));
                Serial.println("PZEM-004T ENERGY: " + String(AC_ENERGY));
                Serial.println("PZEM-004T FREQUENCY: " + String(AC_FREQUENCY));
                Serial.println("PZEM-004T PF: " + String(AC_PF));
            }
        }

        DynamicJsonBuffer jsonBuffer;
        JsonObject &root = jsonBuffer.createObject();
        root["deviceName"] = DEVICE_NAME;

        JsonObject &data = root.createNestedObject("sensor");
        // DHT11
        data["humidity"] = HUMIDITY;
        data["temperature"] = TEMPERATURE;

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
        //Send data to Serial
        Serial3.write(output.c_str(), output.length());

        // if (ENABLE_DEBUG_MODE)
        //     Serial.print(output);

        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }

    i++;

    //(start, แนวตั้ง,ไปทางช้าย, ลงล่าง)

    // delay(2000);
    // myGLCD.setTextSize(1.9);
    // myGLCD.fillScr(0, 0, 255);
    // myGLCD.setColor(255, 0, 0);
    // myGLCD.fillRoundRect(160, 70, 319, 169);

    // myGLCD.setColor(255, 255, 255);
    // myGLCD.setBackColor(255, 0, 0);
    // myGLCD.print("That's it!", CENTER, 93);
    // myGLCD.print("Restarting in a", CENTER, 119);
    // myGLCD.print("few seconds...", CENTER, 132);

    // myGLCD.setColor(0, 255, 0);
    // myGLCD.setBackColor(0, 0, 255);
    // myGLCD.print("Runtime: (msecs)", CENTER, 290);
    // myGLCD.printNumI(millis(), CENTER, 305);
}

#define MAX_WORLD_COUNT 5
#define MIN_WORLD_COUNT 2
char *Words[MAX_WORLD_COUNT];

byte split_message(char *str) {
    byte word_count = 0;             //number of words
    char *item = strtok(str, " ,");  //getting first word (uses space & comma as delimeter)

    while (item != NULL) {
        if (word_count >= MAX_WORLD_COUNT) {
            break;
        }
        Words[word_count] = item;
        item = strtok(NULL, " ,");  //getting subsequence word
        word_count++;
    }
    return word_count;
}

void print_message(byte word_count) {
    //if (word_count >= MIN_WORLD_COUNT) {
    //Serial.print("Word count : ");  Serial.println(word_count);
    for (byte sms_block = 0; sms_block < word_count; sms_block++) {
        Serial.print("Word ");
        Serial.print(sms_block + 1);
        Serial.print(" : ");
        Serial.println(Words[sms_block]);
    }
    Serial.println("--------------------");
    //}
}

int xx = 0;

String temp, humidity, soiMoisture;
String ipAddress, lastUpdate;
bool inverterState, coolingFanState, ledLightStage, spotLightState,
    powerBackupStage, livingRoomLightStage, gadenLightStage,
    waterFallPumpStage, waterThePlantStage, waterSpinklerStage;

String solarPanelVoltage, solarPanelCurrent, solarPanelPower, batteryCharging, batteryCapacity, loadStatus;

String dc_voltage_usage, dc_current_usage, dc_active_power,
    dc_active_energy, dc_active_energy_raw, ac_voltage_usage,
    ac_current_usage, ac_active_power, ac_active_energy,
    ac_frequency, ac_pf;

void serialEvent3() {
    byte word_count;

    while (Serial3.available() > 0) {
        //char inChar = (char)Serial3.read();  // get the new byte:
        String comStr = Serial3.readStringUntil('\n');

        comStr.trim();
        DynamicJsonBuffer jsonBuffer;
        JsonObject &root = jsonBuffer.parseObject(comStr);

        if (!root.success()) {
            Serial.println("Failed to parse json");
            Serial.println(comStr);
            Serial.println("--------------------");
        } else {
            // Serial.println("DATA :" + comStr);
            // word_count = split_message(comStr.c_str());
            // if (word_count >= MIN_WORLD_COUNT) {
            //     Serial.println("DATA :" + String(xx));
            //     print_message(word_count);
            // }
            // xx++;

            // String deviceName = Words[0];

            //Verify Json Format

            if (root["command"]["action"] == "ENERGY_RESET") {
                pzem017.resetEnergy();
                Serial.println("Reset Energy");
            }
            String deviceName = root["deviceName"];
            String lastUpdate = root["time"];
            Serial.println(deviceName + " LastUpdate:" + lastUpdate);

            ipAddress = root["ipAddress"].as<String>();
            lastUpdate = root["time"].as<String>();

            if (deviceName == "SOLAR_CHARGER") {
                solarPanelVoltage = root["sensor"]["solarPanelVoltage"].as<String>();
                solarPanelCurrent = root["sensor"]["solarPanelCurrent"].as<String>();
                solarPanelPower = root["sensor"]["solarPanelPower"].as<String>();
                batteryCharging = root["sensor"]["batteryCharging"].as<String>();
                batteryCapacity = root["sensor"]["batteryCapacity"].as<String>();
                loadStatus = root["sensor"]["loadStatus"].as<String>();
            } else if (deviceName == "SolarPower") {
                dc_voltage_usage = root["sensor"]["dc"]["voltage_usage"].as<String>();
                dc_current_usage = root["sensor"]["dc"]["current_usage"].as<String>();
                dc_active_power = root["sensor"]["dc"]["active_power"].as<String>();
                dc_active_energy = root["sensor"]["dc"]["active_energy"].as<String>();
                dc_active_energy_raw = root["sensor"]["dc"]["active_energy_raw"].as<String>();
                ac_voltage_usage = root["sensor"]["ac"]["voltage_usage"].as<String>();
                ac_current_usage = root["sensor"]["ac"]["current_usage"].as<String>();
                ac_active_power = root["sensor"]["ac"]["active_power"].as<String>();
                ac_active_energy = root["sensor"]["ac"]["active_energy"].as<String>();
                ac_frequency = root["sensor"]["ac"]["frequency"].as<String>();
                ac_pf = root["sensor"]["ac"]["pf"].as<String>();

                temp = root["sensor"]["temperature"].as<String>();
                humidity = root["sensor"]["humidity"].as<String>();

                inverterState = root["sensor"]["INVERTER"].as<bool>();
                coolingFanState = root["sensor"]["COOLING_FAN"].as<bool>();
                ledLightStage = root["sensor"]["LIGHT"].as<bool>();
                spotLightState = root["sensor"]["SPOTLIGHT"].as<bool>();
                powerBackupStage = root["sensor"]["POWER_BACKUP"].as<bool>();

            } else if (deviceName == "FarmBot") {
                soiMoisture = root["sensor"]["soilMoistureRaw"].as<String>();
                waterThePlantStage = root["sensor"]["WATER_THE_PLANTS"].as<bool>();
                waterSpinklerStage = root["sensor"]["WATER_SPRINKLER"].as<bool>();
            } else if (deviceName == "FarmBot") {
                //livingRoomLightStage = root["sensor"]["INVERTER"].as<bool>();
                livingRoomLightStage = false;
                gadenLightStage = root["sensor"]["GARDEN_LIGHT"].as<bool>();
                waterFallPumpStage = root["sensor"]["WATER_FALL_PUMP"].as<bool>();
            }

            displayPzemSensor(dc_voltage_usage, dc_current_usage, dc_active_power,
                              dc_active_energy, dc_active_energy_raw, ac_voltage_usage,
                              ac_current_usage, ac_active_power, ac_active_energy,
                              ac_frequency, ac_pf);
            displayControlChargerInfo(solarPanelVoltage, solarPanelCurrent, solarPanelPower, batteryCharging, batteryCapacity, loadStatus);
            displayEnvironment(temp, humidity, soiMoisture);
            displaySwitch(inverterState, coolingFanState, ledLightStage, spotLightState, powerBackupStage,
                          livingRoomLightStage, gadenLightStage, waterFallPumpStage,
                          waterThePlantStage, waterSpinklerStage);
            showDeviceInfo(ipAddress, lastUpdate);
        }
    }
}