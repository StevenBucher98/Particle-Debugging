// FILE: field.h
// All functions pertaining to collection of data

#include "config.h"

int bufferTime = 1000;              // Buffer time before going to sleep (in milliseconds)
const char*eventName = "data";      // Name of event for particle integration and GCP storage

SdFat sd;                       // SD card object
File dataLog;                   // File Object

I2CSoilMoistureSensor i2CSoilMoistureSensorDefault;     // Soil Mositure Sensor Obj
DS18B20  ds18b20(tempPin, true);                        // Internal Temp Sensor
const int MAXRETRY = 4;                                 // # of retries for internal temp
DFRobot_SHT20 sht20;                                    // Temp/Humidity Sensor
Adafruit_SI1145 uv = Adafruit_SI1145();                 // UV light sensor
PowerShield batteryMonitor;                             // 3.7V Battery Monitor

// Char variables for data that will be sent
char chTemp[16];
char chUVInfo[16];
char chHumidity[16];
char chIntTemp[16];
char chVisibleLight[16];
char chSoilMositure[16];
char chSolarVolt[16];
char chBatPercent[16];

// Data Variables
float fIntTemp;         // Internal Temperature of the sensor
float fIntTempF;        // Internal Temperature of the sensor Fahrenheit
float fHumidity;        // Humidity
float fTempC;           // Temp in Celius
float fTempF;           // Temp in Fahrenheit
float fUVIndex;         // UV index
float fVisibleLight;    // Visible Light
float fSoilMositure;    // Capacitance of soil
//float sTemp;          // Soil Temperature

// Power Variables
float fSolarVolt;           // Solar Panel Voltage Level
float fStateOfCharge;       // State of charge of the battery, Measured in %
float fCellOutputV;         // Output voltage of the battery (between 3.7V~4.08V)

uint32_t currentMemory;

// Publishing the Data to Particle Cloud and Google Cloud Platform
char* publishData(){
    char date[16] = "";
    char ti[12] = "";
    char data[200] = "";
    myconfig.getTime(ti);
    myconfig.getDate(date);
    strcat(data, "{\"name\":\"" );
    strcat(data, deviceName);
    strcat(data, "\",\"dateTime\":\"" );
    strcat(data, date);
    strcat(data, " ");
    strcat(data, ti);
    strcat(data, "\",\"data\":{");
    strcat(data, "\"t\":");
    strcat(data, chTemp);
    strcat(data, ",\"iT\":");
    strcat(data, chIntTemp);
    strcat(data, ",\"sM\":");
    strcat(data, chSoilMositure);
    strcat(data, ",\"h\":");
    strcat(data, chHumidity);
    strcat(data, ",\"uv\":");
    strcat(data, chUVInfo);
    strcat(data, ",\"vL\":");
    strcat(data, chVisibleLight);
    strcat(data, ",\"sV\":");
    strcat(data, chSolarVolt);
    strcat(data, ",\"bP\":");
    strcat(data, chBatPercent);
    strcat(data, "}}");
    
    myconfig.sdLog(data, "data.txt");
    
    Particle.publish("memory", String(currentMemory), PRIVATE);
    Particle.publish(eventName, data, PRIVATE);       //Published data to DataStore
    return data;
}

// Getting temperature and checking if valid
void getTemp(){
    fHumidity = sht20.readHumidity();
	fTempC = sht20.readTemperature();
	fTempF = ((9*fTempC)/5) + 32;
	
	int i =0;
	do {
        fIntTemp = ds18b20.getTemperature();
    } while (!ds18b20.crcCheck() && MAXRETRY > i++);
	
	fIntTempF = ds18b20.convertToFahrenheit(fIntTemp);
    if (isnan(fHumidity) || isnan(fTempC) || isnan(fTempF)) {
		myconfig.oledPrint(0,0,"Temp sensor Error", true, 2);
		//while(1);
	}
	
	if(fIntTempF >= 130 || fIntTempF <= 30){
        myconfig.oledPrint(0,0,"INTERNAL TEMP", true, 2);
        myconfig.errorMsg("interal temp");
        //TODO shut down or something
    }
}

