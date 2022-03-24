#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoJSON.h>
#include <base64.h>


void http_begin();
String http_get(String api, String input, boolean authRequired, String username, String password);
int http_post(String api, String input, boolean authRequired, String username, String password);
DynamicJsonDocument JSON_deserialization(String jsonString);
String JSON_serialization(DynamicJsonDocument json);
