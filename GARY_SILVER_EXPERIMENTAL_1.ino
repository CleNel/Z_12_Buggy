#include <WiFiNINA.h>
char ssid[] = "Redmi 10";
char pass[] = "buggycode";
WiFiServer server(5200);
WiFiClient client;
int wifi_status = WL_IDLE_STATUS;
String data = "";

const int H_BRIDGE_LEFT_A = 9;
const int H_BRIDGE_LEFT_B = 10;
const int H_BRIDGE_RIGHT_A = 5;
const int H_BRIDGE_RIGHT_B = 6;

bool wifi_check = true;
bool f = false;
bool b = false;
bool l = false;
bool r = false;

void setup() {
  Serial.begin(9600);
  String firmware_version = WiFi.firmwareVersion();


  while (wifi_status != WL_CONNECTED) {
    wifi_status = WiFi.begin(ssid, pass);
    Serial.println("Connecting");
    delay(1000);
  }

  server.begin();

  IPAddress ip = WiFi.localIP();
  Serial.println("IP: " + String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." + String(ip[3]));

  pinMode(H_BRIDGE_LEFT_A, OUTPUT);
  pinMode(H_BRIDGE_LEFT_B, OUTPUT);
  pinMode(H_BRIDGE_RIGHT_A, OUTPUT);
  pinMode(H_BRIDGE_RIGHT_B, OUTPUT);

}

void send(String dataToSend) {
  if (client) {
    if (client.connected()) {
      client.println(dataToSend);
    }
  } else client = server.available();
}

void loop() {

  if (client) {
    // Read Data
    if (client.connected()) {
      if (client.available()) data = client.readStringUntil('\n');
    }
  } else client = server.available();

  // Execute Commands
  if (data.length() > 0) {
    //Serial.println(data);
    if (data == "f") f = true;
    else if (data == "b") b = false;
    else if (data == "l") l = true;
    else if (data == "r") r = true;

    if(f){
      slow_straight();
    }

    if(b){
      reverse();
    }

    if(l){
      soft_left_turn();
    }

    if(r){
      soft_right_turn();
    }

    // Refresh data
    data = "";    
  }

  
  
}


//Straight
void slow_straight() {
  analogWrite(H_BRIDGE_LEFT_B, 0);
  analogWrite(H_BRIDGE_LEFT_A, 100);
  analogWrite(H_BRIDGE_RIGHT_A, 100);
  analogWrite(H_BRIDGE_RIGHT_B, 0);
}

//Reverse
void reverse() {
  analogWrite(H_BRIDGE_LEFT_B, 100);
  analogWrite(H_BRIDGE_LEFT_A, 0);
  analogWrite(H_BRIDGE_RIGHT_A, 0);
  analogWrite(H_BRIDGE_RIGHT_B, 100);
}

//Right

void soft_right_turn() {
  analogWrite(H_BRIDGE_LEFT_A, 220);
  analogWrite(H_BRIDGE_LEFT_B, 0);
  analogWrite(H_BRIDGE_RIGHT_A, 100);
  analogWrite(H_BRIDGE_RIGHT_B, 0);
  delay(300);
}

//Left

void soft_left_turn() {
  analogWrite(H_BRIDGE_LEFT_B, 0);
  analogWrite(H_BRIDGE_LEFT_A, 100);
  analogWrite(H_BRIDGE_RIGHT_B, 0);
  analogWrite(H_BRIDGE_RIGHT_A, 220);
  delay(300);
}

void stop() {
  analogWrite(H_BRIDGE_LEFT_B, 0);
  analogWrite(H_BRIDGE_LEFT_A, 0);
  analogWrite(H_BRIDGE_RIGHT_A, 0);
  analogWrite(H_BRIDGE_RIGHT_B, 0);
}
