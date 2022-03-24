#include "nfc_pn532.h"
#include "wifi_c.h"
#include "mqtt.h"
#include "http.h"
#include "trackerlogic.h"
#include "iostream"

u_int p_time = 0;

DynamicJsonDocument obj(1024);
String serial = "{\"id\":\"cd4df3b8-2b9d-4aae-b047-85181022cefa\",\"model_id\":\"5f50592c-44e2-11de-9ac7-000d566af2f2\",\"name\":\"NFC server demo\",\"description\":\"test\",\"attributes\":[{\"id\":\"0aeddd86-ce3c-11dd-8da6-001d091dd9dd\",\"name\":\"asset_tag\",\"value\":\"at123\"}]}";

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
}

void loop() {
  if(millis() - p_time >= 1000) {
    p_time = millis();
    Serial.println("-----------------------" + String(millis()) + "-----------------------");
  //   // mqtt_pub("{\"1\":\"\",\"2\":\"\",\"3\":\"{\"name\": \"Countdown14\", \"size\":3}\",\"4\":\"{\"name\": \"Viewco\", \"size\":1}\"");
    wifi_mqtt_connect_check();
    // obj = JSON_deserialization("{\"sensor\":\"gps\",\"time\":1351824120,"
    //                      "\"data\":[48.756080,2.302038]}");
    // serial = JSON_serialization(obj);
    // Serial.println("serial:");
    // Serial.println(serial);
    // serial = "{\"id\":\"cd4df3b8-2b9d-4aae-b047-85181022cefa\",\"model_id\":\"5f50592c-44e2-11de-9ac7-000d566af2f2\",\"name\":\"NFC server demo\",\"description\":\"test\",\"attributes\":[{\"id\":\"0aeddd86-ce3c-11dd-8da6-001d091dd9dd\",\"name\":\"asset_tag\",\"value\":\""+String(millis())+"\"}]}";
    // Serial.println(http_get("/devices/rdinfo", "501ef728-6e4d-4d8d-b2b4-19ac01fcf96d", true, "admin", "Monit@r#1"));
    // http_post("/devices", serial, true, "admin", "Monit@r#1");
    // nfc_loop();
    updateRackInfoJson();
    updateSupposedReadings(16);
    rackInfoAlertChecking();
    // rackInfoComparison();
  }

  //nfc
  // nfc_loop();
  // mqtt_pub(nfc_reading_combine());
}
