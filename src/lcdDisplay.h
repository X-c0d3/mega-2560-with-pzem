#ifndef LCD_DISPLAY_H
#define LCD_DISPLAY_H
#include <Arduino.h>
#include <ArduinoJson.h>
#include <UTFTGLUE.h>
// FOR 3.5 TFT LCD Shield ILI9486 480X320

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
int rotation = 3;

#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0
#define WHITE 0xFFFF
#define olive 0x7BE0  //Vertical : 0, Holizontal : 3

void setupDisplay() {
    randomSeed(analogRead(0));

    myGLCD.reset();

    // Setup the LCD
    myGLCD.InitLCD();
    myGLCD.setRotation(rotation);

    myGLCD.setFont(SmallFont);

    myGLCD.clrScr();

    myGLCD.setTextSize(1.9);

    // myGLCD.setColor(255, 0, 0);
    // myGLCD.fillRect(0, 0, 479, 13);
    // myGLCD.setColor(64, 64, 64);
    // if (rotation == 0) {
    //     myGLCD.fillRect(0, 465, 479, 480);
    // } else {
    //     //myGLCD.fillRect(0, 306, 479, 319);
    // }
    // myGLCD.setColor(255, 255, 255);
    // myGLCD.setBackColor(255, 0, 0);
    // myGLCD.print("* Solar Power Monitoring *", CENTER, 1);
    // myGLCD.setBackColor(64, 64, 64);
    // myGLCD.setColor(255, 255, 0);

    // if (rotation == 0) {
    //     //myGLCD.print("http://www.rockdevper.com", CENTER, 468);
    //     myGLCD.setColor(0, 0, 255);
    //     myGLCD.drawRect(0, 14, 319, 479);
    // } else {
    //     // myGLCD.print("http://www.rockdevper.com", CENTER, 308);
    //     myGLCD.setColor(0, 0, 255);
    //     myGLCD.drawRect(0, 14, 479, 319);
    // }
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
    int y1 = 220;
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

String RemoveLastDigit(String input) {
    return input.substring(0, input.length() - 1);
}

void displayPzemSensor(String dc_voltage_usage, String dc_current_usage, String dc_active_power,
                       String dc_active_energy, String dc_active_energy_raw,
                       String inv_ac_voltage_usage,
                       String inv_ac_current_usage,
                       String inv_ac_active_power,
                       String inv_ac_active_energy,
                       String inv_ac_frequency,
                       String inv_ac_pf,
                       String main_ac_voltage_usage,
                       String main_ac_current_usage,
                       String main_ac_active_power,
                       String main_ac_active_energy,
                       String main_ac_frequency,
                       String main_ac_pf

) {
    myGLCD.setColor(255, 255, 0);
    myGLCD.setBackColor(0, 0, 0);
    myGLCD.setTextSize(2);
    if (rotation == 0) {
        myGLCD.print(" (High Volt Inverter) ", 20, 20);
        myGLCD.print("PV Volt: " + dc_voltage_usage + "V", 10, 40);
        myGLCD.print("PV Current: " + dc_current_usage + "A", 10, 60);
        myGLCD.print("PV Power: " + dc_active_power + "W", 10, 80);
        myGLCD.print("Energy: " + dc_active_energy + "KWh", 10, 100);

        myGLCD.setColor(255, 105, 180);
        myGLCD.print(" (Main AC) ", 20, 285);
        myGLCD.print("Voltage: " + main_ac_voltage_usage + "V", 10, 305);
        myGLCD.print("Current: " + main_ac_current_usage + "A", 10, 325);
        myGLCD.print("Power: " + main_ac_active_power + "W", 10, 345);
        myGLCD.print("Energy: " + main_ac_active_energy + "KWh", 10, 365);
        myGLCD.print("Freq: " + main_ac_frequency + "Hz", 10, 385);
        myGLCD.print("PF: " + main_ac_pf, 10, 405);

    } else {
        myGLCD.print(" (Hi-Volt Inverter) ", 10, 1);
        myGLCD.print("PV Volt: " + dc_voltage_usage + "V", 10, 20);
        myGLCD.print("PV Current: " + dc_current_usage + "A", 10, 40);
        myGLCD.print("PV Power: " + RemoveLastDigit(dc_active_power) + "W", 10, 60);
        //Inverter
        myGLCD.setColor(255, 105, 180);
        myGLCD.print("INV Volt: " + inv_ac_voltage_usage + "V", 10, 80);
        myGLCD.print("INV Curr: " + inv_ac_current_usage + "A", 10, 100);
        myGLCD.print("INV Power: " + inv_ac_active_power + "W", 10, 120);
        myGLCD.print("INV Energy: " + RemoveLastDigit(inv_ac_active_energy) + "KWh", 10, 140);

        //Main AC Power
        myGLCD.setColor(255, 105, 180);
        myGLCD.print(" (Main AC) ", 290, 1);
        myGLCD.print("Voltage: " + main_ac_voltage_usage + "V", 260, 20);
        myGLCD.print("Current: " + main_ac_current_usage + "A", 260, 40);
        myGLCD.print("Power: " + main_ac_active_power + "W", 260, 60);
        myGLCD.print("Energy: " + RemoveLastDigit(main_ac_active_energy) + "KWh", 260, 80);
        myGLCD.print("PF: " + main_ac_pf + " FQ " + main_ac_frequency + "Hz", 260, 100);
    }
}

void displayEnvironment(String temp, String humidity, String soiMoisture) {
    int x = 10;
    int y = 160;
    myGLCD.setTextSize(2);
    myGLCD.setColor(0, 255, 0);
    myGLCD.print("Temp: " + temp + "C", x, y);
    myGLCD.print("Humidity: " + humidity + "%", x, y + 20);
    myGLCD.print("Soi Moisture: " + soiMoisture, x, y + 40);
}

void displayControlChargerInfo(String solarPanelVoltage, String solarPanelCurrent,
                               String solarPanelPower, String batteryVoltage, String batteryCharging, String batteryCapacity, String loadStatus, String deviceStatus) {
    //SOLAR_CHARGER
    myGLCD.setTextSize(2);
    myGLCD.setColor(0, 255, 255);
    if (rotation == 0) {
        myGLCD.print(" (MPPT Solar Charger) ", 20, 425);
        myGLCD.print("PV:" + solarPanelVoltage + "V  C:" + solarPanelCurrent + "A BT:" + batteryCapacity + "%", 10, 445);
        // myGLCD.print("PV Curr.: " + solarPanelCurrent + "A", 260, 205);
        // myGLCD.print("PV Power: " + solarPanelPower + "W", 260, 225);
        // myGLCD.print("Charge Curr: " + batteryCharging + "A", 260, 245);
        // myGLCD.print("Bat Capacity: " + batteryCapacity + "%", 260, 265);
        // myGLCD.print("Load Status: " + loadStatus, 260, 285);

    } else {
        myGLCD.print(" (MPPT Charger) ", 270, 120);
        myGLCD.print("PV Volt: " + solarPanelVoltage + "V", 260, 140);     //solarPanelVoltage
        myGLCD.print("PV Curr.: " + solarPanelCurrent + "A", 260, 160);    //solarPanelCurrent
        myGLCD.print("PV Power: " + solarPanelPower + "W", 260, 180);      //solarPanelPower
        myGLCD.print("Charge Curr: " + batteryCharging + "A", 260, 200);   //batteryCharging
        myGLCD.print("Bat Volt: " + batteryVoltage + "V", 260, 220);       //batteryVoltage
        myGLCD.print("Bat Capacity: " + batteryCapacity + "%", 260, 240);  //batteryCapacity
        myGLCD.print("Load Status: " + loadStatus, 260, 260);              //loadStatus
        myGLCD.setColor(MAGENTA);
        myGLCD.print(deviceStatus, 260, 280);  //deviceStatus
    }
}

void showDeviceInfo(String ipAddress, String lastUpdate) {
    int x = 260;
    int y = 300;
    myGLCD.setFont(SmallFont);
    myGLCD.setColor(0, 255, 0);
    if (rotation == 0) {
    } else {
        myGLCD.print("IP:" + ipAddress + "          " + lastUpdate, x, y + 8);
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

void ClearDisplay() {
    myGLCD.clrScr();
}

#endif