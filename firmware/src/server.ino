#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <wifi_info.h> // comment this out and fill in the below two lines
#include <PubSubClient.h>
#include <HomeAuto.h>

#define LED D2
//const char* ssid = "mywifi";
//const char* password = "";


ESP8266WebServer server(80);
WiFiClient client;
PubSubClient pClient(client);
HomeAuto homeAuto;
//const char* mqtt_server = "home.suyash.io";
const char* mqtt_server = "10.0.0.98";
const char* name = "suyash";
void handleRoot() {
  server.send(200, "text/plain", "hello from esp8266!");
}

void handleNotFound(){

  String message = "File Not Found\n\n";
  message += "URI: ";
  message += server.uri();
  message += "\nMethod: ";
  message += (server.method() == HTTP_GET)?"GET":"POST";
  message += "\nArguments: ";
  message += server.args();
  message += "\n";
  for (uint8_t i=0; i<server.args(); i++){
    message += " " + server.argName(i) + ": " + server.arg(i) + "\n";
  }
  server.send(404, "text/plain", message);

}


void handleLED(char* payloadStr){
  Serial.print(payloadStr);
  if (strcmp(payloadStr, "LED_ON")==0){
    Serial.print("on");
    digitalWrite(LED,HIGH);
  }
  else if(strcmp(payloadStr,"LED_OFF")==0){
    digitalWrite(LED,LOW);
  }
}

void callback(char* topic, byte* payload, unsigned int length) {
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  char payloadStr[length];
  Serial.println(length);
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    payloadStr[i]=(char)payload[i];
  }
  Serial.println();
  if (strcmp(topic,name)==0){
    Serial.print("Stuff Happened");
    digitalWrite(LED,HIGH);
    handleLED(payloadStr);
  }
  else if(strcmp(topic,"suyash/status")==0){
    Serial.print("Status");
    pClient.publish("status","hi");
  }
}

void startWIFI(){
  WiFi.begin(ssid, password);
  Serial.println("");
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

}
void msgCallback(char* topic, byte* payload, unsigned int length) {
  Serial.print("HI Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  char payloadStr[length];
  Serial.println(length);
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    payloadStr[i]=(char)payload[i];
  }
  homeAuto.callHandler(payloadStr);
  Serial.println();
}
int a(){
  Serial.println("hi");
}
int b(){
  Serial.println("hib");
}
void setup(void){
  Serial.begin(115200); // Start serial
  pinMode(LED, OUTPUT); // Set LED pin to output

  startWIFI(); // Config/start wifi

  // Begin local server routes
  server.on("/", handleRoot);

  server.on("/redir", [](){
    server.sendHeader("Location", "http://suyashkumar.com/#"+WiFi.localIP().toString());
    server.send(302, "text/plain", "Location: http://suyashkumar.com/#"+WiFi.localIP());
  });
  server.on("/on", [](){
    digitalWrite(LED, HIGH);
    server.send(200,"text/plain","LED ON");
  });
  server.on("/off",[](){
    digitalWrite(LED,LOW);
    server.send(200,"text/plain","LED OFF");
  });
  // Set up server
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("HTTP server started");
  // Set up MQTT:
  //pClient.setServer(mqtt_server, 1883);
  //pClient.setCallback(callback);
  //homeAuto.setCallback(msgCallback);
  char name[20];
  strcpy(name,"hi");
  homeAuto.addHandler("hi", &a);
  homeAuto.callHandler("hi");
  homeAuto.addHandler("turnLEDOn", &b);
  homeAuto.callHandler("namet");
  homeAuto.setClient(pClient);

}


void loop(void){
  server.handleClient(); // Handle local server clients
  homeAuto.handle();
}