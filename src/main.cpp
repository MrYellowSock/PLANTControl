#include <Arduino.h>
#include <ESP8266WebServer.h>
#include <pgmspace.h>
#include <EEPROM.h>
#include <Ticker.h>
#include <web.h>
#include <connector.h>
#include <SparkFun_SHTC3.h>
#include <M2M_LM75A.h>

#define bulb 14
#define addr_setting_led 30
#define addr_setting_air 40
#define addr_setting_col 50

uint8_t hours[hrAday] = {10,20,0,30,60,80,20,10,10,20,50,50,50,50,50,50,50,50,50,50,50,50,50,50};
float fanTemp_led =  0;
float fanTemp_air = 0;
uint8_t collect_interval_minute= 1;

float temp_led = 0;
float temp_air = 0;
float RH = 0;

void onHrChanged(int h){
    analogWrite(bulb,hours[h]);
}

void loadEE(){
    EEPROM.begin(512);
    collect_interval_minute = (uint8_t)EEPROM.read(addr_setting_col);
    for(int i=0;i<hrAday;i++){
        hours[i] = (uint8_t)EEPROM.read(i);        
    }
    EEPROM.get(addr_setting_led,fanTemp_led);
    EEPROM.get(addr_setting_air,fanTemp_air);
    if(isnan(fanTemp_led)){
        fanTemp_led = 0;
        EEPROM.put(addr_setting_led,fanTemp_led);
    }
    if(isnan(fanTemp_air)){
        fanTemp_air = 0;
        EEPROM.put(addr_setting_air,fanTemp_air);
    }
    EEPROM.commit();
}

bool serveOn = true;
ESP8266WebServer server(80);
void handleRoot() {
    server.send(200, "text/html", getFullPage(hours,RH,temp_air,temp_led,fanTemp_led,fanTemp_air,collect_interval_minute));   // Send HTTP status 200 (Ok) and send some text to the browser/client
}
void handleSetled(){
    int hr = atoi(server.arg(0).c_str());
    int brightness = atoi(server.arg(1).c_str());

    if(hr >= hrAday)
        hr = hrAday-1;
    if(brightness > 100)
        brightness = 100;
    hours[hr] = brightness;
    if(hr == getHr()){
        onHrChanged(hr);
    }
    EEPROM.write(hr,brightness);
    EEPROM.commit();
    server.send(200,"text/plain","OK");
}
void handleSetting(){
    String targ = server.arg(0).c_str();
    float val = atof(server.arg(1).c_str());
    if(targ == "air"){
        fanTemp_air = val;
        EEPROM.put(addr_setting_air,val);
    }
    else if(targ == "led"){
        fanTemp_led = val;
        EEPROM.put(addr_setting_led,val);
    }
    else{
        collect_interval_minute = val;
        EEPROM.write(addr_setting_col,val);
    }
    EEPROM.commit();
    server.send(200,"text/plain","OK");
}

M2M_LM75A lm75a;
#define WIRE_PORT Wire
#define WIRE_SPEED  800000  // 800 kHz is the fastest speed that worked on the Uno, but the sensor is rated for up to 1 MHz
SHTC3 mySHTC3;              // Declare an instance of the SHTC3 class
void errorDecoder(SHTC3_Status_TypeDef message)                             // The errorDecoder function prints "SHTC3_Status_TypeDef" resultsin a human-friendly way
{
  switch(message)
  {
    case SHTC3_Status_Nominal : Serial.println("Nominal"); break;
    case SHTC3_Status_Error : Serial.println("Error"); break;
    case SHTC3_Status_CRC_Fail : Serial.println("CRC Fail"); break;
    default : Serial.println("Unknown return code"); break;
  }
}
bool softOn = false;
void moto_on(int pin)
{
  analogWrite(pin,20);
  delay(1500);
  digitalWrite(pin,HIGH);
}
#define fan1 13
#define fan2 15
void sense(){
    //sensor and fan stuffs.
    float prev_temp_air = temp_air;

    mySHTC3.update(); 
    if(mySHTC3.lastStatus == SHTC3_Status_Nominal)              // You can also assess the status of the last command by checking the ".lastStatus" member of the object
    {
        RH = mySHTC3.toPercent();
        temp_air = mySHTC3.toDegC();

        Serial.print("DHT : RH = "); 
        Serial.print(RH);    
        Serial.print("%, T = "); 
        Serial.println(temp_air);
    }

    lm75a.wakeup();
    temp_led = lm75a.getTemperature();
    Serial.print("LM75A : ");
    Serial.println(temp_led);
    
    if(temp_led >= fanTemp_led){
        moto_on(fan1);
        Serial.println("YES1");
    }
    else{
        digitalWrite(fan1,LOW);
    }
    if(temp_air > prev_temp_air+0.01){
        moto_on(fan2);
        Serial.println("YES2");
    }
    else{
        digitalWrite(fan2,LOW);
    }
}


