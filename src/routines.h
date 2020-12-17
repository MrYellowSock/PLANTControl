#include <NTPClient.h>
#include <WiFiUdp.h>
const int timezone = 7 * 3600; //GMT+7

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org");
/*
NTPClient keeps track of time event internet is not connected/update failed no exception thrown.
*/

void initNTP(){
    timeClient.begin();
    timeClient.setTimeOffset(timezone);
}
void updateTime(){
    timeClient.update();
}
int getHr(){
    return timeClient.getHours();
}
int getMinute(){
    return timeClient.getMinutes();
}
int getSec(){
    return timeClient.getSeconds();
}
String timeStr(){
    return timeClient.getFormattedTime();
}