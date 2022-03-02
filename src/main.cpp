#include "nfc_pn532.h"
#include "wifi_c.h"
#include "mqtt.h"

u_int p_time = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("Serial Begin.");

  //nfc begin
  nfc_begin();
  Serial.println("NFC Begin.");

  //wifi init
  wifi_init();
  Serial.println("Wifi Init.");

  //mqtt init
  mqtt_init("Server");

}

void loop() {
  if(millis() - p_time >= 1000) {
    p_time = millis();
    Serial.println("-----------------------" + String(millis()) + "-----------------------");

    if(!wifi_stat()){
      Serial.println("wifi not connected");
      wifi_init();
      delay(500);
      mqtt_connect();
      delay(500);
    }
  }

  //nfc
  nfc_loop();
}
