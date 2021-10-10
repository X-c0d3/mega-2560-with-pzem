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
    int y1 = 220;
    int y2 = y1 + 30;
    switchButton("Inverter", inverterState, 1, y1, 40, y2, 0);
    switchButton("FAN", coolingFanState, 50, y1, 90, y2, 62);
    switchButton("LED LI.", ledLightStage, 100, y1, 140, y2, 102);
    switchButton("SP LI.", spotLightState, 150, y1, 190, y2, 155);
    switchButton("P.Backup", powerBackupStage, 200, y1, 240, y2, 200);

    switchButton("LRM LI.", livingRoomLightStage, 1, y1 + 50, 40, y2 + 50, 2);
    switchButton("GRD LI.", gadenLightStage, 50, y1 + 50, 90, y2 + 50, 53);
    switchButton("WF Pump", waterFallPumpStage, 100, y1 + 50, 140, y2 + 50, 100);
    switchButton("WT PLNT", waterThePlantStage, 150, y1 + 50, 190, y2 + 50, 150);
    switchButton("WT SPIK", waterSpinklerStage, 200, y1 + 50, 240, y2 + 50, 200);
}

// String RemoveLastDigit(String input) {
//     return input.substring(0, input.length() - 1);
// }

int marginLeft = 8;
int marginLeftSeconds = 250;
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
                       String main_ac_pf,
                       String backup_ac_voltage,
                       String backup_ac_power,
                       String backup_ac_current,
                       String backup_ac_energy,
                       String backup_ac_frequency,
                       String backup_ac_pf) {
    myGLCD.setColor(255, 255, 0);
    myGLCD.setBackColor(0, 0, 0);
    myGLCD.setTextSize(2);

    if (rotation == 0) {
        myGLCD.print(" (High Volt Inverter) ", 20, 20);
        myGLCD.print("PV Volt: " + dc_voltage_usage + "V", marginLeft, 40);
        myGLCD.print("PV Current: " + dc_current_usage + "A", marginLeft, 60);
        myGLCD.print("PV Power: " + dc_active_power + "W", marginLeft, 80);
        myGLCD.print("Energy: " + dc_active_energy + "KWh", marginLeft, 100);

        myGLCD.setColor(255, 105, 180);
        myGLCD.print(" (PEA Main AC) ", 20, 280);
        myGLCD.print("Voltage: " + main_ac_voltage_usage + "V", 10, 305);
        myGLCD.print("Current: " + main_ac_current_usage + "A", 10, 325);
        myGLCD.print("Power: " + main_ac_active_power + "W", 10, 345);
        myGLCD.print("Energy: " + main_ac_active_energy + "KWh", 10, 365);
        myGLCD.print("Freq: " + main_ac_frequency + "Hz", 10, 385);
        myGLCD.print("PF: " + main_ac_pf, 10, 405);

    } else {
        if (inv_ac_voltage_usage != "0") {
            myGLCD.print(" (Hi-Volt Inverter) ", marginLeft + 4, 1);
            myGLCD.print("INV Volt: " + inv_ac_voltage_usage + " V     ", marginLeft, 20);
            myGLCD.print("INV Curr: " + inv_ac_current_usage + " A    ", marginLeft, 40);
            myGLCD.print("INV Power: " + inv_ac_active_power + " W  ", marginLeft, 60);
            myGLCD.print("INV Energy: " + inv_ac_active_energy + " KWh  ", marginLeft, 80);
            myGLCD.print("INV PF: " + inv_ac_pf + " FQ:" + inv_ac_frequency + "Hz ", marginLeft, 100);
        }

        myGLCD.setColor(255, 105, 180);
        if (dc_voltage_usage != "0") {
            myGLCD.print("PV Volt: " + dc_voltage_usage + " V      ", marginLeft, 120);
            myGLCD.print("PV Current: " + dc_current_usage + " A  ", marginLeft, 140);
            myGLCD.print("PV Power: " + dc_active_power + " W   ", marginLeft, 160);
        }

        if (dc_voltage_usage == "0" && inv_ac_voltage_usage == "0") {
            myGLCD.setColor(255, 255, 0);
            myGLCD.print(" (BackUp) ", marginLeft + 25, 1);
            myGLCD.print("Voltage: " + backup_ac_voltage + " V     ", marginLeft, 20);
            myGLCD.print("Current: " + backup_ac_current + " A    ", marginLeft, 40);
            myGLCD.print("Power: " + backup_ac_power + " W   ", marginLeft, 60);
            myGLCD.print("Energy: " + backup_ac_energy + " KWh  ", marginLeft, 80);
            myGLCD.print("PF: " + backup_ac_pf + " FQC: " + backup_ac_frequency + " Hz ", marginLeft, 100);
        }

        //Main AC Power
        myGLCD.setColor(255, 105, 180);
        myGLCD.print(" (PEA Main AC) ", 270, 1);
        myGLCD.print("Voltage: " + main_ac_voltage_usage + " V  ", marginLeftSeconds, 20);
        myGLCD.print("Current: " + main_ac_current_usage + " A  ", marginLeftSeconds, 40);
        myGLCD.print("Power: " + main_ac_active_power + " W  ", marginLeftSeconds, 60);
        myGLCD.print("Energy: " + main_ac_active_energy + " KWh    ", marginLeftSeconds, 80);
        myGLCD.print("PF: " + main_ac_pf + " FQC: " + main_ac_frequency + " Hz ", marginLeftSeconds, 100);

        if (dc_voltage_usage != "0") {
            myGLCD.setColor(RED);
            myGLCD.print("Backup: " + backup_ac_power + " W " + backup_ac_current + "A ", marginLeftSeconds, 120);
        }
    }
}

