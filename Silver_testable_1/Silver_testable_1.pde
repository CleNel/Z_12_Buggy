void setup() {
  //connects to server
  //frameRate(600);
  myClient = new Client(this,"192.168.86.59",5200);
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
  data= myClient.readString();
  if(data != null){
    println(data);
    String[] speeds = split(data, ',');
    if(speeds[0] == "0.00" && speeds[1] == "0.00"){
      fill(255,255,10);
      rect(350,320,100,300);
      fill(99, 99, 99);
      text("0", 360, 390);
      text("0", 360, 350);
    }
    else {
      fill(255,255,10);
      rect(350,320,100,300);
      fill(99, 99, 99);
      text(speeds[1], 360, 390)
