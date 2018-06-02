// Example by Tom Igoe
// Modified for http://www.DFRobot.com by Lumi, Jan. 2014
// From https://www.dfrobot.com/wiki/index.php/Positioning_ir_camera

import processing.serial.*;

int lf = 10;    // Linefeed in ASCII
String myString = null;
Serial myPort;  // The serial port

void setup() {
  // List all the available serial ports
  println(Serial.list());
  // Open the port you are using at the rate you want:
  myPort = new Serial(this, Serial.list()[3], 57600);
  myPort.clear();
  // Throw out the first reading, in case we started reading 
  // in the middle of a string from the sender.
  myString = myPort.readStringUntil(lf);
  myString = null;
  size(800, 800);
  //frameRate(30);
  textSize(150);
  textAlign(CENTER);
}

void draw() {
  background(77);
  stroke(255, 0, 0);

  if (myPort.available () > 0) { // make sure port is open
    myString = myPort.readStringUntil('\n');
    if (myString != null) {
      myString = trim(myString); // trim off any whitespace
      String[] output = splitTokens(myString, ",");
      println(myString); // display the incoming string
      if (output.length == 3) {
        int xx = int(output[0]); //xcoordinate read from the sensor
        int yy = int(output[1]); //ycoordinate read from the sensor
        String statusvar=(output[2]); //status read from Arduino
        String firecondition="FIRE"; //Different statuses
        String NotFound="NOLIGHT";
        String Tracking="TRACKING";
        //String Pirates="PIRATES";
        ellipseMode(RADIUS);  // Set ellipseMode to RADIUS
        fill(255, 0, 0);  // Set fill to red
        ellipse(width-xx, width-yy, 20, 20);//Draws a circle representing the position of the IR light


        if (statusvar.equals(firecondition)==true) { //Does all of the FIRE page updates
          text("Status: Ready to Fire", 20, 20);
          ellipse(width-xx, width-yy, 20, 20);
          textSize(150);
          textAlign(CENTER);
          fill(255, 0, 0);
          text("Fire!", width/2, height/2);
          delay(2000);
          println("firing!");
          ellipse(width-xx, width-yy, 20, 20);
          textSize(20);
          textAlign(CENTER);
          text("Fired! Please reload the gun and relock the hammer", width/2, height/2+50);
          text("When you're ready, please hit the reset button on the Arduino", width/2, height/2+75);
          println("Pirates!");
          ellipse(width-xx, width-yy, 20, 20);
          noLoop();
        }
        if (statusvar.equals(NotFound)==true) { //Light Not Found status (now obselete)
          fill(255, 255, 255);
          textSize(20);
          textAlign(LEFT);
          text("Status: No light source found", 20, 20);
          println("Nothing to see here!");
        }
        if (statusvar.equals(Tracking)==true) { //Tracking status
          fill(255, 255, 255);
          textSize(20);
          textAlign(LEFT);
          text("Status: Tracking", 20, 20);
          println("Tracking!");
          fill(255, 0, 0);
          ellipse(width-xx, width-yy, 20, 20);
        }
        /*if (statusvar.equals(Pirates)==true) {
         pirates();
         }*/
      }
    }
  }
  delay(50);
}