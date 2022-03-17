#include "wifi_c.h"

// const char* wifi_ssid = "abc556";
// const char* wifi_password = "dg123456";
const char* wifi_ssid = "TL23";
const char* wifi_password = "P@ss1234";
// const char* wifi_ssid = "trash";
// const char* wifi_password = "12345678";

void wifi_init(){
    WiFi.begin(wifi_ssid, wifi_password);
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);

    int timeS = millis();

    while(WiFi.status() != WL_CONNECTED && millis() - timeS < 10000){
        Serial.print('.');
        delay(100);
    }
    if(WiFi.status() != WL_CONNECTED)
        Serial.println("Wifi connect failed.");
}

bool wifi_stat() // connected will return true
{
    bool state = false;
    if (WiFi.status() == WL_CONNECTED)
        state = true;
    return state;
}