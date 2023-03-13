//wifi set up
#include <WiFiNINA.h>
//connecting to hotspot
char ssid[] = "Galaxy A71DEF2";
char pass[] = "123456788";
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
const int SMEAS = 16;

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
double kp=1.0;
double ki=0.0;
double kd=0.0;
double Setpoint=300;
double lastError;
double cumError;
double previousTime;
int speed;

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
  pinMode(SMEAS, INPUT);
  attachInterrupt(ECHO, Echoed, FALLING);
  digitalWrite(TRIG, LOW);
  int lastTime = millis();
  int speed=50;
  double lastError=0;
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
    //object outside range
    sentStop = false;
    stopped = false;
    if (!cleared) {
      send("Cleared");
      cleared = true;
    }
    //computePID(distance);
    speed=speed_control(computePID(distance));
    //send("time:");
    //send(String(millis()));
    //send("distance:");
    //send(String(distance));
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
  double out = kp*error + ki*cumError + kd*rateError;
  double lastError=error;
  double previousTime=currentTime;
  
  return out;
}
int speed_control(double inp2){
  if (inp2 > 0) {
    return map(inp2, 0, 255, 0, 255); // increase the speed of the motor
    Serial.println(map(inp2, 0, 255, 0, 255));
  } 
  else if (inp2 < 0) {
    return map(inp2, -255, 0, -255, 0); // decrease the speed of the motor
  }
}

void loop() {
  if (digitalRead(SMEAS) == HIGH){
    send("pulse");
  }
  if (client) {
    // Read Data
    if (client.connected()) {
      if (client.available()) data = client.readStringUntil('\n');
    }
  } else client = server.available();

  // Execute Commands
  if (data.length() > 0) {
    //Serial.println(data);
    if (data == "go") go = true;
    else if (data == "stop") go = false;

    // Refresh data
    data = "";    
  }

  if (go) {
    //low is white
    //if object not in range, ISR not triggered
    if (!stopped) {
      slow_straight();
      /*
      if (digitalRead(LEYE) == LOW && digitalRead(REYE) == LOW) //stop();
      //if goes over white line on left, turns right
      //else 
     // if (digitalRead(LEYE) == LOW) {
        //soft_right_turn();
        send("Turn right");
      }
 
      //else if (digitalRead(REYE) == LOW) {
        //soft_left_turn();
        send("Turn left");
      }
    */      
    }

    delay(20);

    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(5);
    trigT = micros();
    digitalWrite(TRIG, LOW);
  } else stop();
}


//Straight
void slow_straight() {
  analogWrite(H_BRIDGE_LEFT_B, 0);
  analogWrite(H_BRIDGE_LEFT_A, speed);
  analogWrite(H_BRIDGE_RIGHT_A, speed);
  analogWrite(H_BRIDGE_RIGHT_B, 0);
}

//Right

void soft_right_turn() {
  analogWrite(H_BRIDGE_LEFT_A, abs(speed));
  analogWrite(H_BRIDGE_LEFT_B, 0);
  analogWrite(H_BRIDGE_RIGHT_A, 10);
  analogWrite(H_BRIDGE_RIGHT_B, 0);
  delay(300);
}

//Left

void soft_left_turn() {
  analogWrite(H_BRIDGE_LEFT_B, 0);
  analogWrite(H_BRIDGE_LEFT_A, 10);
  analogWrite(H_BRIDGE_RIGHT_B, 0);
  analogWrite(H_BRIDGE_RIGHT_A, abs(speed));
  delay(300);
}
//stop
void stop() {
  analogWrite(H_BRIDGE_LEFT_B, 0);
  analogWrite(H_BRIDGE_LEFT_A, 0);
  analogWrite(H_BRIDGE_RIGHT_A, 0);
  analogWrite(H_BRIDGE_RIGHT_B, 0);
}
