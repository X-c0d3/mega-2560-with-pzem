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
Pzem solarDCmeter;
Pzem powerBackup;
Pzem mainPower;
Pzem solarInverter;

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

String deviceName;

void displayData() {
    displayPzemSensor(
        solarDCmeter,
        solarInverter,
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
    // Send Data
    static uint32_t last_send = millis();
    if (millis() - last_send > 1000) {
        last_send = millis();

        digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    }

    if (comReady) {
        displayData();
        comReady = false;
    }
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
                    solarDCmeter.voltage = atoi(Words[6]);
                    // solarDCmeter.current = atoi(Words[7]);
                    // solarDCmeter.power = atoi(Words[8]);
                    solarDCmeter.energy = atoi(Words[9]);
                    solarDCmeter.enegy_kw_day = atoi(Words[10]);

                    // AC
                    solarInverter.voltage = atoi(Words[11]);
                    solarInverter.current = atoi(Words[12]);
                    solarInverter.power = atoi(Words[13]);
                    solarInverter.energy = atoi(Words[14]);
                    solarInverter.frequency = atoi(Words[15]);
                    solarInverter.pf = atoi(Words[16]);

                    inverterState = String(Words[17]) == "ON";
                    coolingFanState = String(Words[18]) == "ON";
                    ledLightStage = (loadStatus == "ON") ? true : String(Words[19]) == "ON";
                    spotLightState = String(Words[20]) == "ON";
                    powerBackupStage = String(Words[21]) == "ON";

                } else if (deviceName == "BootMppt") {
                    // DC
                    // dc_voltage_usage = Words[4];
                    solarDCmeter.current = atoi(Words[5]);
                    solarDCmeter.power = atoi(Words[6]);
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