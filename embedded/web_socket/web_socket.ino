
#include <Arduino.h>
#include <string.h>
#include "DHT.h"
#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
 
#define DHTPIN D1
#define DHTTYPE DHT11
#define MQ2pin (A0)

WebSocketsClient webSocket;

unsigned long messageInterval = 1000;
bool connected = false;
int mode = -1; 

DHT dht(DHTPIN, DHTTYPE);


void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_TEXT:
            if(!strcmp((char *)payload,"temperature")) 
            mode = 0;

            if(!strcmp((char *)payload,"gas")) 
            mode = 1;
            break; 
        case WStype_BIN:
            hexdump(payload, length);
            break;
    }
 
}

void serial_tochar(char * out) {
    while(Serial.available()==0) { }
    String str =Serial.readString();
    char char_array[str.length()];
    str.toCharArray(char_array, str.length());
    out = strtok(char_array, " ");
}

void connect_wifi() {
    char * username;
    Serial.println("Please enter the username: ");
    serial_tochar(username);

    char * password;
    Serial.println("Please enter the password: ");
    serial_tochar(password);

    WiFi.begin(username, password);

    uint8_t i = 0;
    while(WiFi.status() != WL_CONNECTED && i < 20) {
      delay(500);
      Serial.print( "." );
      i++;
    }
    return WiFi.status() == WL_CONNECTED;
}

void print_networks() {
    int networks_number = WiFi.scanNetworks();
    for(int i=0;i<networks_number;i++) 
    {
      Serial.print("ssid: ");
      Serial.print(WiFi.SSID(i));
      Serial.print(", strength: ");
      Serial.print(WiFi.RSSI(i));
      Serial.print("\n");
    }
}

void send_temperature() {
    int temp = dht.readTemperature();
    char tempc[5];
    itoa(temp, tempc,10);
    Serial.print(temp);
    String b = "{temperature: 123}";
    webSocket.sendTXT(tempc);
}

void send_gas() {
    int gas = analogRead(MQ2pin);
    char gasc[5];
    itoa(gas, gasc,10);
    Serial.print(gas);
    //String a = "{gas: 456}";
    webSocket.sendTXT( gasc);
}

void setup() {
    Serial.begin(115200); 
    for(uint8_t t = 4; t > 0; t--) {
        Serial.printf("Pease wait for %d seconds\n", t);
        Serial.flush();
        delay(1000);
    }
    while(!connected)
    {
        print_networks();
        connected = connect_wifi();
    }
    Serial.print("Local IP: "); Serial.println(WiFi.localIP());
    // server address, port and URL
    webSocket.begin("esp32-ws.herokuapp.com", 80, "/sensor","text");
 
    // event handler
    webSocket.onEvent(webSocketEvent);
}
 
unsigned long lastUpdate = millis();

void loop() {
    webSocket.loop();
    if (connected && lastUpdate+messageInterval<millis()){
        if (mode == 1)
        {
            send_gas();
        }  
        else if(mode == 0)
        {
            send_temperature();
        }
        lastUpdate = millis();
    }
}
