import controlP5.*;
import processing.net.*;
ControlP5 cp5;
Client myClient;
String data;
String command;

void setup() {
  //connects to server
  myClient = new Client(this,"192.168.86.201",5200);
  myClient.write("I am a new client");
  size(640,400);
  cp5= new ControlP5(this);
  //makes buttons
  cp5.addButton("Go").setValue(0).setPosition(320-100,240).setSize(200,19);
  cp5.addButton("Stop").setValue(0).setPosition(320-100,280).setSize(200,19);
  //background colour
  background(255, 255, 10);
  textSize(30);
  text("buggy speed (cm/s):", 100, 350); 
  //fill(0, 408, 612);
  text("object speed (cm/s):", 100, 390);
  //fill(99, 99, 99);
  noStroke();
  }

void draw() {
  //data = "5,22";
  data = myClient.readString();
  myClient.clear();
  if(data != null){
    //println(data);
    String reversed = new StringBuilder(data).reverse().toString();
    int end = reversed.indexOf('#');
    int start = reversed.indexOf('@', end);
    String out = reversed.substring(end + 1, start);
    String[] speeds = split(new StringBuilder(out).reverse().toString(), ',');
    fill(255,255,10);
    rect(350,320,150,300);
    fill(99, 99, 99);
    println(speeds[1]);
    if (speeds[1]=="no"){
      text("no object", 360, 390);
      text(speeds[0], 360, 350);
    }
    else{
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
