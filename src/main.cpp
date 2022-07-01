#include <Arduino.h>
#include <ArduinoJson.h>
#include <DHTesp.h>
#include <HardwareSerial.h>
#include <PZEM004Tv30.h>
#include <PZEM017.h>
#include <SoftwareSerial.h>
#include <UTFTGLUE.h>

#include "Models/Pzem.h"
#include "config.h"
#include "lcdDisplay.h"
// For Mega 2560 Pro Hardware Serial
// Hardware Serial0  =  1: RX0,   0: TX0
// Hardware Serial1  = 19: RX1,  18: TX1
// Hardware Serial2  = 17: RX2,  16: TX2
// Hardware Serial3  = 15: RX3,  14: TX3

#define USING_16BIT_BUS
// https://community.platformio.org/t/how-to-increase-the-arduino-serial-buffer-size/122

// PZEM017 pzem017(&Serial1);
// PZEM004Tv30 pzem04t(&Serial2);
DHTesp dht;
Pzem powerBackup;
Pzem mainPower;

// WireSerial (RX/TX) - Cable
// SoftwareSerial wireSerial(11, 10);  // RX/TX Define hardware

void setup() {
    Serial.begin(DEFAULT_BAUD_RATE);
    //  wireSerial.begin(9600);
    Serial3.begin(76800);

    dht.setup(DHTpin, DHTesp::DHT11);  // for DHT11 Connect DHT sensor to GPIO 15

    // initialize digital pin LED_BUILTIN as an output.
    pinMode(LED_BUILTIN, OUTPUT);
    delay(2000);

    // pzem017.setCurrentShunt(0x0001);
    //  pzem017.setHIVoltageAlarm(HIGHT_VOLTAGE);
    //  pzem017.setLOWVoltageAlarm(LOW_VOLTAGE);

    // pzem017.resetEnergy();
    setupDisplay();
}

#define MAX_WORLD_COUNT 140
#define MIN_WORLD_COUNT 5
char *Words[MAX_WORLD_COUNT];

byte split_message(char *str) {
    byte word_count = 0;            // number of words
    char *item = strtok(str, "|");  // getting first word (uses space & comma as delimeter)

    while (item != NULL) {
        if (word_count >= MAX_WORLD_COUNT) {
            break;
        }
        Words[word_count] = item;
        item = strtok(NULL, "|");  // getting subsequence word
        word_count++;
    }
    return word_count;
}

const byte numNrfChars = 1000;
char nrfBuffer[numNrfChars];  // an array to store the received data
boolean comReady = false;

String temp, humidity, soiMoisture;
String ipAddress, lastUpdate;
bool inverterState, coolingFanState, ledLightStage, spotLightState,
    powerBackupStage, livingRoomLightStage, gadenLightStage,
    waterFallPumpStage, waterThePlantStage, waterSpinklerStage;

String solarPanelVoltage, solarPanelCurrent, solarPanelPower, batteryVoltage,
    batteryCharging, batteryCapacity, loadStatus, deviceStatus, loadCurrent, loadPower, chargerTemp, chargeAmount;

String dc_voltage_usage, dc_current_usage, dc_active_power,
    dc_active_energy, dc_active_energy_raw,
    inv_ac_voltage_usage,
    inv_ac_current_usage,
    inv_ac_active_power,
    inv_ac_active_energy,
    inv_ac_frequency,
    inv_ac_pf;

String deviceName;

void displayData() {
    displayPzemSensor(dc_voltage_usage, dc_current_usage, dc_active_power,
                      dc_active_energy, dc_active_energy_raw,
                      inv_ac_voltage_usage,
                      inv_ac_current_usage,
                      inv_ac_active_power,
                      inv_ac_active_energy,
                      inv_ac_frequency,
                      inv_ac_pf,
                      mainPower,
                      powerBackup);
    displayControlChargerInfo(solarPanelVoltage, solarPanelCurrent, solarPanelPower, batteryVoltage, batteryCharging, batteryCapacity, loadStatus, deviceStatus, loadCurrent, loadPower, chargerTemp, chargeAmount);
    displayEnvironment(temp, humidity, soiMoisture);
    displaySwitch(inverterState, coolingFanState, ledLightStage, spotLightState, powerBackupStage,
                  livingRoomLightStage, gadenLightStage, waterFallPumpStage,
                  waterThePlantStage, waterSpinklerStage);
    showDeviceInfo(ipAddress, lastUpdate);

    refreshScreen();
}

