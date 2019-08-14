#include <SPI.h>
#include <LoRa.h>


//define the pins used by the transceiver module
#define ss 5
#define rst 14
#define dio0 2

const int trigPin = 12;
const int echoPin = 13;

void setup() {
    //initialize Serial Monitor
    pinMode(trigPin, OUTPUT); // Sets the trigPin as an Output
    pinMode(echoPin, INPUT); // Sets the echoPin as an Input
    
    Serial.begin(115200);
    while (!Serial);
    Serial.println("LoRa Sender");
    
    LoRa.setPins(ss, rst, dio0);
    
    while (!LoRa.begin(922.52E6)) {
      Serial.println(".");
    delay(500);
  }
 
  LoRa.setSyncWord(0xF3);
  Serial.println("LoRa Initializing OK!");
 
}

void loop() {

    //Sending a LOW pulse to clear the trigPin 
    //and sets the trigger to a HIGH pulse for 10 microseconds
    digitalWrite(trigPin, LOW);
    delayMicroseconds(5);
    digitalWrite(trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(trigPin, LOW);


    // Reads the echoPin, returns the sound wave travel time in microseconds
    float duration = pulseIn(echoPin, HIGH);

    // Calculating the distance
    float distance= (duration/2) / 29.1;

    Serial.print("Sending packet: ");
    //Serial.print("Ketinggian Air : ");
    //Serial.println(distance);

    String myString = String(distance);

    String message = "{ \"Device_ID\": \"ESP32\", \"Distance\": \"" + myString + "\", \"Location\": \"No Location\"}";

    Serial.print("JSON Message =");
    Serial.println(message);
    
    //Send LoRa packet to receiver
    LoRa.beginPacket();
    LoRa.print(message);
    LoRa.endPacket();
    delay(10000);
}
