#include "trackerlogic.h"

void getRackInfoJson(){
    // String content = http_get(\"/devices/rdinfo\", \"501ef728-6e4d-4d8d-b2b4-19ac01fcf96d\", true, \"admin\", \"Monit@r#1\");
    String content = "{\"code\":\"1\", \"data\":{\"slot\":\"3\"}}";
    // DynamicJsonDocument json = JSON_deserialization(content);
    
    // Serial.println(JSON_deserialization(content)["data"]);
}

