#include "nfc_pn532.h"
#include "wifi_c.h"
#include "mqtt.h"
#include "trackerlogic.h"

u_int p_time = 0;

void wifi_mqtt_connect_check(){
  if(!wifi_stat()){
      Serial.println("wifi not connected");
      wifi_init();
      delay(500);
      mqtt_connect();
      delay(500);
    }
}

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
  mqtt_init("Server", "/tracker/501ef728-6e4d-4d8d-b2b4-19ac01fcf96d/alert");
  Serial.println("MQTT connected");

  readerInstanceIdxInit();
  falsifyAlertIsTriggered();
}

void loop() {
  if(millis() - p_time >= 1000) {
    p_time = millis();
    Serial.println("------------" + String(p_time) + "------------");
    wifi_mqtt_connect_check();

    nfc_read_all();
    printNFCsContents();
    if (doorIsOpened()) {
      falsifyAlertIsTriggered();
      Serial.println("Door is Opened");
    } else {
      fetchRackStatusJson();
      updateInstanceReaderMapping();
      updateAllReadersStatus();
      alertsAccordingToReaderStatuses();
    }

  }
}