void displayEnvironment(String temp, String humidity, String soiMoisture) {
    int y = 180;
    myGLCD.setTextSize(2);
    myGLCD.setColor(0, 255, 0);
    myGLCD.print("Tmp: " + temp + "C Hdt: 55% ", marginLeft, y);
    // myGLCD.print("Humidity: " + humidity + " %     ", marginLeft, y + 20);
    myGLCD.print("Soi Moisture: " + soiMoisture + "  ", marginLeft, y + 20);
}

bool IsLoadEnabled = false;
void displayControlChargerInfo(String solarPanelVoltage, String solarPanelCurrent,
                               String solarPanelPower, String batteryVoltage, String batteryCharging, String batteryCapacity, String loadStatus, String deviceStatus,
                               String loadCurrent, String loadPower, String chargerTemp, String chargeAmount) {
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
        myGLCD.print(" (MPPT Charger) ", 270, 140);
        myGLCD.print("Bat Volt: " + batteryVoltage + " V ", marginLeftSeconds, 160);      //batteryVoltage
        myGLCD.print("Bat Capacity: " + batteryCapacity + "% ", marginLeftSeconds, 180);  //batteryCapacity

        if (loadStatus == "ON" && solarPanelVoltage == "0.0") {
            //Night Mode
            myGLCD.print("Load Curr: " + loadCurrent + " A ", marginLeftSeconds, 200);     //Load Current
            myGLCD.print("Load Power: " + loadPower + " W ", marginLeftSeconds, 220);      //Load Power
            myGLCD.print("Device Temp: " + chargerTemp + " C ", marginLeftSeconds, 240);   //Device Temperature
            myGLCD.print("Charge Amnt: " + chargeAmount + " Ah", marginLeftSeconds, 260);  //Charge Amount
        } else {
            //Day Mode
            myGLCD.print("Charge Curr: " + batteryCharging + " A ", marginLeftSeconds, 200);    //batteryCharging
            myGLCD.print("PV Volt: " + solarPanelVoltage + " V    ", marginLeftSeconds, 220);   //solarPanelVoltage
            myGLCD.print("PV Curr.: " + solarPanelCurrent + " A    ", marginLeftSeconds, 240);  //solarPanelCurrent
            myGLCD.print("PV Power: " + solarPanelPower + " W  ", marginLeftSeconds, 260);      //solarPanelPower
        }

        myGLCD.print("Load Status: " + loadStatus, marginLeftSeconds, 280);  //loadStatus
        myGLCD.setColor(MAGENTA);

        deviceStatus.trim();
        myGLCD.print((deviceStatus == "-") ? "" : deviceStatus, marginLeftSeconds, 300);  //deviceStatus
        IsLoadEnabled = loadStatus == "ON";
    }
}

void showDeviceInfo(String ipAddress, String lastUpdate) {
    int y = 300;
    myGLCD.setFont(SmallFont);
    myGLCD.setColor(0, 255, 0);
    if (IsLoadEnabled) {
        if (rotation == 0) {
        } else {
            myGLCD.print("IP:" + ipAddress + "           " + lastUpdate + "  ", marginLeftSeconds, y + 8);
        }
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