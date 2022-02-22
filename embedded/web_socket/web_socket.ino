
#include <Arduino.h>
#include <string.h>

#include <ESP8266WiFi.h>
#include <WebSocketsClient.h>
 
WebSocketsClient webSocket;
 


unsigned long messageInterval = 1000;
bool connected = false;
#define MQ2pin (A0)
  
int temp_gas_toggle=-1; 
#include "DHT.h"
#define DHTPIN D1
#define DHTTYPE DHT11

DHT dht(DHTPIN, DHTTYPE);


void webSocketEvent(WStype_t type, uint8_t * payload, size_t length) {
    switch(type) {
            break;
        case WStype_TEXT:
            if(!strcmp((char *)payload,"temperature")) 
            temp_gas_toggle = 0;

            if(!strcmp((char *)payload,"gas")) 
            temp_gas_toggle =1;
        break; 
        case WStype_BIN:
             hexdump(payload, length);
    }
 
}

void connect_wifi() {
    Serial.println("Please enter the username: ");
    while(Serial.available()==0)
    {}
    String ssid =Serial.readString();
    char str_array[ssid.length()];
    ssid.toCharArray(str_array, ssid.length());
    char* username = strtok(str_array, " ");

    Serial.println("Please enter the password: ");
    while(Serial.available()==0)
    {}
    String password_string =Serial.readString();
    char str_array2[password_string.length()];
    password_string.toCharArray(str_array2, password_string.length());
    char* password = strtok(str_array2, " ");
    
    WiFi.begin(username, password);

    int i=0;
    while( WiFi.status() != WL_CONNECTED ) {
      i++;
      delay ( 500 );
      if (i>20) break;
      Serial.print ( "." );
    }
       if (i<20) break;
    }
}

void print_networks()
{
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

void setup() {
    Serial.begin(115200); 
    for(uint8_t t = 4; t > 0; t--) {
        Serial.printf("Pease wait for %d seconds\n", t);
        Serial.flush();
        delay(1000);
    }
    while(1)
    {
    print_networks();
    connect_wifi();
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
        if (temp_gas_toggle==1)
        {
        int gas = analogRead(MQ2pin);
        char gasc[5];
        itoa(gas, gasc,10);
        Serial.print(gas);
        //String a = "{gas: 456}";
        webSocket.sendTXT( gasc);
        }  
        else if (temp_gas_toggle == -1)
        {}
        else
        {
        int temp = dht.readTemperature();
        char tempc[5];
        itoa(temp, tempc,10);

        Serial.print(temp);
        String b = "{temperature: 123}";
        webSocket.sendTXT(tempc);
        }
        lastUpdate = millis();
    }
}
