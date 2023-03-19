import controlP5.*;
import processing.net.*;
ControlP5 cp5;
Client myClient;
String data;
String command;

void setup() {
  //connects to server

  myClient = new Client(this,"192.168.100.59",5200);
  myClient.write("I am a new client");
  size(640,480);
  cp5= new ControlP5(this);
  //makes buttons
  cp5.addButton("Go").setValue(0).setPosition(320-100,240).setSize(200,19);
  cp5.addButton("Stop").setValue(0).setPosition(320-100,280).setSize(200,19);
  //background colour
  background(255, 255, 10);
  textSize(30);
  text("buggy speed:", 60, 350); 
  //fill(0, 408, 612);
  text("object speed:", 60, 390);
  fill(99, 99, 99);
  }

void draw() {
   
  data = myClient.readString();
  if(data != null){
  println(data);
  textSize(30);
  text("buggy speed:", 60, 350); 
  //fill(0, 408, 612);
  text("object speed:", 60, 390);
  fill(99, 99, 99);
  
  if(data.charAt(0)=='$'){
      background(255, 255, 10);
      textSize(30);
      text("buggy speed:", 60, 350); 
      //fill(0, 408, 612);
      text("object speed:", 60, 390);
      fill(99, 99, 99);
  
      String bv = data.substring(1, 5);
      text(bv, 270, 350);
      fill(99, 99, 99);
    }
  if(data.charAt(0)=='%'){
      background(255, 255, 10);
      textSize(30);
      text("buggy speed:", 60, 350); 
      //fill(0, 408, 612);
      text("object speed:", 60, 390);
      fill(99, 99, 99);
      
      String bv = data.substring(1, 5);
      text(bv, 270, 390);
      fill(99, 99, 99);
  }
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
