#include "mqtt.h"

WiFiClient espClient;
PubSubClient mqttclient(espClient);
// const char* mqtt_server = "18.162.55.224";
const char* mqtt_server = "muselabs-mqtt.com";
// uint16_t mqtt_port = 9000;
uint16_t mqtt_port = 1883;
String s=";";
int id = 0;
char* mqtt_client_name;

char* MQTT_TOPIC_SERVER = "/server";

void mqtt_connect(){
    mqttclient.connect(mqtt_client_name);
    Serial.println("mqtt connected as" + String(mqtt_client_name));
}

void mqtt_init(char* client, char* topic){
    mqttclient.setServer(mqtt_server, mqtt_port);
    mqtt_client_name = client;
    MQTT_TOPIC_SERVER = topic;
    mqtt_connect();
}

void mqtt_pub(String content){
  if (!mqttclient.connected()) {
    Serial.println("mqtt broker not connected. Reconnecting ...");
    mqtt_connect();
    delay(500);
  }
  else{
    mqttclient.publish(MQTT_TOPIC_SERVER, content.c_str());
    Serial.println("published");
  }
}


