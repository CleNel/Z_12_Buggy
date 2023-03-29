#include <Arduino_LSM6DS3.h>
//wifi set up
#include <WiFiNINA.h>
//connecting to hotspot
char ssid[] = "Redmi 10";
char pass[] = "buggycode";
//port of connection
WiFiServer server(5200);
WiFiClient client;
int wifi_status = WL_IDLE_STATUS;
String data = "";
//assigning sensor pins
const int LEYE = 2;
const int REYE = 3;
const int TRIG = 12;
const int ECHO = 11; 
const int SMEAS = A1;

const int H_BRIDGE_LEFT_A = 9;
const int H_BRIDGE_LEFT_B = 10;
const int H_BRIDGE_RIGHT_A = 5;
const int H_BRIDGE_RIGHT_B = 6;

const double circumference = 21;
double velocity = 0;
const int velocityUpdateThreshold = 50; // ms
int previousVelocityUpdate = 0;
bool wifi_check = true;
double speed=0;
double x=0;
double y=0;
double angle = 0;
float gX, gY, gZ;
int SMEARS = 0;
double lastT = 0;
double currentT = 0;
bool go = false;
bool stopped = false;


void setup() {
  // put your setup code here, to run once:
  //frequency of connection
  Serial.begin(9600);
  String firmware_version = WiFi.firmwareVersion();

  //connects to hotspot
  while (wifi_status != WL_CONNECTED) {
    wifi_status = WiFi.begin(ssid, pass);
    Serial.println("Connecting");
    delay(1000);
  }
  //starts server
  server.begin();
  //prints ip address of server
  IPAddress ip = WiFi.localIP();
  Serial.println("IP: " + String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." + String(ip[3]));

  pinMode(H_BRIDGE_LEFT_A, OUTPUT);
  pinMode(H_BRIDGE_LEFT_B, OUTPUT);
  pinMode(H_BRIDGE_RIGHT_A, OUTPUT);
  pinMode(H_BRIDGE_RIGHT_B, OUTPUT);

}



void loop() {
  // put your main code here, to run repeatedly:

  //location();





  if (client) {
    // Read Data
    if (client.connected()) {
      if (client.available()) data = client.readStringUntil('\n');
    }
  } else client = server.available();

  // Execute Commands
  if (data.length() > 0) {
    //Serial.println(data);
    if (data == "go"){
      go = true;
    } 
    else if (data == "stop"){
      go = false;
    }

    // Refresh data
    data = "";    
  }

  // Update Velocity
  if (millis() - previousVelocityUpdate > velocityUpdateThreshold) {
    updateVelocity();
    previousVelocityUpdate = millis();
  }

  if (go) {
    if (!stopped) {
      if (data == "UP"){
        slow_straight();
      }
    }
  } else 
    stop();
}
/*
void angle(){
  angle= angle + (gZ*angle_time);
}
*/
void SMEARED () {
  SMEARS++;
}
/*
void location(){
  x=cosine(angle())*velocity*go_time;
  y=sine(angle())*velocity*go_time;
}
*/
void updateVelocity () {
  currentT = micros();

  velocity = (1000000 * circumference * SMEARS) / ((currentT - lastT) * 960);
  //send("$"+String(velocity));
  lastT = currentT;
  SMEARS = 0;
}


void send(String dataToSend) {
  if (client) {
    if (client.connected()) {
      client.println(dataToSend);
    }
  } else client = server.available();
}


void slow_straight() {
  if (speed>0){
    analogWrite(H_BRIDGE_LEFT_B, 0);
    analogWrite(H_BRIDGE_LEFT_A, speed);
    analogWrite(H_BRIDGE_RIGHT_A, speed);
    analogWrite(H_BRIDGE_RIGHT_B, 0);
  }
  
  if (speed<0){
    analogWrite(H_BRIDGE_LEFT_B, 0);
    analogWrite(H_BRIDGE_LEFT_A, 0);
    analogWrite(H_BRIDGE_RIGHT_A, 0);
    analogWrite(H_BRIDGE_RIGHT_B, 0);
  } 
}

//right
void soft_right_turn() {
  double time_start_right=0;
  analogWrite(H_BRIDGE_LEFT_A, 225);
  analogWrite(H_BRIDGE_LEFT_B, 0);
  analogWrite(H_BRIDGE_RIGHT_A, 40);
  analogWrite(H_BRIDGE_RIGHT_B, 0);
  double time_taken_right = micros()-time_start_right;
  IMU.readGyroscope(gX, gY, gZ);
  angle = gZ*time_taken_right*1000;
}
//Left
void soft_left_turn() {
  double time_start_left=0;
  analogWrite(H_BRIDGE_LEFT_B, 0);
  analogWrite(H_BRIDGE_LEFT_A, 40);
  analogWrite(H_BRIDGE_RIGHT_B, 0);
  analogWrite(H_BRIDGE_RIGHT_A, 225);
  double time_taken_left = micros()-time_start_left;
  IMU.readGyroscope(gX, gY, gZ);
  angle = gZ*time_taken_left*1000;
}
void stop() {
  analogWrite(H_BRIDGE_LEFT_B, 0);
  analogWrite(H_BRIDGE_LEFT_A, 0);
  analogWrite(H_BRIDGE_RIGHT_A, 0);
  analogWrite(H_BRIDGE_RIGHT_B, 0);
}