#include <WiFiNINA.h>
char ssid[] = "Redmi 10";
char pass[] = "buggycode";
WiFiServer server(5200);
WiFiClient client;
int wifi_status = WL_IDLE_STATUS;
String data = "";

const int LEYE = 2;
const int REYE = 3;
const int TRIG = 12;
const int ECHO = 11;  //12;

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


void setup() {
  Serial.begin(9600);
  /*
  WiFi.beginAP(ssid, pass);
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address:");
  Serial.println(ip);
  server.begin();
  */

  String firmware_version = WiFi.firmwareVersion();


  while (wifi_status != WL_CONNECTED) {
    wifi_status = WiFi.begin(ssid, pass);
    Serial.println("Connecting");
    delay(1000);
  }

  server.begin();

  IPAddress ip = WiFi.localIP();
  Serial.println("IP: " + String(ip[0]) + "." + String(ip[1]) + "." + String(ip[2]) + "." + String(ip[3]));


  pinMode(LEYE, INPUT);
  pinMode(REYE, INPUT);
  pinMode(H_BRIDGE_LEFT_A, OUTPUT);
  pinMode(H_BRIDGE_LEFT_B, OUTPUT);
  pinMode(H_BRIDGE_RIGHT_A, OUTPUT);
  pinMode(H_BRIDGE_RIGHT_B, OUTPUT);
  pinMode(TRIG, OUTPUT);
  pinMode(ECHO, INPUT);
  attachInterrupt(ECHO, Echoed, FALLING);


  digitalWrite(TRIG, LOW);
  int lastTime = millis();
}

double distanceUS(int t) {
  return t * (0.343 / 2);
}

void Echoed() {
  echoT = micros();
  distance = distanceUS(echoT - trigT);
  Serial.println(distance);
  if (distance < 200.0) {
    if (!sentStop) {
      stop();
      send("Object");
      sentStop = true;
      stopped = true;
      cleared = false;
    }
  } else {
    sentStop = false;
    stopped = false;
    if (!cleared) {
      send("Cleared");
      cleared = true;
    }
  }
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
      else data = "";
    } else data = "";
  } else client = server.available();

  // Execute Commands
  if (data.length() > 0) {
    if (data == "go") go = true;
    else if (data == "stop") go = false;
  }

  if (go) {
    //low is white
    //char command=client.read();
    if (!stopped) {
      slow_straight();
      if (digitalRead(LEYE) == LOW) {
        soft_right_turn();
        client.write("Turn right");
      }
      //delay(50);
      if (digitalRead(REYE) == LOW) {
        soft_left_turn();
        client.write("Turn left");
      }      
    }

    delay(20);

    digitalWrite(TRIG, LOW);
    delayMicroseconds(2);
    digitalWrite(TRIG, HIGH);
    delayMicroseconds(5);
    trigT = micros();
    digitalWrite(TRIG, LOW);

    /*
    int currentTime=millis();
    if (currentTime-lastTime>500){
        digitalWrite(TRIG, LOW);
        delayMicroseconds(2);
        digitalWrite(TRIG, HIGH);
        delayMicroseconds(10);
        digitalWrite(TRIG, LOW);
        duration = pulseIn(ECHO, HIGH);
        distance = duration/58;
        int lastTime=millis();
      }

      if (distance<10){
        if (!sentStop) {
          sentStop = true;
          stop();
          delay(500);
          client.write("Stopped");
          stopped=true;
        }
      } else sentStop = false;
    }
    */
  }
}


//Straight
void slow_straight() {
  analogWrite(H_BRIDGE_LEFT_B, 0);
  analogWrite(H_BRIDGE_LEFT_A, 100);
  analogWrite(H_BRIDGE_RIGHT_A, 100);
  analogWrite(H_BRIDGE_RIGHT_B, 0);
  if (stopped == true) {
    stopped = false;
    client.write("moving again");
  }
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
