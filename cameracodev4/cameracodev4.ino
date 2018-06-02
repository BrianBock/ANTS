// Wii Remote IR sensor  test sample code  by kako http://www.kako.com
// modified output for Wii-BlobTrack program by RobotFreak http://www.letsmakerobots.com/user/1433
// modified for http://DFRobot.com by Lumi, Jan. 2014

// From https://www.dfrobot.com/wiki/index.php/Positioning_ir_camera

#include <Wire.h>
#include <Servo.h>

Servo myservo;  // base servo
Servo triggerservo; //trigger servo


int IRsensorAddress = 0xB0;
//int IRsensorAddress = 0x58;
int slaveAddress;
int ledPin = 13;
boolean ledState = false;
byte data_buf[16];
int i;

int Ix[4];
int Iy[4];
int s;
int midleft = 480;
int midright = 540;
String statusvar = "TRACKING";

void Write_2bytes(byte d1, byte d2) // Sets up the sensor for I2C communication
{
  Wire.beginTransmission(slaveAddress);
  Wire.write(d1); Wire.write(d2);
  Wire.endTransmission();
}

void setup()
{
  myservo.attach(6);  // attaches the servo on pin 3 to the servo object
  triggerservo.attach(5); //attaches the trigger servo to pin 5
  myservo.write(90);
  triggerservo.write(93);
  slaveAddress = IRsensorAddress >> 1;   // This results in 0x21 as the address to pass to TWI
  Serial.begin(57600);
  pinMode(ledPin, OUTPUT);      // Set the LED pin as output
  Wire.begin();
  // IR sensor initialize
  Write_2bytes(0x30, 0x01); delay(10);// Sets up the sensor for I2C communication
  Write_2bytes(0x30, 0x08); delay(10);
  Write_2bytes(0x06, 0x90); delay(10);
  Write_2bytes(0x08, 0xC0); delay(10);
  Write_2bytes(0x1A, 0x40); delay(10);
  Write_2bytes(0x33, 0x33); delay(10);
  delay(100);

}

void loop()
{
  statusvar = "TRACKING"; //Status printed to Serial monitor for Processing
  int servoposition = myservo.read(); //curent position of the base servo
  ledState = !ledState;
  if (ledState) {
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(ledPin, LOW);
  }

  //IR sensor read
  Wire.beginTransmission(slaveAddress); //reads the sensor values
  Wire.write(0x36);
  Wire.endTransmission();

  Wire.requestFrom(slaveAddress, 16);        // Request the 2 byte heading (MSB comes first)
  for (i = 0; i < 16; i++) {
    data_buf[i] = 0;
  }
  i = 0;
  while (Wire.available() && i < 16) {
    data_buf[i] = Wire.read();
    i++;
  }

  Ix[0] = data_buf[1]; //X coordinate from the sensor
  Iy[0] = data_buf[2]; //Y coordinate from the sensor
  s   = data_buf[3];
  Ix[0] += (s & 0x30) << 4;
  Iy[0] += (s & 0xC0) << 2;
  delay(15);

  if ((Ix[0] > 512) && (Ix[0] < 1023)) { //checks if the xcoordinate is past the half way point but still defined (<1023)
    if (servoposition != 180) { //safety to prevent the Servo from trying to turn past it's max
      myservo.write(servoposition + 1); //increments the servo 1 degree
      servoposition = myservo.read(); //rechecks the servo position
      statusvar = "TRACKING";

      Serial.print( int(Ix[0]) ); //Prints the x and y coords for Processing
      Serial.print(",");
      Serial.print( int(Iy[0]) );
      Serial.print(",");
      Serial.println(statusvar);

      delay(20);
    }
  }
  if ((Ix[0] < 512) && (Ix[0] >= 0)) { //checks if the xcoordinate is to the left of the half way point
    if (servoposition != 0) { //safety to prevent the servo from trying to turn past it's min
      myservo.write(servoposition - 1); //moves the servo one degree
      servoposition = myservo.read();
      statusvar = "TRACKING";

      Serial.print( int(Ix[0]) ); //Prints the x and y coords for Processing
      Serial.print(",");
      Serial.print( int(Iy[0]) );
      Serial.print(",");
      Serial.println(statusvar);


      delay(20);
    }
  }


  /*
    //Old Function for a third status - "No Light Found" that was removed due to noise inherent in the sensor

    if ((Ix[0] = 1023) && (Iy[0] = 1023)) { //checks if there is no light found (x=1023)
      statusvar = "NOLIGHT";
      Serial.print( int(Ix[0]) ); //Prints the x and y coords for Processing
      Serial.print(",");
      Serial.print( int(Iy[0]) );
      Serial.print(",");
      Serial.println(statusvar);
      delay(20);

    }
  */

  if ((Ix[0] > midleft) && (Ix[0] < midright)) { //checks midpoint fire condition
    if (servoposition != 0) { //safety to prevent the servo from trying to turn past it's min
      int k1 = Ix[0];
      if ((Ix[0] > midleft) && (Ix[0] < midright)) { //checks midpoint fire condition
        int k2 = Ix[0];
        if ((k2 - k1 <= 50) || (k1 - k2) <= 50) { //tolerance
          statusvar = "FIRE";
          Serial.print( int(Ix[0]) ); //Prints the x and y coords for Processing
          Serial.print(",");
          Serial.print( int(Iy[0]) );
          Serial.print(",");
          Serial.println(statusvar);
          delay(200);
          triggerservo.write(150); //Pulls the trigger
          delay(500);
          triggerservo.write(93); //Returns the trigger servo in it's default position
          pirates();
          delay(20);
        }
      }
    }
  }

  Serial.print( int(Ix[0]) ); //Prints the x and y coords for Processing
  Serial.print(",");
  Serial.print( int(Iy[0]) );
  Serial.print(",");
  Serial.println(statusvar);

  delay(50);
}