void loop() {
    // Recived Data
    // static String jsonResponse = "";
    //  while (wireSerial.available() > 0) {
    //      String jsonResponse = wireSerial.readString();
    //      //char inChar = wireSerial.read();
    //      //jsonResponse += inChar;
    //      //if (inChar == '\r') {

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

    // Send Data
    static uint32_t last_send = millis();
    if (millis() - last_send > 1000) {
        last_send = millis();

        // // PZEM-017
        // float DC_VOLTAGE = pzem017.voltage();
        // float DC_CURRENT = pzem017.current();
        // float DC_POWER = pzem017.power() * 10;
        // float DC_ENERGY = pzem017.energy() * 10;

        // float AC_VOLTAGE = pzem04t.voltage();
        // float AC_CURRENT = pzem04t.current();
        // float AC_POWER = pzem04t.power();
        // float AC_ENERGY = pzem04t.energy();
        // float AC_FREQUENCY = pzem04t.frequency();
        // float AC_PF = pzem04t.pf();

        // // DHT11
        // float HUMIDITY = !isnan(dht.getHumidity()) ? dht.getHumidity() : 0;
        // float TEMPERATURE = !isnan(dht.getTemperature()) ? dht.getTemperature() : 0;

        // //Random Data
        // if (ENABLE_TEST_MODE) {
        //     DC_VOLTAGE = random(11.2, 15.4);
        //     DC_CURRENT = random(2.1, 7.6);
        //     DC_POWER = random(90.5, 200.3);
        //     DC_ENERGY = random(400.4, 600.9);

        //     AC_VOLTAGE = random(200, 250);
        //     AC_CURRENT = random(8, 15);
        //     AC_POWER = random(300, 400);
        //     AC_ENERGY = random(300, 500);
        //     AC_FREQUENCY = random(49, 54);
        //     AC_PF = random(20, 30);

        //     HUMIDITY = random(50, 100);
        //     TEMPERATURE = random(20, 50);
        // }

        // if (ENABLE_DEBUG_MODE) {
        //     if (!isnan(DC_VOLTAGE)) {
        //         Serial.println("########### PZEM-017 ###############");
        //         Serial.println("PZEM-017 V: " + String(DC_VOLTAGE));
        //         Serial.println("PZEM-017 A: " + String(DC_CURRENT));
        //         Serial.println("PZEM-017 W:" + String(DC_POWER));
        //         Serial.println("PZEM-017 E: " + String(DC_ENERGY));
        //     }

        //     if (!isnan(AC_VOLTAGE)) {
        //         Serial.println("########### PZEM-014 ###############");
        //         Serial.println("PZEM-004T VOLTAGE: " + String(AC_VOLTAGE));
        //         Serial.println("PZEM-004T CURRENT: " + String(AC_CURRENT));
        //         Serial.println("PZEM-004T POWER:" + String(AC_POWER));
        //         Serial.println("PZEM-004T ENERGY: " + String(AC_ENERGY));
        //         Serial.println("PZEM-004T FREQUENCY: " + String(AC_FREQUENCY));
        //         Serial.println("PZEM-004T PF: " + String(AC_PF));
        //     }
        // }

        // DynamicJsonDocument root(1024);
        // root["deviceName"] = DEVICE_NAME;

        // JsonObject data = root.createNestedObject("sensor");
        // // DHT11
        // data["humidity"] = HUMIDITY;
        // data["temperature"] = TEMPERATURE;

        // JsonObject dc = data.createNestedObject("dc");
        // // PZEM-017
        // dc["voltage_usage"] = DC_VOLTAGE;
        // dc["current_usage"] = DC_CURRENT;
        // dc["active_power"] = DC_POWER;
        // dc["active_energy"] = DC_ENERGY;

        // JsonObject ac = data.createNestedObject("ac");
        // // PZEM-004T
        // ac["voltage_usage"] = AC_VOLTAGE;
        // ac["current_usage"] = AC_CURRENT;
        // ac["active_power"] = AC_POWER;
        // ac["active_energy"] = AC_ENERGY;
        // ac["frequency"] = AC_FREQUENCY;
        // ac["pf"] = AC_PF;

        // JsonObject deviceState = root.createNestedObject("deviceState");
        // deviceState["LED1"] = "ON";
        // deviceState["LED2"] = "OFF";

        // String output;
        // serializeJson(root, output);
        // Serial3.write(output.c_str(), output.length());

        // if (ENABLE_DEBUG_MODE)
        //     Serial.print(output);

        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }

    if (comReady) {
        displayData();
        comReady = false;
    }

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

int digits(int x) {
    return ((bool)x * (int)log10(abs(x)) + 1);
}

char endMarker = '\n';
void serialEvent3() {
    static byte ndx = 0;
    char rc;
    while (Serial3.available() > 0) {
        rc = Serial3.read();
        if (rc != endMarker) {
            nrfBuffer[ndx] = rc;
            ndx++;
            if (ndx >= numNrfChars) {
                ndx = numNrfChars - 1;
            }
        } else {
            nrfBuffer[ndx] = '\0';  // terminate the string
            ndx = 0;

            int sizeData = strlen(nrfBuffer) - 1;
            Serial.println(nrfBuffer);
            byte word_count = split_message(nrfBuffer);
            int ckSum = String(Words[0]).toInt();

            deviceName = Words[1];
            ipAddress = Words[2];
            lastUpdate = Words[3];

            if (deviceName == "Clear") {
                ClearDisplay();
            }

            if (word_count > MIN_WORLD_COUNT && ckSum == (sizeData - digits(ckSum))) {
                if (deviceName == "SolarPower") {
                    humidity = Words[4];
                    temp = Words[5];

                    // DC
                    dc_voltage_usage = Words[6];
                    // dc_current_usage = Words[7];
                    // dc_active_power = Words[8];
                    dc_active_energy = Words[9];
                    dc_active_energy_raw = Words[10];

                    // AC
                    inv_ac_voltage_usage = Words[11];
                    inv_ac_current_usage = Words[12];
                    inv_ac_active_power = Words[13];
                    inv_ac_active_energy = Words[14];
                    inv_ac_frequency = Words[15];
                    inv_ac_pf = Words[16];

                    inverterState = String(Words[17]) == "ON";
                    coolingFanState = String(Words[18]) == "ON";
                    ledLightStage = (loadStatus == "ON") ? true : String(Words[19]) == "ON";
                    spotLightState = String(Words[20]) == "ON";
                    powerBackupStage = String(Words[21]) == "ON";

                } else if (deviceName == "BootMppt") {
                    // DC
                    // dc_voltage_usage = Words[4];
                    dc_current_usage = Words[5];
                    dc_active_power = Words[6];
                    // temp= Words[7];
                    // vout= Words[8];
                    // fanshow= Words[9];
                    // booting= Words[10];
                    // message= Words[11];

                } else if (deviceName == "PowerBackup") {
                    // Pzem 004tv3
                    powerBackup.voltage = atoi(Words[4]);
                    powerBackup.current = atoi(Words[5]);
                    powerBackup.power = atoi(Words[6]);
                    powerBackup.energy = atoi(Words[7]);
                    powerBackup.frequency = atoi(Words[8]);
                    powerBackup.pf = atoi(Words[9]);
                    // Solar Charger
                    solarPanelVoltage = Words[10];
                    solarPanelCurrent = Words[11];
                    solarPanelPower = Words[12];
                    batteryCharging = Words[13];
                    batteryCapacity = Words[14];
                    loadStatus = String(Words[15]);
                    batteryVoltage = Words[16];
                    chargerTemp = Words[17];
                    deviceStatus = Words[18];
                    ledLightStage = (loadStatus == "ON");
                    loadCurrent = Words[19];
                    loadPower = Words[20];
                    chargeAmount = Words[21];
                } else if (deviceName == "MainPower") {
                    mainPower.voltage = atoi(Words[4]);
                    mainPower.current = atoi(Words[5]);
                    mainPower.power = atoi(Words[6]);
                    mainPower.energy = atoi(Words[7]);
                    mainPower.frequency = atoi(Words[8]);
                    mainPower.pf = atoi(Words[9]);
                } else if (deviceName == "HomeControl") {
                    gadenLightStage = String(Words[4]) == "ON";
                    livingRoomLightStage = String(Words[5]) == "ON";
                } else if (deviceName == "FarmBot") {
                    soiMoisture = String(Words[4]);
                    // soiMoisture_Raw = Words[5];
                    waterThePlantStage = String(Words[6]) == "ON";
                    waterSpinklerStage = String(Words[7]) == "ON";
                } else if (deviceName == "SmartGarden") {
                    waterFallPumpStage = String(Words[4]) == "ON";
                }

            } else {
                Serial.println(" ***** Check Sum Error ***** => Check Sum:" + String(ckSum) + " Ori Size:" + String(sizeData) + " digits:" + digits(ckSum) + " word_count:" + word_count);
            }

            comReady = true;
        }
    }
}