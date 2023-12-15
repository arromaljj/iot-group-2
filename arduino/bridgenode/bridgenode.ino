#include <ESP8266WiFi.h>
#include <espnow.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <NTPClient.h>
#include <WiFiUdp.h>

// NTP Client setup
WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, "pool.ntp.org", 0, 60000); // Time offset in seconds (adjust according to your timezone), update interval in milliseconds

// MQTT Broker settings`
const char* mqttServer = "172.20.10.3";
const int mqttPort = 1883;
const char* mqttDeviceId = "seat_pad";
const char* mqttUser = ""; // No authentication needed for now
const char* mqttToken = ""; // No authentication needed for now
const char* mqttTopic = "seat_pad/data/json";
const char* mqttTopicDisplay = "seat_pad/display/json";
const char* mqttTopicCommand = "esp/command";

// Replace with the MAC Address of ESP-A
uint8_t espAMacAddress[] = {0x08,0x3A,0x8D,0xEE,0xE2,0x27};
//08:3A:8D:EE:E2:27

// WiFi credentials
//char ssid[] = "anish";  // Your network SSID (name)
//char pass[] = "12345678";  // Your network password

char ssid[] = "iPhone (8)";  //  your network SSID (name)
char pass[] = "gimmejuice";  //

// String ssid2 = "RALT_RIoT";


// Deep sleep and wake duration settings
//const unsigned long sleepDuration = 30e6; // 30 seconds in microseconds
//const unsigned long wakeDuration = 120e3; 
//unsigned long wakeTime = 0;

#define BUTTON_PIN 2 // GPIO pin the push button is connected to

// MQTT objects and variables
WiFiClient wifiClient;
PubSubClient mqttClient(mqttServer, mqttPort, wifiClient);

// JSON document for MQTT payload
StaticJsonDocument<100> jsonDoc;
JsonObject payload = jsonDoc.to<JsonObject>();
JsonObject status = payload.createNestedObject("d");
char msg[50];
bool counter = 0;
bool buttonState = 0;


// Structure to send data
typedef struct struct_message {
    uint8_t mac[6];
    bool limitSwitchActive;
    char* get_string() {
        char counterStr[50];
        snprintf(counterStr, 50,  "%i: %s", counter, limitSwitchActive ? "true" : "false");
        return counterStr; 
    }

    char* get_mac(char* macStr, size_t size){
        snprintf(macStr, size, "%02X:%02X:%02X:%02X:%02X:%02X", 
                 mac[0], mac[1], mac[2], 
                 mac[3], mac[4], mac[5]);
        return macStr;
    }

    char* get_json(char* json, size_t size){
        char _mac[18];
        get_mac(_mac, 18);
        // snprintf(json, 100, "{\"mac\": \"%02X:%02X:%02X:%02X:%02X:%02X\", \"limitSwitchActive\": %s}", mac[0], mac[1], mac[2], mac[3], mac[4], mac[5], limitSwitchActive ? "true" : "false");
        snprintf(json, size, "{ \"limitSwitchActive\": %s, \"mac\": \"%s\"}", limitSwitchActive ? "true" : "false", _mac);
        return json;
    }    

} struct_message;




/*
// Callback when data is received
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
    struct_message myData;
    memcpy(&myData, incomingData, sizeof(myData));
    Serial.print("Bytes received: ");
    Serial.println(len);
    Serial.print("Message: ");
    Serial.println(myData.text);
    Serial.print("From MAC: ");
    for (int i = 0; i < 6; i++) {
        Serial.print(myData.mac[i], HEX);
        if (i < 5)Serial.print(":");
    }
    Serial.println();

    // Publish received data to MQTT
    char msg[100];
    snprintf(msg, 100, "Received from ESP-A: %s", myData.text);
    mqttClient.publish(mqttTopic, msg);
}
*/
void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
    struct_message myData;
    memcpy(&myData, incomingData, sizeof(myData));
    char json[100];
    Serial.println(myData.get_json(json, 100));
    // Use the predefined ESP-A MAC address


    // char macStr[18];
    // snprintf(macStr, sizeof(macStr), "%02X:%02X:%02X:%02X:%02X:%02X", 
    //          espAMacAddress[0], espAMacAddress[1], espAMacAddress[2], 
    //          espAMacAddress[3], espAMacAddress[4], espAMacAddress[5]);

    // Get the current timestamp
     // Get the current Unix timestamp
    // unsigned long unixTimestamp = timeClient.getEpochTime();

    // // Publish received data along with ESP-A MAC address and Unix timestamp to MQTT
    // char msg[250];
    // snprintf(msg, 150,  "\"value\": %s, \"mac\": %s, \"unix\": %lu }", myData.limitSwitchActive, macStr, unixTimestamp);
    // printf("msg: %s\n", msg);
    mqttClient.publish(mqttTopic, json);
}





