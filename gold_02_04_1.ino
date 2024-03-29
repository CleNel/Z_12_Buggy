//#include <WiFiNINA.h>
//char ssid[] = "Redmi10";
//char pass[] = "buggycode";
//WiFiServer server(5200);
//WiFiClient client;
//int wifi_status = WL_IDLE_STATUS;
String data = "";

const int H_BRIDGE_LEFT_A = 9;
const int H_BRIDGE_LEFT_B = 10;
const int H_BRIDGE_RIGHT_A = 5;
const int H_BRIDGE_RIGHT_B = 6;

//bool wifi_check = true;
bool f = false;
bool b = false;
bool l = false;
bool r = false;
bool s = false;
unsigned long start_time, function_time;
double time_elap = 0;
double right_time = 0;
double left_time = 0;
double rev_time = 0;
double stop_time = 0;
double straight_time = 0;
int speed;
double xcoord;
double ycoord;
double angle;
void setup() {
  Serial.begin(9600);
  while (!Serial) {
    ; 
  }
  pinMode(H_BRIDGE_LEFT_A, OUTPUT);
  pinMode(H_BRIDGE_LEFT_B, OUTPUT);
  pinMode(H_BRIDGE_RIGHT_A, OUTPUT);
  pinMode(H_BRIDGE_RIGHT_B, OUTPUT);
  speed=100;
  xcoord = 0.0;
  ycoord = 0.0;
  angle = 0.01745329*90;
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
void loop() {
  float gZ;
  if (Serial.available() > 0){
    data=Serial.readStringUntil('\n');
    if (data.length() > 0) {
      //reverse();
      //Serial.println(data);
      if (data == "f"){
        f = true;
        b=false;
        l=false;
        r=false;
        s = false;
      } 
      else if (data == "b") {
        b = true;
        f = false;
        l=false;
        r=false;
        s=false;
        
      }
      else if (data == "l") {
        l = true;
        b=false;
        f=false;
        r = false;
        s=false;
      }
      else if (data == "r") {
        r = true;
        l=false;
        f=false;
        b=false;
        s=false;
      }
      else if (data == "s") {
        s = true;
        l=false;
        f=false;
        b=false;
        r=false;
      }
      else if (data == "v"){
        speed=speed+10;
      }
      //b = true;
      if(f){
        start_time = micros();
        slow_straight();
        xcoord=xcoord+velocity*cos(0.01745329*angle)*time;
        ycoord=ycoord+velocity*sin(0.01745329*angle)*time;
        function_time = micros();
        time_elap = function_time - start_time;
        straight_time = straight_time + time_elap;
      }
      if(s){
        start_time = micros();
        stop();
        function_time = micros();
        time_elap = function_time - start_time;
        stop_time = stop_time + time_elap;
      }
      if(b){
        start_time = micros();
        reverse();
        function_time = micros();
        time_elap = function_time - start_time;
        rev_time = rev_time + time_elap;
      }

      if(l){
        start_time = micros();
        soft_left_turn();
        function_time = micros();
        time_elap = function_time - start_time;
        left_time = left_time + time_elap;
      }

      if(r){
        start_time = micros();
        soft_right_turn();
        function_time = micros();
        time_elap = function_time - start_time;
        right_time = right_time + time_elap;
      }

      // Refresh data
     // data = "";    
    }

  }
  
}


//Straight
void slow_straight() {
  analogWrite(H_BRIDGE_LEFT_B, 0);
  analogWrite(H_BRIDGE_LEFT_A, speed);
  analogWrite(H_BRIDGE_RIGHT_A, speed);
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
  analogWrite(H_BRIDGE_LEFT_A, 100);
  analogWrite(H_BRIDGE_LEFT_B, 0);
  analogWrite(H_BRIDGE_RIGHT_A, 0);
  analogWrite(H_BRIDGE_RIGHT_B, 100);
  delay(300);
}

//Left

void soft_left_turn() {
  analogWrite(H_BRIDGE_LEFT_B, 100);
  analogWrite(H_BRIDGE_LEFT_A, 0);
  analogWrite(H_BRIDGE_RIGHT_B, 0);
  analogWrite(H_BRIDGE_RIGHT_A, 100);
  delay(300);
}

void stop() {
  analogWrite(H_BRIDGE_LEFT_B, 0);
  analogWrite(H_BRIDGE_LEFT_A, 0);
  analogWrite(H_BRIDGE_RIGHT_A, 0);
  analogWrite(H_BRIDGE_RIGHT_B, 0);
}
