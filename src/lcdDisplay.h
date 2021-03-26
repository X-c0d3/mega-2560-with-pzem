#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H
#include <Arduino.h>
#include <ArduinoJson.h>
#include <UTFTGLUE.h>
// FOR 3.5 TFT LCD Shield

// LCD_RST  = A4
// LCD_CS   = A3
// LCD_RS   = A2
// LCD_WR   = A1
// LCD_RD   = A0

// LCD_D2   = D2 (GPIO 2) OC3B
// LCD_D3   = D3 (GPIO 3) OC3C
// LCD_D4   = D4 (GPIO 4) OC0B
// LCD_D5   = D5 (GPIO 5) OC3A
// LCD_D6   = D6 (GPIO 6) OC4A
// LCD_D7   = D7 (GPIO 7) OC4B

// LCD_D0   = D8 (GPIO 8) 0C4C
// LCD_D1   = D9 (GPIO 9) 0C2B

// - can use 5V (No need 3.3V)

UTFTGLUE myGLCD(0, A2, A1, A3, A4, A0);  //all dummy args
DynamicJsonBuffer jsonBuffer;
int rotation = 0;  //Vertical : 0, Holizontal : 3

void setupDisplay() {
    randomSeed(analogRead(0));

    myGLCD.reset();

    // Setup the LCD
    myGLCD.InitLCD();
    myGLCD.setRotation(rotation);

    myGLCD.setFont(SmallFont);

    myGLCD.clrScr();

    myGLCD.setTextSize(1.9);

    myGLCD.setColor(255, 0, 0);
    myGLCD.fillRect(0, 0, 479, 13);
    myGLCD.setColor(64, 64, 64);
    if (rotation == 0) {
        myGLCD.fillRect(0, 465, 479, 480);
    } else {
        myGLCD.fillRect(0, 306, 479, 319);
    }
    myGLCD.setColor(255, 255, 255);
    myGLCD.setBackColor(255, 0, 0);
    myGLCD.print("* Solar Power Monitoring *", CENTER, 1);
    myGLCD.setBackColor(64, 64, 64);
    myGLCD.setColor(255, 255, 0);

    if (rotation == 0) {
        myGLCD.print("http://www.rockdevper.com", CENTER, 468);
        myGLCD.setColor(0, 0, 255);
        myGLCD.drawRect(0, 14, 319, 479);
    } else {
        myGLCD.print("http://www.rockdevper.com", CENTER, 308);
        myGLCD.setColor(0, 0, 255);
        myGLCD.drawRect(0, 14, 479, 305);
    }
}

void switchButton(String title, bool isOn, int x1, int y1, int x2, int y2, int marginLeft) {
    myGLCD.setTextSize(2);
    if (isOn)
        myGLCD.setColor(0, 255, 0);
    else
        myGLCD.setColor(255, 0, 0);

    myGLCD.fillRoundRect(x1, y1, x2, y2);
    myGLCD.setColor(0, 0, 0);

    myGLCD.print(isOn ? "ON" : "OFF", isOn ? x1 + 8 : x1 + 4, y2 - 22);
    myGLCD.setColor(0, 255, 254);
    myGLCD.setFont(SmallFont);
    myGLCD.print(title, marginLeft, y2 + 5);
}

void displaySwitch(bool inverterState, bool coolingFanState, bool ledLightStage, bool spotLightState, bool powerBackupStage,
                   bool livingRoomLightStage, bool gadenLightStage, bool waterFallPumpStage, bool waterThePlantStage, bool waterSpinklerStage) {
    int start = 10;
    int y1 = 185;
    int x2 = 50;
    int y2 = y1 + 30;
    switchButton("Inverter", inverterState, start, y1, x2, y2, 8);
    switchButton("FAN", coolingFanState, start * 6, y1, x2 * 2, y2, 73);
    switchButton("LED LI.", ledLightStage, start * 11, y1, x2 * 3, y2, 113);
    switchButton("SP LI.", spotLightState, start * 16, y1, x2 * 4, y2, 165);
    switchButton("P.Backup", powerBackupStage, start * 21, y1, x2 * 5, y2, 210);

    switchButton("LRM LI.", livingRoomLightStage, start, y1 + 50, x2, y2 + 50, 10);
    switchButton("GRD LI.", gadenLightStage, start * 6, y1 + 50, x2 * 2, y2 + 50, 63);
    switchButton("WF Pump", waterFallPumpStage, start * 11, y1 + 50, x2 * 3, y2 + 50, 110);
    switchButton("WT PLNT", waterThePlantStage, start * 16, y1 + 50, x2 * 4, y2 + 50, 160);
    switchButton("WT SPIK", waterSpinklerStage, start * 21, y1 + 50, x2 * 5, y2 + 50, 213);
}

void showDeviceInfo(String ipAddress, String lastUpdate) {
    int x = 10;
    int y = 285;
    myGLCD.setFont(SmallFont);
    myGLCD.setColor(255, 0, 0);
    if (rotation == 0) {
    } else {
        myGLCD.print("IP: " + ipAddress, x, y);
        myGLCD.print("Last Update: " + lastUpdate, x, y + 10);
    }
}

