#include "wifi_c.h"

const char* wifi_ssid = "abc556";
const char* wifi_password = "dg123456";

void wifi_init(){
    WiFi.begin(wifi_ssid, wifi_password);
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);

    while(WiFi.status() != WL_CONNECTED){
        Serial.print('.');
        delay(100);
    }
}