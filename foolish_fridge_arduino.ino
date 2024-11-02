#include <ArduinoHttpClient.h>
#include <WiFiS3.h>
#include "settings.h"

char ssid[] = SECRET_SSID;
char password[] = SECRET_PASSWORD;

char serverAdress[] = "192.168.0.128";  //PC
//char serverAdress[] = "192.168.0.205";  //Laptop
int port = 8080;

WiFiClient wifi;
HttpClient client = HttpClient(wifi, serverAdress, port);

int obstacleSensorPin  = A0;
int doorOpenTime = 0;
int internalArduinoCounter= 0;
int detect = 50;
bool doorOpen =false;
bool warning = false;

String warningString = "2";
String doorOpenTimeString = "0";


void sendData(){
  String foolishFridgeData = "{\"warning\":" + warningString + ",\"doorOpenTime\":" + doorOpenTimeString + "}";
  client.beginRequest();
  client.post("/FoolishFridge");

  client.sendHeader("Content-Type", "application/json");
  client.sendHeader("Content-Length", foolishFridgeData.length());

  client.beginBody();
  client.print(foolishFridgeData);
  client.endRequest();

  int statusCode = client.responseStatusCode();
  String response = client.responseBody();
  Serial.println("Detta skickas: ");
  Serial.println(foolishFridgeData);

}


void setup() {
  Serial.begin(9600);

  Serial.println("Ansluter till wifi...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    Serial.println("...");
    delay(500);
  }
  Serial.println("Ansluten till wifi");

  pinMode(obstacleSensorPin, INPUT);
}

void loop() {

  int obstacleSensorPinValue = analogRead(obstacleSensorPin);
  Serial.print("Jag är värdet på sensorn: ");
  Serial.println(obstacleSensorPinValue);


  if(obstacleSensorPinValue < detect && doorOpen == false){
    Serial.println("|      DÖRREN ÖPPNAS       |");
    Serial.print("Värdet på sensorn: ");
    Serial.println(obstacleSensorPinValue);
    internalArduinoCounter = internalArduinoCounter+2000;
    doorOpen = true;
    doorOpenTime = doorOpenTime+ 2000;
  } else if (obstacleSensorPinValue < detect && doorOpen == true){
    Serial.println("|   DÖRREN ÄR FORTFARANDE ÖPPEN  |");
    Serial.println();
    Serial.print("Värdet på sensorn: ");
    Serial.println(obstacleSensorPinValue);
    Serial.print("Värdet på intern räknare: ");
    Serial.println(internalArduinoCounter);
    Serial.print("Sparad tid: ");
    Serial.println(doorOpenTime);
    Serial.print("Warning: ");
    Serial.println(warning);
    Serial.println();
    doorOpenTime = doorOpenTime+ 2000;
    internalArduinoCounter = internalArduinoCounter+2000;

    if (internalArduinoCounter >= 10000){
    warning = true;
    Serial.println("|   !!!!!!!PANIC!!!!!!!!!  |");
    Serial.println();
    Serial.print("Värdet på sensorn: ");
    Serial.println(obstacleSensorPinValue);
    Serial.print("Värdet på intern räknare: ");
    Serial.println(internalArduinoCounter);
    Serial.print("Sparad tid: ");
    Serial.println(doorOpenTime);
    Serial.print("Warning: ");
    Serial.println(warning);
    Serial.println();
    doorOpenTime = doorOpenTime+ 2000;
    internalArduinoCounter = 2000;

    warningString = String(warning);
    doorOpenTimeString = String(doorOpenTime);
    delay(300);

    sendData();
    delay(500);

    }


  }
    if(obstacleSensorPinValue > detect && doorOpen == false){
    Serial.println("DÖRREN FORTFARANDE STÄNGD");

  } else if(obstacleSensorPinValue > detect && doorOpen == true){
    Serial.println("DÖRREN STÄNGS");
    if (warning == true){
    warningString = String(2);

    } else {
      warningString = String(warning);
    }

    doorOpenTimeString = String(doorOpenTime);
    delay(300);

    sendData();
    delay(500);

    warning = false;
    doorOpen=false;
    internalArduinoCounter =0;
    doorOpenTime=0;
  }


 
  delay(2000);
  Serial.println();

}