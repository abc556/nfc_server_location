#ifndef _MQTT_H_
#define _MQTT_H_

#include <Arduino.h>
#include <WiFi.h>
#include <PubSubClient.h>

//extern WiFiClient espClient;
//extern PubSubClient client(WiFiClient);
extern const char* mqtt_server;
extern uint16_t mqtt_port;
extern String s;
extern char* mqtt_client_name;

extern char* MQTT_TOPIC_SERVER;

void mqtt_connect();
void mqtt_init(char* client, char* topic);
void mqttpub(char* content);


//to be added
//mpu
extern int stibx;
extern char cver[3][2];
extern char nver[3][2];



#endif