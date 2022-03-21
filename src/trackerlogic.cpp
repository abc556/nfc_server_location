#include "trackerlogic.h"

extern DynamicJsonDocument rack_json(1024);
extern int instance_idx [reader_count] = { -1 };
extern int min_U = 2;

void updateRackInfoJson(){
    // String content = http_get("/devices/rdinfo", "501ef728-6e4d-4d8d-b2b4-19ac01fcf96d", true, "admin", "Monit@r#1");
    String content = "{\"code\": \"1\",\"data\": [{\"description\": \"test\",\"mid\": \"5f50592c-44e2-11de-9ac7-000d566af2f2\",\"upos\": 40.0,\"pid\": \"ca7391ec-d002-11dd-8252-001d091dd9dd\",\"mname\": \"2950III\",\"acl\": 15,\"tid\": \"59e3262e-cd7b-11dd-81bf-001d091dd9dd\",\"sid\": 21115,\"vid\": \"6b16fb0e-cd81-11dd-83bf-001d091dd9dd\",\"is_reserved\": false,\"wo\": false,\"id\": \"cd4df3b8-2b9d-4aae-b047-85181022cefa\",\"sn\": null,\"eups\": null,\"owner\": null,\"side\": \"\",\"enclourse\": null,\"pname\": \"PowerEdge\",\"status_name\": \"Operational\",\"ip\": null,\"tname\": \"Server - Rackmount\",\"psid\": 1670,\"tsid\": 533,\"vsid\": 246,\"shelf\": null,\"msid\": 28436,\"slots\": null,\"vname\": \"Dell\",\"at\": \"at123456\",\"reserved\": false,\"name\": \"NFC server demo\",\"_id\": \"cd4df3b8-2b9d-4aae-b047-85181022cefa\",\"position\": \"40.0 U\",\"_pid_\": null,\"_is_child_\": false,\"status\": 1},{\"description\": null,\"mid\": \"9d2b566a-d52d-11ea-bf0d-b32c7d6d75cf\",\"upos\": 38.0,\"pid\": \"ca7391ec-d002-11dd-8252-001d091dd9dd\",\"mname\": \"R740xd (12D 3.5)\",\"acl\": 15,\"tid\": \"59e3262e-cd7b-11dd-81bf-001d091dd9dd\",\"sid\": 21116,\"vid\": \"6b16fb0e-cd81-11dd-83bf-001d091dd9dd\",\"is_reserved\": false,\"wo\": false,\"id\": \"032d6098-a43e-4d67-8def-2f2f5e823158\",\"sn\": null,\"eups\": null,\"owner\": null,\"side\": \"\",\"enclourse\": null,\"pname\": \"PowerEdge\",\"status_name\": \"Operational\",\"ip\": null,\"tname\": \"Server - Rackmount\",\"psid\": 1670,\"tsid\": 533,\"vsid\": 246,\"shelf\": null,\"msid\": 54715,\"slots\": null,\"vname\": \"Dell\",\"at\": null,\"reserved\": false,\"name\": \"NFC Server Demo -1\",\"_id\": \"032d6098-a43e-4d67-8def-2f2f5e823158\",\"position\": \"38.0 U\",\"_pid_\": null,\"_is_child_\": false,\"status\": 1}],\"message\": \"Success\"}";
    // String content = "{\"sensor\":\"gps\",\"time\":1351824120,\"data\":[48.756080,2.302038]}";
    rack_json = JSON_deserialization(content);
}

void updateSupposedReadings(int start_u){
    for(int i = 0; i<sizeof(rack_json["data"]); i++) {
        DynamicJsonDocument temp_json = JSON_deserialization(rack_json["data"][i]);
        String temp_position = temp_json["position"];
        std::replace(temp_position.begin(), temp_position, ".", " ");
        stringstream ssin(temp_position);
        string temp_array[3];
        while (ssin.good() && i < 3){
            ssin >> temp_array[i];
            ++i;
        }
        int reader_num = (stoi(temp_array[0]) - start_u) / min_U;
        reader_correct_idx [reader_num] = i; // [1, -1, 2, -1, -1, -1, 3, -1]
    }
}