Ticker updater;
bool needUpdate = false;
void uppy(){
    if(!conn){
        needUpdate = true;
    }
    else{
        updater.detach();
    }
}
int startMILI;

String sheet_URL = "/macros/s/AKfycbwoyprE-FeYLJkiMYpwkQeaxsjZFoYOwUxhMvW39_k_a_yhIa8/exec";
void onMinChanged (int min){
    if(min % collect_interval_minute == 0 || collect_interval_minute >= 60 && min == 0){
        //collect dht sensor info
        checkNconnect();
        if(WiFi.status() == WL_CONNECTED){
            float rh=-1000,C=-1000;
            if(mySHTC3.lastStatus == SHTC3_Status_Nominal){
                mySHTC3.update();
                rh = mySHTC3.toPercent();
                C = mySHTC3.toDegC();
            }
            String URL = sheet_URL+"?temp="+C+"&rh="+rh;
            httpGet(URL);
        }
    }

    if(!serveOn && !needUpdate){
        //sleep
        Serial.println("Wifi Off.");
        digitalWrite(indicator_led,LOW);
        WiFi.forceSleepBegin();
        //delay(10000);
        //WiFi.forceSleepWake();  //wake automatically.
    }
}
void onSecChanged (int sec){
    if(sec % 10 == 0){
        sense();
    }
}

void dhtSetup(){
    WIRE_PORT.begin();
    #if(WIRE_SPEED <= SHTC3_MAX_CLOCK_FREQ)                     // You can boost the speed of the I2C interface, but keep in mind that other I2C sensors may not support as high a speed!
        WIRE_PORT.setClock(WIRE_SPEED);                             
    #else
        WIRE_PORT.setClock(SHTC3_MAX_CLOCK_FREQ);
    #endif
    Serial.print("dht22 = ");           // Most SHTC3 functions return a variable of the type "SHTC3_Status_TypeDef" to indicate the status of their execution 
    errorDecoder(mySHTC3.begin(WIRE_PORT));  
    Serial.print("Choosing low-power measurements with T first: ");
    if(mySHTC3.setMode(SHTC3_CMD_CSE_TF_LPM) == SHTC3_Status_Nominal)
    {
        Serial.print(" successful");
    }
    else
    {
        Serial.print(" failed");
    }
    Serial.println("\n");
    
}
void setup() {
    //Pin setup
    pinMode(bulb,OUTPUT);
    pinMode(indicator_led,OUTPUT);
    pinMode(fan1,OUTPUT);
    pinMode(fan2,OUTPUT);
    digitalWrite(indicator_led,LOW);
    analogWriteFreq(10000);
    analogWriteRange(100);
    analogWrite(bulb,hours[getHr()]);

    //test
    //moto_on(fan1);

    //Serial
    Serial.begin(115200);

    //WIFI
    WiFi.mode(WIFI_STA);
    checkNconnect();

    //Server
    loadEE();
    server.on("/", handleRoot);      //Which routine to handle at root location
    server.on("/setled",handleSetled);
    server.on("/setting",handleSetting);
    server.begin();                

    //ntp
    initNTP();
    updateTime();
    startMILI = millis();
    updater.attach(60*10,uppy);
    
    //TEMP SENS
    dhtSetup();

    lm75a.begin();
    lm75a.wakeup();
    Serial.println(lm75a.getDeviceMode());
}


int prev_hr  = -1;
int prev_min = -1;
int prev_sec = -1;
void loop(){
    if(getHr() != prev_hr){
        prev_hr = getHr();
        onHrChanged(prev_hr);
    }
    if(getMinute() != prev_min){
        prev_min = getMinute();
        onMinChanged(prev_min);
    }
    if(getSec() != prev_sec){
        prev_sec = getSec();
        onSecChanged(prev_sec);
    }
    
    if(serveOn){
        if(millis()-startMILI >= server_live_minute*60*1000){
            serveOn = false;
            Serial.println("Server Off.");
        }
        else{
            checkNconnect();
            server.handleClient();
        }
    }
    else if(needUpdate){
        checkNconnect();
        updateTime();
        Serial.println("Update attempted.");
        needUpdate = false;
    }
}