void pirates() {
  /*
    https://github.com/xitangg/-Pirates-of-the-Caribbean-Theme-Song
      "He's A Pirate" / "Pirates of the Caribbean" Theme Song Playing
    By Xitang 2016.06.27
    Youtube in Action: https://youtu.be/sjPAj1lXgtk


    INSTRUCTION: Hook up Pin 10 to the positive side of a buzzer or a microphone, hook up
    any resistor to the negative side of a buzzer (to limit current & volume & to protect the pin),
    and then connect the other end of the resistor to ground pin. Upload the sketch and enjoy!

    Music notes of the song obtained from Easy Music (Great website)
    Link: http://easymusic.altervista.org/recorder-hes-a-pirate-pirates-of-caribbean-sheet-music-guitar-chords/

    Also would like to credit Musicnotes's "How to Read Sheet Music" Guide
    Link: http://www.musicnotes.com/blog/2014/04/11/how-to-read-sheet-music/

    Code inspired by Chapter 5 of Jeremy Blum's book "Exploring Arduino"
    Link: http://www.exploringarduino.com/content/ch5/

    Music notes' frequencies obtained from arduino website
    Link: https://www.arduino.cc/en/Tutorial/toneMelody
  */
  const int buzzer = 10; //Define pin 10, can use other PWM pins  (5,6 or 9)
  //Note pins 3 and 11 can't be used when using the tone function in Arduino Uno
  const int songspeed = 1.5; //Change to 2 for a slower version of the song, the bigger the number the slower the song
  //*****************************************
#define NOTE_C4  262   //Defining note frequency
#define NOTE_D4  294
#define NOTE_E4  330
#define NOTE_F4  349
#define NOTE_G4  392
#define NOTE_A4  440
#define NOTE_B4  494
#define NOTE_C5  523
#define NOTE_D5  587
#define NOTE_E5  659
#define NOTE_F5  698
#define NOTE_G5  784
#define NOTE_A5  880
#define NOTE_B5  988
  //*****************************************
  int notes[] = {       //Note of the song, 0 is a rest/pulse
    NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, 0,
    NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0,
    NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4, 0,
    NOTE_A4, NOTE_G4, NOTE_A4, 0,

    NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, 0,
    NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0,
    NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4, 0,
    NOTE_A4, NOTE_G4, NOTE_A4, 0,

    NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, 0,
    NOTE_A4, NOTE_C5, NOTE_D5, NOTE_D5, 0,
    NOTE_D5, NOTE_E5, NOTE_F5, NOTE_F5, 0,
    NOTE_E5, NOTE_D5, NOTE_E5, NOTE_A4, 0,

    NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0,
    NOTE_D5, NOTE_E5, NOTE_A4, 0,
    NOTE_A4, NOTE_C5, NOTE_B4, NOTE_B4, 0,
    NOTE_C5, NOTE_A4, NOTE_B4, 0,

    NOTE_A4, NOTE_A4,
    //Repeat of first part
    NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0,
    NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4, 0,
    NOTE_A4, NOTE_G4, NOTE_A4, 0,

    NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, 0,
    NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0,
    NOTE_C5, NOTE_D5, NOTE_B4, NOTE_B4, 0,
    NOTE_A4, NOTE_G4, NOTE_A4, 0,

    NOTE_E4, NOTE_G4, NOTE_A4, NOTE_A4, 0,
    NOTE_A4, NOTE_C5, NOTE_D5, NOTE_D5, 0,
    NOTE_D5, NOTE_E5, NOTE_F5, NOTE_F5, 0,
    NOTE_E5, NOTE_D5, NOTE_E5, NOTE_A4, 0,

    NOTE_A4, NOTE_B4, NOTE_C5, NOTE_C5, 0,
    NOTE_D5, NOTE_E5, NOTE_A4, 0,
    NOTE_A4, NOTE_C5, NOTE_B4, NOTE_B4, 0,
    NOTE_C5, NOTE_A4, NOTE_B4, 0,
    //End of Repeat

    NOTE_E5, 0, 0, NOTE_F5, 0, 0,
    NOTE_E5, NOTE_E5, 0, NOTE_G5, 0, NOTE_E5, NOTE_D5, 0, 0,
    NOTE_D5, 0, 0, NOTE_C5, 0, 0,
    NOTE_B4, NOTE_C5, 0, NOTE_B4, 0, NOTE_A4,

    NOTE_E5, 0, 0, NOTE_F5, 0, 0,
    NOTE_E5, NOTE_E5, 0, NOTE_G5, 0, NOTE_E5, NOTE_D5, 0, 0,
    NOTE_D5, 0, 0, NOTE_C5, 0, 0,
    NOTE_B4, NOTE_C5, 0, NOTE_B4, 0, NOTE_A4
  };
  //*****************************************
  int duration[] = {         //duration of each note (in ms) Quarter Note is set to 250 ms
    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 375, 125,

    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 375, 125,

    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 125, 250, 125,

    125, 125, 250, 125, 125,
    250, 125, 250, 125,
    125, 125, 250, 125, 125,
    125, 125, 375, 375,

    250, 125,
    //Rpeat of First Part
    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 375, 125,

    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 375, 125,

    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 250, 125, 125,
    125, 125, 125, 250, 125,

    125, 125, 250, 125, 125,
    250, 125, 250, 125,
    125, 125, 250, 125, 125,
    125, 125, 375, 375,
    //End of Repeat

    250, 125, 375, 250, 125, 375,
    125, 125, 125, 125, 125, 125, 125, 125, 375,
    250, 125, 375, 250, 125, 375,
    125, 125, 125, 125, 125, 500,

    250, 125, 375, 250, 125, 375,
    125, 125, 125, 125, 125, 125, 125, 125, 375,
    250, 125, 375, 250, 125, 375,
    125, 125, 125, 125, 125, 500
  };


  for (int i = 0; i < 203; i++) {       //203 is the total number of music notes in the song
    int wait = duration[i] * songspeed;
    tone(buzzer, notes[i], wait);        //tone(pin,frequency,duration)
    delay(wait);
  }                        //delay is used so it doesn't go to the next loop before tone is finished playing
  //You can click reset on Arduino to replay the song
}
