#include <ESP8266WiFi.h>
#include <espnow.h>


// GPIO pin where the limit switch is connected
const int limitSwitchPin = 5; // Example GPIO pin
int counter = 0;
bool prevLimitSwitchState = false;
bool hasPeer = false;
// uint8_t receiverMac[] = {0x08,0x3A,0x8D,0xEF,0x09,0xDB}; 
uint8_t receiverMac[] = {0x50,0x2,0x91,0x78,0x6,0xDE};


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


struct_message myData;

// Callback when data is sent
void OnDataSent(uint8_t *mac_addr, uint8_t sendStatus) {
    counter += 1;
    if (sendStatus == 0) {
        // Serial.println("Delivery success");
    } else {
        // Serial.println("Delivery fail");
    }
}

void OnDataRecv(uint8_t * mac, uint8_t *incomingData, uint8_t len) {
    // Handle received data from any ESP
    struct_message *receivedData = (struct_message *)incomingData;
    // Process the received data as needed
}

void setupEspNow(){
    WiFi.mode(WIFI_STA);
    if (esp_now_init () != 0) {
        Serial.println("*** ESP_Now init failed");
        ESP.restart();
    }
    esp_now_set_self_role(ESP_NOW_ROLE_CONTROLLER);
    esp_now_register_send_cb(OnDataSent);
    esp_now_register_recv_cb(OnDataRecv);
}




void setup() {
    Serial.begin(115200);
    pinMode(limitSwitchPin, INPUT_PULLUP); // Set the limit switch as an input

    // Set device as a Wi-Fi Station
    // WiFi.mode(WIFI_STA);
    setupEspNow();
    WiFi.macAddress(myData.mac);
}


bool updateLimitSwitch(){
    bool shouldUpdate = false;
    bool currentLimitSwitchState = digitalRead(limitSwitchPin) == HIGH; // Assuming the limit switch is active LOW
    if (currentLimitSwitchState != prevLimitSwitchState){
        myData.limitSwitchActive = currentLimitSwitchState;
        // char json[100];
        // Serial.println(myData.get_json(json, sizeof(json)));
        shouldUpdate = true;
    }
    prevLimitSwitchState = currentLimitSwitchState;

    return shouldUpdate;
}


bool checkAndConnectToBridge(){
    if (hasPeer){
        return true;
    }
   // Replace with ESP B's MAC address
    hasPeer = esp_now_add_peer(receiverMac, ESP_NOW_ROLE_SLAVE, 1, NULL, 0) == 0;
    Serial.println(hasPeer ? "Added Peer" : "Failed to add peer");
    return hasPeer;
}


void transmitSwitchState(){
    // Send data to ESP B
    if (hasPeer){
        esp_now_send(receiverMac, (uint8_t *) &myData, sizeof(myData));
    }
}



void loop() {
    bool hasUpdated = updateLimitSwitch();
    if (hasUpdated){
        checkAndConnectToBridge();
        transmitSwitchState();
        char json[100];
        Serial.println(myData.get_json(json, sizeof(json)));
    }
    delay(200);



    // if (currentLimitSwitchState != limitSwitchActive) {
    //     // Limit switch state changed
    //     myData.limitSwitchActive = currentLimitSwitchState;
    //     // strcpy(myData.text, myData.limitSwitchActive ? "Limit Switch Active" : "Limit Switch Inactive");

    //     // Send data to ESP B
    //     uint8_t receiverMac[] = {0x08,0x3A,0x8D,0xEF,0x09,0xDB}; // Replace with ESP B's MAC address
    //     esp_now_add_peer(receiverMac, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
    //     esp_now_send(receiverMac, (uint8_t *) &myData, sizeof(myData));

    //     limitSwitchActive = currentLimitSwitchState;
    // }
    // static bool lastLimitSwitchState = !currentLimitSwitchState;



    // if (currentLimitSwitchState != lastLimitSwitchState) {
    //     // Limit switch state changed
    //     myData.limitSwitchActive = currentLimitSwitchState;
    //     // strcpy(myData.text, myData.limitSwitchActive ? "Limit Switch Active" : "Limit Switch Inactive");

    //     // Send data to ESP B
    //     uint8_t receiverMac[] = {0x08,0x3A,0x8D,0xEF,0x09,0xDB}; // Replace with ESP B's MAC address
    //     esp_now_add_peer(receiverMac, ESP_NOW_ROLE_SLAVE, 1, NULL, 0);
    //     esp_now_send(receiverMac, (uint8_t *) &myData, sizeof(myData));

    //     lastLimitSwitchState = currentLimitSwitchState;
    // }

    // Other code for sleep management, etc.
}