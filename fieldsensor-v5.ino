//FILE: fieldsensor-v4.ino
#include "field.h"

STARTUP(System.enableFeature(FEATURE_RESET_INFO));  // Enabling reason for last reset to check for crashes
STARTUP(WiFi.selectAntenna(ANT_AUTO));              // Auto search for antenna 

int signalstrength;

void setup() {
    
    pinMode(relayPin, OUTPUT);
    pinMode(errorPin, OUTPUT);
    pinMode(solarPin, INPUT);
    
    digitalWrite(relayPin, HIGH);
    
    // Next lines for power shield
    batteryMonitor.begin();
    // This sets up the fuel gauge
    batteryMonitor.quickStart();

    if (System.resetReason() == 0) {
        Particle.publish("ERROR", "CRASH, ENTERING SAFE MODE");
        System.enterSafeMode();
    }
    
    // Config Set up
    myconfig.setup();

    // Checking if connected to the internet
    if(Particle.connected() == false){
        digitalWrite(errorPin, HIGH);
        myconfig.errorMsg("Particle Cloud");
    }
    
    myconfig.oledPrint(0,0, "ON", true, 2.75);
    
    //Initializing the UV Sensor
    while(! uv.begin()) {
        myconfig.oledPrint(0,0,"error", true, 2.5);
        delay(1000);
    }
    
    // Beginning the MicroSD card and checking for any error
    if (!sd.begin(chipSelect, SPI_HALF_SPEED)) {
        myconfig.errorMsg("SD card Error");
        sd.initErrorHalt();
    }
    
    sht20.initSHT20();  // Init SHT20 Sensor
    delay(100);
    sht20.checkSHT20(); // Check SHT20 Sensor
    
    // Setting default address of soil moisture to 0x20
    i2CSoilMoistureSensorDefault.begin();

}

void loop() {
     // Syncing and setting time zone to pacific
    Particle.syncTime();
    Time.zone(tZone);
    
    delay(1000);
    getTemp();
    getUV();
    getMositure();
    getPower();    

    //Converting float ints to char arrays and then strings
    sprintf(chTemp, "%2.2f", fTempF);
    sprintf(chUVInfo, "%2.2f", fUVIndex);
    sprintf(chHumidity, "%2.2f", fHumidity);
    sprintf(chIntTemp, "%2.2f", fIntTempF);
    sprintf(chVisibleLight, "%2.2f", fVisibleLight);
    sprintf(chSoilMositure, "%2.2f", fSoilMositure);
    sprintf(chSolarVolt, "%2.2f", fSolarVolt);
    sprintf(chBatPercent, "%2.2f", fStateOfCharge);
    
    oled();
    delay(1000);
    currentMemory = System.freeMemory();
    //Particle.publish("mem", currentMemory);
    
    publishData(); // Published and logs to the SD card
    signalstrength = WiFi.RSSI();
    Particle.publish("Signal", String(signalstrength));
    
    delay(bufferTime);

    display.clearDisplay();
    display.display();
    
    digitalWrite(relayPin, LOW);
    System.sleep(SLEEP_MODE_DEEP, INTERVAL_TIME);
    
}