void mqttCallback(char* topic, byte* payload, unsigned int length) {
    payload[length] = 0; // Null-terminate the payload
    String command = String((char*)payload);
    if (command == "sleep" || command == "wake") {
        // Forward the command to ESP-A via ESP-NOW
        esp_now_send(espAMacAddress, payload, length);
    }
}



void initEspNow(){
        // Initialize ESP-NOW
    if (esp_now_init() != 0) {
        Serial.println("Error initializing ESP-NOW");
        return;
    }
    esp_now_set_self_role(ESP_NOW_ROLE_SLAVE);
    esp_now_register_recv_cb(OnDataRecv);
 
}


void initWifi(){

    WiFi.begin(ssid, pass);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("WiFi Connected");
}

void initMQTT(){
    mqttClient.setServer(mqttServer, mqttPort);
    while (!mqttClient.connected()) {
        Serial.println("Connecting to MQTT Broker...");
        if (mqttClient.connect(mqttDeviceId, mqttUser, mqttToken)) {
            Serial.println("MQTT Connected");
            //mqttClient.subscribe(mqttTopicDisplay);
            mqttClient.subscribe(mqttTopicCommand);
        } else {
            Serial.print("Failed to connect to MQTT, rc=");
            Serial.println(mqttClient.state());
            delay(5000);
        }
    }
    
}

void initNTPClient(){
    timeClient.begin();
    timeClient.update();

}


void printMac(){
    // represent the MAC address as a uint8_t array
    uint8_t mac[6];
    WiFi.macAddress(mac);
    // print in following format: {0x08,0x3A,0x8D,0xEF,0x09,0xDB}
    Serial.print("{");
    for (int i = 0; i < sizeof(mac); ++i) {
        Serial.print("0x");
        Serial.print(mac[i], HEX);
        if (i < sizeof(mac)-1) {
            Serial.print(",");
        }
    }
    Serial.println("}");

}


void setup() {
    Serial.begin(115200);
    Serial.setTimeout(2000);
    // Initialize WiFi and MQTT for both first boot and wake from deep sleep
    WiFi.mode(WIFI_STA);
    initWifi();
    // initNTPClient();
    // pinMode(BUTTON_PIN, INPUT_PULLUP);
    //wakeTime = millis();
    // Connect to MQTT broker
    initMQTT();
    // Initialize ESP-NOW
    initEspNow();
    printMac();

}


void reattemptMQTT(){
      while (!mqttClient.connected()) {
        Serial.print("Attempting MQTT connection...");
        if (mqttClient.connect(mqttDeviceId, mqttUser, mqttToken)) {
            Serial.println("MQTT Connected");
            mqttClient.subscribe(mqttTopicDisplay);
        } else {
            Serial.print("Failed to connect to MQTT, rc=");
            Serial.println(mqttClient.state());
            delay(5000);
        }
    }
}

void loop() {
   // if (millis() - wakeTime > wakeDuration) {
     //   Serial.println("Going to sleep");
       // ESP.deepSleep(sleepDuration);
    //}

    mqttClient.loop();

    
    reattemptMQTT();
  

    



    // // Read the state of the push button
    // buttonState = digitalRead(BUTTON_PIN) == LOW; // Assuming the button is active LOW

    // // Send data to MQTT
    // status["buttonPressed"] = buttonState;
    // serializeJson(jsonDoc, msg, sizeof(msg));
    // Serial.println(msg);
    // if (!mqttClient.publish(mqttTopic, msg)) {
    //     Serial.println("MQTT Publish failed");
    // }

    // // Pause but keep polling MQTT
    // for (int i = 0; i < 10; i++) {
    //     mqttClient.loop();
    //     delay(1000);
    // }

}