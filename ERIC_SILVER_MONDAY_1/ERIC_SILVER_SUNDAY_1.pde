import hypermedia.net.*;

import controlP5.*;
import processing.net.*;
ControlP5 cp5;
Client myClient;
String data;
String command;
String buggy_string= "";
String obj_string = "";
UDP udp;
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
  udp = new UDP(this, 5200);
  udp.listen(true);
  }

void draw() {
  textSize(30);
  text("buggy speed:", 60, 350); 
  //fill(0, 408, 612);
  text("object speed:", 60, 390);
  fill(99, 99, 99);
  Datagram packet = udp.availablePacket();
  if (packet != null){
    String incoming = new String(packet.getData()).trim();
    if (incoming != null) {
      String[] vels = splitTokens(incoming, ",");
      buggy_string = vels[0];
      obj_string = vels[1];
    }
  }
  // Display the strings on the window
  textAlign(CENTER);
  textSize(24);
  text(buggy_string, 120, 350);
  text(obj_string, 120, 390);
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