void rackInfoAlertChecking(){
    int nfc_reader_idx = 0;
    int supposed_instance_idx = 0;
    std::vector<int> alert_missing_array;
    std::vector<int> alert_incorrect_array;
    std::vector<int> alert_redundant_array;
    String reader_result = "";

    reader_result = nfc1_read();
    nfc_reader_idx = 0;
    supposed_instance_idx = reader_correct_idx[nfc_reader_idx];
    if( supposed_instance_idx != -1) {
        if (reader_result == "nonfc") {
            alert_missing_array.push_back(supposed_instance_idx);
        } else {
            if (reader_result != rack_json["data"][supposed_instance_idx]) {
                nfc1_write(rack_json["data"][supposed_instance_idx]);
                alert_incorrect_array.push_back(supposed_instance_idx);
            }
        }
    } else {
        if (reader_result != "") {
            alert_redundant_array.push_back(nfc_reader_idx);
        }
    }
    reader_result = nfc2_read();
    nfc_reader_idx = 1;
    supposed_instance_idx = reader_correct_idx[nfc_reader_idx];
    if( supposed_instance_idx != -1) {
        if (reader_result == "nonfc") {
            alert_missing_array.push_back(supposed_instance_idx);
        } else {
            if (reader_result != rack_json["data"][supposed_instance_idx]) {
                nfc2_write(rack_json["data"][supposed_instance_idx]);
                alert_incorrect_array.push_back(supposed_instance_idx);
            }
        }
    } else {
        if (reader_result != "") {
            alert_redundant_array.push_back(nfc_reader_idx);
        }
    }
    reader_result = nfc3_read();
    nfc_reader_idx = 2;
    supposed_instance_idx = reader_correct_idx[nfc_reader_idx];
    if( supposed_instance_idx != -1) {
        if (reader_result == "nonfc") {
            alert_missing_array.push_back(supposed_instance_idx);
        } else {
            if (reader_result != rack_json["data"][supposed_instance_idx]) {
                nfc3_write(rack_json["data"][supposed_instance_idx]);
                alert_incorrect_array.push_back(supposed_instance_idx);
            }
        }
    } else {
        if (reader_result != "") {
            alert_redundant_array.push_back(nfc_reader_idx);
        }
    }
    reader_result = nfc4_read();
    nfc_reader_idx = 3;
    supposed_instance_idx = reader_correct_idx[nfc_reader_idx];
    if( supposed_instance_idx != -1) {
        if (reader_result == "nonfc") {
            alert_missing_array.push_back(supposed_instance_idx);
        } else {
            if (reader_result != rack_json["data"][supposed_instance_idx]) {
                nfc4_write(rack_json["data"][supposed_instance_idx]);
                alert_incorrect_array.push_back(supposed_instance_idx);
            }
        }
    } else {
        if (reader_result != "") {
            alert_redundant_array.push_back(nfc_reader_idx);
        }
    }
    reader_result = nfc5_read();
    nfc_reader_idx = 4;
    supposed_instance_idx = reader_correct_idx[nfc_reader_idx];
    if( supposed_instance_idx != -1) {
        if (reader_result == "nonfc") {
            alert_missing_array.push_back(supposed_instance_idx);
        } else {
            if (reader_result != rack_json["data"][supposed_instance_idx]) {
                nfc5_write(rack_json["data"][supposed_instance_idx]);
                alert_incorrect_array.push_back(supposed_instance_idx);
            }
        }
    } else {
        if (reader_result != "") {
            alert_redundant_array.push_back(nfc_reader_idx);
        }
    }
    reader_result = nfc6_read();
    nfc_reader_idx = 5;
    supposed_instance_idx = reader_correct_idx[nfc_reader_idx];
    if( supposed_instance_idx != -1) {
        if (reader_result == "nonfc") {
            alert_missing_array.push_back(supposed_instance_idx);
        } else {
            if (reader_result != rack_json["data"][supposed_instance_idx]) {
                nfc6_write(rack_json["data"][supposed_instance_idx]);
                alert_incorrect_array.push_back(supposed_instance_idx);
            }
        }
    } else {
        if (reader_result != "") {
            alert_redundant_array.push_back(nfc_reader_idx);
        }
    }
    
    if alert_missing_array.size() > 0 {
        int arr[alert_missing_array.size()];
        std::copy(alert_missing_array.begin(), alert_missing_array.end(), arr)
        // mqtt alert missing
    }
    if alert_incorrect_array.size() > 0 {
        int arr[alert_incorrect_array.size()];
        std::copy(alert_incorrect_array.begin(), alert_incorrect_array.end(), arr)
        // mqtt alert incorrect
    }
    if alert_redundant_array.size() > 0 {
        int arr[alert_redundant_array.size()];
        std::copy(alert_redundant_array.begin(), alert_redundant_array.end(), arr)
        // mqtt alert redundant 
    }
}