// Getting UV index and other UV values down the line TODO
void getUV(){
    fUVIndex = uv.readUV();
    fVisibleLight = uv.readVisible();
    fUVIndex /=100;
}

// Getting Moisture from soil sensor
void getMositure(){
    fSoilMositure = i2CSoilMoistureSensorDefault.getCapacitance();
    //sTemp = i2CSoilMoistureSensorDefault.getTemperature();
}

// Getting power from power shield
void getPower(){
    fSolarVolt = (((float)analogRead(solarPin)) / 4095) * 3.3; // Reading analog value and converting to Voltage
    fSolarVolt *= 6.7;                                          // Multiplied by 5.7 since volage is multipled by 1/5.7 to be safe to read
    fStateOfCharge = batteryMonitor.getSoC();                          // Get percentage of charge left by the battery
    
}

//Time and battery percentage for 128X32 oled
void displayData(String id, String val, int delayTime){
    char ti[12] = "";
    myconfig.getTime(ti);
    myconfig.oledPrint(0,0,"Time:", false, 1.3);
    myconfig.oledPrint(30,0, ti, false, 1.3);
    myconfig.oledPrint(90,0, chBatPercent, false, 1.3);
    myconfig.oledPrint(120, 0, "%", false ,1.3);
    myconfig.oledPrint(0,15, id, false, 2);
    myconfig.oledPrint(40,15, val, false, 2);
    display.display();
    delay(delayTime);
    display.clearDisplay();
}

//display the recorded data
void oled(){
    
    display.clearDisplay();
    display.display();
    display.setTextColor(WHITE);
    
    
    displayData("T:", chTemp, 1000);
    displayData("iT:", chIntTemp, 1000);
    displayData("SM:", chSoilMositure, 1000);
    displayData("UV:", chUVInfo, 1000);
    displayData("H:", chHumidity, 1000);
    displayData("VL:", chVisibleLight, 1000);
    displayData("SV:", chSolarVolt, 1000);
    //display.clearDisplay();
    display.display();

// Uncomment if OLED is 128x64
//     Particle.publish("oled begin");
//     display.clearDisplay();
//     display.display();
//     display.setTextColor(WHITE);
//     myconfig.oledPrint(0,0,"Time:", false, 1.3);
//     myconfig.oledPrint(30,0,myconfig.getTime(), false, 1.3);
  
//     myconfig.oledPrint(90,0, chBatPercent, false, 1.3);
//     myconfig.oledPrint(120, 0, "%", false ,1.3);

//     myconfig.oledPrint(0,20, "T:", false, 1.3);
//     myconfig.oledPrint(15,20, chTemp, false, 1.3);
  
//     myconfig.oledPrint(0,30, "IT:", false, 1.3);
//     myconfig.oledPrint(23,30, chIntTemp, false, 1.3);

//     myconfig.oledPrint(0,40, "SM:", false, 1.3);
//     myconfig.oledPrint(23,40, chSoilMositure, false, 1.3);

//     myconfig.oledPrint(0,50, "VL:", false, 1.3);
//     myconfig.oledPrint(23,50, chVisibleLight, false, 1.3);
  
//     myconfig.oledPrint(70,20, "UV:", false, 1.3);
//     myconfig.oledPrint(93,20, chUVInfo, false, 1.3);
  
//     myconfig.oledPrint(70,30, "H:", false, 1.3);
//     myconfig.oledPrint(85,30, chHumidity, false, 1.3);
  
//     myconfig.oledPrint(70,40, "SV:", false, 1.3);
//     myconfig.oledPrint(93,40, chSolarVolt, false, 1.3);
  
//     myconfig.oledPrint(70,50, "XX:", false, 1.3);
//     myconfig.oledPrint(93,50, "YY.YY", false, 1.3);

//     display.display();
//     delay(1000);
//     Particle.publish("Oled end");

}

