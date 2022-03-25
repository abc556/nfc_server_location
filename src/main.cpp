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
  // if(millis() - p_time >= 1000) {
    p_time = millis();
    Serial.println("------------" + String(p_time) + "------------");
    wifi_mqtt_connect_check();
    nfc_read_all();
    printNFCsContents();
    if (doorIsOpened()) {
      falsifyAlertIsTriggered();
      Serial.println("Door is Opened");
      door_closed_n_mqtt_published = false;
      count_door_closed = 0;
      mqtt_json_string = "";
    } else {
      fetchRackStatusJson();
      updateInstanceReaderMapping();
      updateAllReadersStatus();
      alertsAccordingToReaderStatuses();
      Serial.println("count_door_closed: " + String(count_door_closed));
      Serial.println(mqtt_json_string);
      if (count_door_closed >= ALERT_THRESHOLD+1 && door_closed_n_mqtt_published == false) {
        if(mqtt_json_string!="") {
          mqtt_json_string += "}";
          mqtt_pub(mqtt_json_string);
          door_closed_n_mqtt_published = true;
        }
      } else {
        count_door_closed++;
      }
    }

  // }
}
