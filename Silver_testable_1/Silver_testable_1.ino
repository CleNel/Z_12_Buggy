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


bool wifi_check = true;
double distance;
long duration;
int lastTime;
bool go = false;
bool stopped = false;
bool cleared = true;
bool sentStop = false;

int trigT = 0;
int echoT = 1;
double kp=0.030;
double ki=0.000000001;
double kd=0.9;
double Setpoint=200;
double lastError;
double lastError1;
double cumError;
double previousTime;
double previousTime1;
double lastdistance=0;
int speed;
bool pulsed = false;
int pulses=0;
int lastTimeSpeed = 0;
double buggy_speed;
double rel_speed;

void setup() {
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

  //sets what each pin does
  pinMode(LEYE, INPUT);
  pinMode(REYE, INPUT);
  pinMode(H_BRIDGE_LEFT_A, OUTPUT);
  pinMode(H_BRIDGE_LEFT_B, OUTPUT);
  pinMode(H_BRIDGE_RIGHT_A, OUTPUT);
  pinMode(H_BRIDGE_RIGHT_B, OUTPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  pinMode(SMEAS, INPUT_PULLUP);
  attachInterrupt(SMEAS, SMEARED, CHANGE);
  attachInterrupt(ECHO, Echoed, FALLING);
  digitalWrite(TRIG, LOW);
  int lastTime = millis();
  int speed=50;
  double lastError=0;

}

double lastT = 0;
double currentT = 0;
const double circumference = 21;
double velocity = 0;
int SMEARS = 0;
const int velocityUpdateThreshold = 50; // ms
int previousVelocityUpdate = 0;

void updateVelocity () {
  currentT = micros();

  velocity = (1000000 * circumference * SMEARS) / ((currentT - lastT) * 960);
  //send("$"+String(velocity));
  lastT = currentT;
  SMEARS = 0;
}

void SMEARED () {
  SMEARS++;
}

//calculates distance using speed of sound for sonar
double distanceUS(int t) {
  return t * 0.172;
}

//ISR function
void Echoed() {
  //sets echo to current time
  echoT = micros();
  //calibrates distance
  distance = distanceUS(echoT - trigT - 480);
  //Serial.println(String(distance) + "mm");
  //Serial.println(String(echoT - trigT - 480) + "us");
  //runs if distance less than 100mm
  if (distance < 100.0) {
    //doesnt run again if object still there
    if (!sentStop) {
      //stops motor
      stop();
      //send("Stopped: Object detected at " + String(distance)+"mm");
      sentStop = true;
      stopped = true;
      cleared = false;
    }
  } 
  else {
    sentStop = false;
    stopped = false;
    if (!cleared) {
      //send("Cleared");
      cleared = true;
    }
    computePID(distance);
    speed=speed_control_v2(computePID(distance));
  }
}
//function to communicate with client
void send(String dataToSend) {
  if (client) {
    if (client.connected()) {
      client.println(dataToSend);
    }
  } else client = server.available();
}

double computePID(double inp){
  double currentTime=millis();
  double elapsedTime=(double)(currentTime-previousTime);
  double error= inp-Setpoint;
  double cumError=cumError + error*elapsedTime;
  double rateError=(error-lastError)/elapsedTime;
  //double rel_speed=rateError;
  //Serial.println(rel_speed);
  double out = kp*error + ki*cumError + kd*rateError;
  double lastError=error;
  double previousTime=currentTime;
  
  return out;
}

/*
int speed_control(double inp2){
  Serial.println(inp2);
  if (inp2 > 0) {
    if (inp2<7.5){
      return (inp2/7.5)*255;
    }
    else {
      return 255;
    }
  } 
  else if (inp2 < 0) {
    if (inp2>-7.5){
      return (inp2/7.5)*255;
    }
    else {
      return -255;
    }
  }
}
*/
int speed_control_v2(double inp2){
  if (abs(inp2) < 1.0) {
    return 255 * inp2;
  } else return 255 * (abs(inp2)/inp2);
}

void loop() {
  /*
  if(Wifi_status != WL_CONNECTED){
    Serial.println("wifierr");
  }
  */
  if (millis()-previousTime1 > 500){
    rel_speed=(lastdistance-distance)/(previousTime1 -millis());
    lastdistance=distance;
    previousTime1=millis();
    //Serial.println(rel_speed);
  }
  send(String(velocity)+","+String(rel_speed+velocity));
  //Serial.println(String(velocity));
 // Serial.println(String(obj_speed));
  
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
    //low is white
    //if object not in range, ISR not triggered
    if (!stopped) {
      slow_straight();
      
      //if (digitalRead(LEYE) == LOW && digitalRead(REYE) == LOW) //stop();
      //if goes over white line on left, turns right
      if (digitalRead(LEYE) == LOW) {
        soft_right_turn();
        //send("Turn right");
      }
 
      else if (digitalRead(REYE) == LOW) {
        soft_left_turn();
        //send("Turn left");
      }
      
    }

    delay(20);

    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(5);
    trigT = micros();
    digitalWrite(TRIG, LOW);
  } else 
    stop();
}


//Straight
void slow_straight() {
  if (speed>0){
    analogWrite(H_BRIDGE_LEFT_B, 0);
    analogWrite(H_BRIDGE_LEFT_A, speed);
    analogWrite(H_BRIDGE_RIGHT_A, speed);
    analogWrite(H_BRIDGE_RIGHT_B, 0);
  }
  
  if (speed<0){
    analogWrite(H_BRIDGE_LEFT_B, abs(speed));
    analogWrite(H_BRIDGE_LEFT_A, 0);
    analogWrite(H_BRIDGE_RIGHT_A, 0);
    analogWrite(H_BRIDGE_RIGHT_B, abs(speed));
  } 
}


//Right

void soft_right_turn() {
  analogWrite(H_BRIDGE_LEFT_A, 220);
  analogWrite(H_BRIDGE_LEFT_B, 0);
  analogWrite(H_BRIDGE_RIGHT_A, 40);
  analogWrite(H_BRIDGE_RIGHT_B, 0);
  delay(300);
}

//Left

void soft_left_turn() {
  analogWrite(H_BRIDGE_LEFT_B, 0);
  analogWrite(H_BRIDGE_LEFT_A, 40);
  analogWrite(H_BRIDGE_RIGHT_B, 0);
  analogWrite(H_BRIDGE_RIGHT_A, 220);
  delay(300);
}
//stop
void stop() {
  analogWrite(H_BRIDGE_LEFT_B, 0);
  analogWrite(H_BRIDGE_LEFT_A, 0);
  analogWrite(H_BRIDGE_RIGHT_A, 0);
  analogWrite(H_BRIDGE_RIGHT_B, 0);
}