void displayEnvironment(String temp, String uumidity, String soiMoisture) {
    int x = 10;
    int y = 120;
    myGLCD.setTextSize(2);
    myGLCD.setColor(0, 255, 0);
    myGLCD.print("Temp: " + temp + " C", x, y);
    myGLCD.print("Humidity: " + uumidity + "%", x, y + 20);
    myGLCD.print("Soi Moisture: " + soiMoisture, x, y + 40);
}

void displayPzemSensor(String dc_voltage_usage, String dc_current_usage, String dc_active_power,
                       String dc_active_energy, String dc_active_energy_raw, String ac_voltage_usage,
                       String ac_current_usage, String ac_active_power, String ac_active_energy,
                       String ac_frequency, String ac_pf

) {
    myGLCD.setColor(255, 255, 0);
    myGLCD.setBackColor(0, 0, 0);
    myGLCD.setTextSize(2);
    if (rotation == 0) {
        myGLCD.print(" (DC) ", 20, 20);
        myGLCD.print("Voltage: " + dc_voltage_usage + "V", 10, 40);
        myGLCD.print("Current: " + dc_current_usage + "A", 10, 60);
        myGLCD.print("Power: " + dc_active_power + "W", 10, 80);
        myGLCD.print("Energy: " + dc_active_energy + "KWh", 10, 100);

        myGLCD.setColor(255, 105, 180);
        myGLCD.print(" (AC) ", 20, 285);
        myGLCD.print("Voltage: " + ac_voltage_usage + "V", 10, 305);
        myGLCD.print("Current: " + ac_current_usage + "A", 10, 325);
        myGLCD.print("Power: " + ac_active_power + "W", 10, 345);
        myGLCD.print("Energy: " + ac_active_energy + "KWh", 10, 365);
        myGLCD.print("Freq: " + ac_frequency + "Hz", 10, 385);
        myGLCD.print("PF: " + ac_pf, 10, 405);

    } else {
        myGLCD.print(" (DC) ", 60, 20);
        myGLCD.print("Voltage: " + dc_voltage_usage + "V", 10, 40);
        myGLCD.print("Current: " + dc_current_usage + "A", 10, 60);
        myGLCD.print("Power: " + dc_active_power + "W", 10, 80);
        myGLCD.print("Energy: " + dc_active_energy + "KWh", 10, 100);

        myGLCD.setColor(255, 105, 180);
        myGLCD.print(" (AC) ", 315, 20);
        myGLCD.print("Voltage: " + ac_voltage_usage + "V", 260, 40);
        myGLCD.print("Current: " + ac_current_usage + "A", 260, 60);
        myGLCD.print("Power: " + ac_active_power + "W", 260, 80);
        myGLCD.print("Energy: " + ac_active_energy + "KWh", 260, 100);
        myGLCD.print("Freq: " + ac_frequency + "Hz", 260, 120);
        myGLCD.print("PF: " + ac_pf, 260, 140);
    }
}

void displayControlChargerInfo(String solarPanelVoltage, String solarPanelCurrent,
                               String solarPanelPower, String batteryCharging, String batteryCapacity, String loadStatus) {
    //SOLAR_CHARGER
    myGLCD.setTextSize(2);
    myGLCD.setColor(0, 255, 255);
    if (rotation == 0) {
        myGLCD.print(" (Solar Charger) ", 20, 425);
        myGLCD.print("PV:" + solarPanelVoltage + "V  C:" + solarPanelCurrent + "A BT:" + batteryCapacity + "%", 10, 445);
        // myGLCD.print("PV Curr.: " + solarPanelCurrent + "A", 260, 205);
        // myGLCD.print("PV Power: " + solarPanelPower + "W", 260, 225);
        // myGLCD.print("Charge Curr: " + batteryCharging + "A", 260, 245);
        // myGLCD.print("Bat Capacity: " + batteryCapacity + "%", 260, 265);
        // myGLCD.print("Load Status: " + loadStatus, 260, 285);

    } else {
        myGLCD.print(" (Solar Charger) ", 260, 165);
        myGLCD.print("PV Volt: " + solarPanelVoltage + "V", 260, 185);     //solarPanelVoltage
        myGLCD.print("PV Curr.: " + solarPanelCurrent + "A", 260, 205);    //solarPanelCurrent
        myGLCD.print("PV Power: " + solarPanelPower + "W", 260, 225);      //solarPanelPower
        myGLCD.print("Charge Curr: " + batteryCharging + "A", 260, 245);   //batteryCharging
        myGLCD.print("Bat Capacity: " + batteryCapacity + "%", 260, 265);  //batteryCapacity
        myGLCD.print("Load Status: " + loadStatus, 260, 285);              //loadStatus
    }
}

void refreshScreen() {
    //clear screen
    if (rotation == 0) {
        // myGLCD.setColor(0, 0, 0);
        // myGLCD.fillRect(1, 15, 318, 465);
        myGLCD.flush();
        myGLCD.clearWriteError();
    }
}

#endif