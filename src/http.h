#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJSON.h>


void http_begin();
String http_get(String api, String input);
int http_post(String api, String input);
void JSON_decode(String jsonString);
