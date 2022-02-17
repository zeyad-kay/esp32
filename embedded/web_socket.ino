
#include <Arduino.h>
#include <string.h>

#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
 
WebSocketsClient webSocket;
 
const char *ssid     = "Etisalat-5CY7";
const char *password = "20061998";
 
unsigned long messageInterval = 5000;
bool connected = false;
#define MQ2pin (A0)
  
int flag=-1; 
#define DEBUG_SERIAL Serial

#include "DHT.h"
#define DHTPIN D1
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);


void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
        case WStype_DISCONNECTED:
            DEBUG_SERIAL.printf("Disconnected!\n");
            connected = false;
            break;
        case WStype_CONNECTED: {
            connected = true;
            DEBUG_SERIAL.println("Connected");
        }
            break;
        case WStype_TEXT:
        DEBUG_SERIAL.printf("[WSc] text RESPONSE: %s\n", payload);
            if(!strcmp((char *)payload,"temperature")) 
            flag = 0;

            if(!strcmp((char *)payload,"gas")) 
            flag =1;
        break; 
        case WStype_BIN:
            DEBUG_SERIAL.printf("[WSc] get binary length: %u\n", length);
             hexdump(payload, length);
            break;
                case WStype_PING:
                        // pong will be send automatically
                        DEBUG_SERIAL.printf("[WSc] get ping\n");
                        break;
                case WStype_PONG:
                        // answer to a ping we send
                        DEBUG_SERIAL.printf("[WSc] get pong\n");
                        break;
    }
 
}

void setup() {
    DEBUG_SERIAL.begin(115200);
 
//  DEBUG_SERIAL.setDebugOutput(true);
 
    DEBUG_SERIAL.println();
    DEBUG_SERIAL.println();
    DEBUG_SERIAL.println();
 
    for(uint8_t t = 4; t > 0; t--) {
        DEBUG_SERIAL.printf("[SETUP] BOOT WAIT %d...\n", t);
        DEBUG_SERIAL.flush();
        delay(1000);
    }
 
    WiFi.begin(ssid, password);
 
    while ( WiFi.status() != WL_CONNECTED ) {
      delay ( 500 );
      Serial.print ( "." );
    }        
    DEBUG_SERIAL.print("Local IP: "); DEBUG_SERIAL.println(WiFi.localIP());
    // server address, port and URL
    webSocket.begin("esp32-ws.herokuapp.com", 80, "/sensor","text");
 
    // event handler
    webSocket.onEvent(webSocketEvent);
}
 
unsigned long lastUpdate = millis();

void loop() {
    
    webSocket.loop();
    if (connected && lastUpdate+messageInterval<millis()){
        if (flag==1)
        {
        int gas = analogRead(MQ2pin);
        char gasc[5];
        itoa(gas, gasc,10);
        Serial.print("gas");
        //String a = "{gas: 456}";
        webSocket.sendTXT( gasc);
        }  
        else if (flag == -1)
        {}
        else
        {
        float temp = dht.readTemperature();
        char tempc[5];
        itoa(temp, tempc,10);
        Serial.print("gas");
        Serial.print(temp);
        String b = "{temperature: 123}";
        webSocket.sendTXT(tempc);
        }  
        lastUpdate = millis();
    }
}
