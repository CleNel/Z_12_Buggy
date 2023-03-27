import controlP5.*;
import processing.net.*;
ControlP5 cp5;
Client myClient;
String data;
String command;
PImage background;

void setup() {
  //connects to server
  myClient = new Client(this,"192.168.86.201",5200);
  myClient.write("I am a new client");
  size(834,538);
  cp5= new ControlP5(this);
  //makes buttons
  cp5.addButton("Go").setValue(0).setPosition(400-100,417).setSize(15,15);
  cp5.addButton("Stop").setValue(0).setPosition(612-100,417).setSize(15,15);
  //background colour
  background = loadImage("C:\\Users\\Garym\\OneDrive\\Pictures\\spaceshiptester.jpg");
  
 
  }

void draw() {
  //data = "5,22";
  data = myClient.readString();
   textSize(15);
   background(background);
  text("buggy speed \n (cm/s):", 10, 140); 
  //fill(0, 408, 612);
  text("object speed \n (cm/s):", 690, 140);
  //fill(99, 99, 99);
  noStroke();
  myClient.clear();
  if(data != null){
    //println(data);
    String reversed = new StringBuilder(data).reverse().toString();
    int end = reversed.indexOf('#');
    int start = reversed.indexOf('@', end);
    println(end);
    if (end >0){
    String out = reversed.substring(end + 1, start);
    String[] speeds = split(new StringBuilder(out).reverse().toString(), ',');
    fill(255,255,10);
    rect(350,320,150,300);
    fill(99, 99, 99);
    println(speeds[1]);
      text(speeds[1], 360, 390);
      text(speeds[0], 360, 350);
    }
    //if(speeds[0] == "0.00" && speeds[1] == "0.00"){  
    //  text("0", 360, 390);
    //  text("0", 360, 350);
    //}
    //else {
      
    //}
  }
}

public void Go(int theValue){
  if (myClient.active()){
     myClient.write("go\n");
   }
}

public void Stop(int theValue){
  if (myClient.active()){
     myClient.write("stop\n");
   }
}
