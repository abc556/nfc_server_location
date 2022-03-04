#include "nfc_pn532.h"
#include "wifi_c.h"
#include "mqtt.h"

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
  // nfc_begin();
  Serial.println("NFC Begin.");

  //wifi init
  wifi_init();
  Serial.println("Wifi Init.");

  //mqtt init
  mqtt_init("Server", "/server");

}

void loop() {
  // if(millis() - p_time >= 1000) {
  //   p_time = millis();
  //   Serial.println("-----------------------" + String(millis()) + "-----------------------");
  //   // mqtt_pub("{\"1\":\"\",\"2\":\"\",\"3\":\"{\"name\": \"Countdown14\", \"size\":3}\",\"4\":\"{\"name\": \"Viewco\", \"size\":1}\"");
  // }

  //nfc
  // nfc_loop();
  mqtt_pub(nfc_reading_combine());
  wifi_mqtt_connect_check();

}
