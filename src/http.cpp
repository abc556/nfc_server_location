#include <http.h>

HTTPClient http;
String serverPath = "http://vdc50-4249.op/rest";

void http_begin()
{
    if(WiFi.status() == WL_CONNECTED){
        http.begin(serverPath.c_str());
    }else{
        Serial.println("wifi not connected");
    }
}

String http_get(String api, String param)
{
    String fullPath = serverPath + api + param;
    if(WiFi.status() == WL_CONNECTED){
        http.begin(fullPath.c_str());
        int httpResponseCode = http.GET();
        String payload = "{}";

        if (httpResponseCode>0) {
            Serial.print("HTTP Response code: ");
            Serial.println(httpResponseCode);
            payload = http.getString();
        }else {
            Serial.print("Error code: ");
            Serial.println(httpResponseCode);
        }
        http.end();
        return payload;
    }else{
        Serial.println("get failed, wifi not connected");
        return "get failed, wifi not connected";
    }
}

int http_post(String api, int alertId, String content)
{
    String fullPath = serverPath + api;

    if(WiFi.status() == WL_CONNECTED){
        WiFiClient client;
        HTTPClient http;

        http.begin(client, fullPath);

        http.addHeader("Content-Type", "application/x-www-form-urlencoded");
        String httpRequestData = content;

        int httpResponseCode = http.POST(httpRequestData);

        Serial.print("HTTP Response code: ");
        Serial.println(httpResponseCode);

        http.end();
        return httpResponseCode;
    }else{
        Serial.println("post failed, wifi not connected");
        return -1;
    }
}

void JSON_decode(String jsonString)
{
    DynamicJsonDocument doc(1024);

    deserializeJson(doc, F("{\"sensor\":\"gps\",\"time\":1351824120,"
                         "\"data\":[48.756080,2.302038]}"));

    JsonObject obj = doc.as<JsonObject>();
    String sensor = obj[F("sensor")];
    Serial.println(sensor.c_str());
}