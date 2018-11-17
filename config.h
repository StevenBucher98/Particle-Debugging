// FILE: config.h

// CONSTRUCTOR
//     config();

// SETUP
//     setup();
    
// MEMBER MODIFICATION FUNCTIONS
    // String leadingZero(int x);
    //     Postcondition: Adds a leading zero to any digit before 10 to keep format
    
    // void sdLog(String data, String filename);
    //     Precondition: SD card  was opened succesfully
    //     Postcondition: Logging the data to file "temperature.txt"  or error to "log.txt" on the microSD card
    // void errorMsg(String error);
    //     Postcondition: Prints and logs any errors messages
    // int resetFunc(String command);
    //     Postcondition: Resets the photon
        
    // int enterSafeMode(String command);
    //     Postcondition: Puts the photon in safe mode
    
    // String getDate();
    //     Postcondition: Gets the current date in the format YYYY-MM-DD
    // String getTime();
    //     Postcondition: Gets the current time in the format HH:MM:SS
    // void oledPrint(int row, int col, String value, bool clear, float size);
    //     Precondition: valid row, col, value, clear, and size values are given
    //     Postcondition: "value" is printed on the oled based on the passed parameters
        
    
#include <LiquidCrystal_I2C_Spark.h>
#include <I2CSoilMoistureSensor.h>
#include <DS18B20.h>
#include <Adafruit_SI1145.h>
#include <SdFat.h>
#include <PowerShield.h>
#include "application.h"
#include "DFRobot_SHT20.h"
#include <Adafruit_SSD1306.h>

#include <cstdlib> 
#include <cstdio>
#include <string>

// Pins
#define errorPin D7         // Pin for Error LED
#define tempPin D2          // Internal Temperature Pin
#define solarPin A0         // Analog Input Pin for Solar Voltage
#define relayPin D4         // Relay Pin

// OLED
#define OLED_RESET D5
#define NUMFLAKES 10
#define XPOS 0
#define YPOS 1
#define DELTAY 2

const int INTERVAL_TIME = 300;  // Interval time for how long between data samples

const String deviceName = "field5";
const int tZone = -7;

extern SdFat sd;                        // SD Card   
extern File dataLog;                    // File 
const uint8_t chipSelect = SS;          // Chip select SS = CS -> A2

extern Adafruit_SSD1306 display;


// Uncomment if using 128x64 OLED
// #if (SSD1306_LCDHEIGHT != 64)
// #error("Height incorrect, please fix Adafruit_SSD1306.h!");
// #endif

using namespace std;

    class config{
        public:
            config();
            void setup();
            String leadingZero(int x);
            void sdLog(char *data, String fileName);
            void errorMsg(char *error);
            int resetFunc(String commmand);
            int enterSafeMode(String command);
            void getDate(char *date);
            void getTime(char *ti);
            void oledPrint(int row, int col, String value, bool clear, float size);
    };
    extern config myconfig;
        