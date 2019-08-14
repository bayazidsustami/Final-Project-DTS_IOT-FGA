#include <LoRa.h>
#include <SPI.h>
 
#include <AWS_IOT.h>
#include <WiFi.h>
 
// include your personal WiFi and AWS configuration.
#include "config.h"
 
AWS_IOT hornbill;
 
#define ss 26
#define rst 14
#define dio0 2
 
int status = WL_IDLE_STATUS;
int tick=0,msgCount=0,msgReceived = 0;
 
char payload[512];
char rcvdPayload[512];
 
void mySubCallBackHandler (char *topicName, int payloadLen, char *payLoad)
{
    strncpy(rcvdPayload,payLoad,payloadLen);
    rcvdPayload[payloadLen] = 0;
    msgReceived = 1;
}
 
 
 
void setup() {
    Serial.begin(115200);
    while (!Serial);
    delay(500);
    Serial.println("LoRa Receiver");
    //setup LoRa transceiver module
    LoRa.setPins(ss, rst, dio0);
 
    while (!LoRa.begin(922.52E6)) {
      Serial.println(".");
    delay(500);
 
    LoRa.setSyncWord(0xF3);
    Serial.println("LoRa Initializing OK!");
  }
   
    while (status != WL_CONNECTED)
    {
        Serial.print("Attempting to connect to SSID: ");
        Serial.println(WIFI_SSID);
        // Connect to WPA/WPA2 network. Change this line if using open or WEP network:
        status = WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
 
        // wait 5 seconds for connection:
        delay(5000);
    }
 
    Serial.println("Connected to wifi");
 
    if(hornbill.connect(HOST_ADDRESS, CLIENT_ID,
                aws_root_ca_pem, certificate_pem_crt, private_pem_key)== 0)
    {
        Serial.println("Connected to AWS");
        delay(1000);
 
        if(0 == hornbill.subscribe(TOPIC_NAME,mySubCallBackHandler))
        {
            Serial.println("Subscribe Successfull");
        }
        else
        {
            Serial.println("Subscribe Failed, Check the Thing Name and Certificates");
            while(1);
        }
    }
    else
    {
        Serial.println("AWS connection failed, Check the HOST Address");
        while(1);
    }
 
    delay(2000);
 
  }
 
 
void loop() {
  if(msgReceived == 1)
    {
        msgReceived = 0;
        Serial.print("Received Message:");
        Serial.println(rcvdPayload);
    }
   
    // try to parse packet
    int packetSize = LoRa.parsePacket();

    Serial.print(packetSize);
    if (packetSize) {
      // received a packet
      Serial.print("Received packet from lora: ");
      // read packet
      while (LoRa.available()) {
        String LoRaData = LoRa.readString();
        Serial.println(LoRaData);
       
        //creating json 
        //String message = "{ \"default\": \"Banjir\", \"data\": \"" + LoRaData + "\"}";
        
        LoRaData.toCharArray(payload, LoRaData.length()+1);
        //sprintf(payload,"%c", LoRaData);
        if(hornbill.publish(TOPIC_NAME,payload) == 0)
        {        
            Serial.print("Publish Message:");
            Serial.println(payload);
        }
        else
        {
            Serial.println("Publish failed");
        }
      }
     
    }
    delay (1000);
}
