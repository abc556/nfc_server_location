#include <http.h>

HTTPClient http;
// String serverPath = "http://vdc50-4249.op/rest";
String serverPath = "http://192.168.0.51/rest";

void http_begin()
{
    if(WiFi.status() == WL_CONNECTED){
        http.begin(serverPath.c_str());
    }else{
        Serial.println("wifi not connected");
    }
}

String http_get(String api, String param, boolean authRequired=false, String username="", String password="")
{
    String fullPath = serverPath + api + "/" + param;
    if(WiFi.status() == WL_CONNECTED){
        http.begin(fullPath.c_str());
        if(authRequired){
            String auth = base64::encode(username + ":" + password);
            http.addHeader("Authorization", "Basic " + auth);
        }
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

int http_post(String api, String jsonString, boolean authRequired=false, String username="", String password="")
{
    String fullPath = serverPath + api;

    if(WiFi.status() == WL_CONNECTED){
        WiFiClient client;
        HTTPClient http;

        http.begin(client, fullPath);

        http.addHeader("Content-Type", "application/json");
        String httpRequestData = jsonString;
        if(authRequired){
            String auth = base64::encode(username + ":" + password);
            http.addHeader("Authorization", "Basic " + auth);
        }
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

DynamicJsonDocument JSON_deserialization(String jsonString)
{
    DynamicJsonDocument obj(1024);
    deserializeJson(obj, jsonString);
    return obj;
}

String JSON_serialization(DynamicJsonDocument json)
{
    String serialString;
    serializeJson(json, serialString);
    return serialString;

}