
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

void setup() {
    Serial.begin(115200); 
    for(uint8_t t = 4; t > 0; t--) {
        Serial.printf("Pease wait for %d seconds\n", t);
        Serial.flush();
        delay(1000);
    }
    while(1)
    {
     int networks_number = WiFi.scanNetworks();
    for(int counter1=0;counter1<networks_number;counter1++) 
    {
      Serial.println(WiFi.SSID(counter1));
      Serial.println(WiFi.RSSI(counter1));
    }
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
    int counter2=0;
    while ( WiFi.status() != WL_CONNECTED ) {
      counter2++;
      delay ( 500 );
      if (counter2>20)
      { break;}
      Serial.print ( "." );
    }
       if (counter2<20)
      { break;}
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
