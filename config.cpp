//FILE: config.cpp
#include "config.h"
Adafruit_SSD1306 display = Adafruit_SSD1306(OLED_RESET);

config::config(){
     Particle.function("resetFunc", &config::resetFunc, this);  // Function for reseting the device remotely
}

void config::setup(){
    display.begin(SSD1306_SWITCHCAPVCC, 0x3C);
    display.display();
   // init done
    delay(2000);
    display.clearDisplay();   // clears the screen and buffer
    display.display();
   
}

String config::leadingZero(int x){
     String str0;
    if(x >=0 && x <=9){
        str0 = "0" + String(x);
        return str0;
    }else{
        return String(x);
    }
}

void config::sdLog(char* data, String fileName){
    if (!dataLog.open(fileName, O_RDWR | O_CREAT | O_AT_END)) {
        myconfig.errorMsg("failed opening file");
        sd.errorHalt("opening test.txt for write failed");
    }
    dataLog.print(data);        // Printing Data
    dataLog.println(",");       // , seperator
    
    dataLog.close();
}

void config::errorMsg(char* error){
    oledPrint(0,0,"Error with", true, 2);
    oledPrint(0,0,error, false, 2);
    digitalWrite(errorPin, HIGH);
    Particle.publish("ERROR", error, PRIVATE);
    sdLog(error, "log.txt");
    
}

int config::resetFunc(String command){
    oledPrint(0,0,"Reseting in 3 seconds", true,1.5);
    delay(3000);
    if(command == "true"){
        System.reset();
    }
    return 1;
}

int config::enterSafeMode(String command){
    if(command == "true"){
        System.enterSafeMode();
    }
}

void config::getDate(char *date){
   // char date[32] = "";
    char buffer[8] = "a";
    sprintf(buffer, "%d", Time.year());
    strcat(date, buffer);
    strcat(date, "-");
    strcat(date, leadingZero(Time.month()));
    strcat(date, "-");
    strcat(date, leadingZero(Time.day()));
  
}

void config::getTime(char *ti){
    
    strcat(ti,leadingZero(Time.hour()) );
    strcat(ti, ":");
    strcat(ti, leadingZero(Time.minute()));
    strcat(ti, ":");
    strcat(ti, leadingZero(Time.second()));
    
}


void config::oledPrint(int row, int col, String value, bool clear, float size){
    if(clear){
        display.clearDisplay();
    }
    display.setTextColor(WHITE);
    display.setCursor(row, col);
    display.setTextSize(size);
    display.println(value);
    display.display();
}

config myconfig;

