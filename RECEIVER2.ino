#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <RF24_config.h>
#include <printf.h>
#include <Wire.h>

#define in1 7 // Right
#define in2 5 // Left
#define in3 4 // Forward
#define in4 2 // Reverse

RF24 radio(8,9); // CE, CSN
const byte address[6] = "00001";

unsigned long lastReceiveTime = 0;
unsigned long currentTime = 0;

char receivedData[32] = "";
int  xAxis, yAxis;
int motorSpeedFW= 0;
int motorSpeedRV= 0;
int motorSpeedLt = 0;
int motorSpeedRt = 0;
int joystick[2]; 

void setup() {

  pinMode(in1, OUTPUT); // 7 right
  pinMode(in2, OUTPUT); // 5 left
  pinMode(in3, OUTPUT); // 4 forward
  pinMode(in4, OUTPUT); // 2 reverse
  Serial.begin(9600);
  radio.begin();
  radio.openReadingPipe(0, address);
  radio.setPALevel(RF24_PA_MAX);
  radio.startListening();
    
  digitalWrite(in1, LOW); // 7 right
  digitalWrite(in2, LOW); // 5 left
  digitalWrite(in3, LOW); // 4 forward
  digitalWrite(in4, LOW); // 2 reverse
  resetData();
}


void loop() {

  if (radio.available()) {   // If the NRF240L01 module received data
     
    radio.read( joystick, sizeof(joystick));

    radio.read(&receivedData, sizeof(receivedData));
    yAxis = joystick[0];
    xAxis = joystick[1];
    lastReceiveTime = millis(); // At this moment we have received the data
  }
  // Check whether we keep receving data, or we have a connection between the two modules
  currentTime = millis();
  if ( currentTime - lastReceiveTime > 1000 ) { // If current time is more then 1 second since we have recived the last data, that means we have lost connection
    resetData(); // If connection is lost, reset the data. It prevents unwanted behavior, for example if a drone has a throttle up and we lose connection, it can keep flying unless we reset the values
  }

    Serial.print(" | Y: ");
    Serial.print(yAxis);
    Serial.print(" | X: ");
    Serial.print(xAxis);

  delay(100);

  

///// y axis fwd and rev /////////
  
  if (yAxis < 510) {

    analogWrite(in3, (motorSpeedFW)); //fw
    digitalWrite(in4, 0); //rv

    motorSpeedFW = map(yAxis, 510, 0, 0, 255);
  
  }
  else if (yAxis > 550) {

    digitalWrite(in3, 0); // fw
    analogWrite(in4, motorSpeedRV); // rv
    
    motorSpeedRV = map(yAxis, 550, 1023, 0, 255);
  }

  else {
    digitalWrite(in3, 0); // fw
    digitalWrite(in4, 0); // rv
  }


////////// x axis rt and left ////////////

   if (xAxis < 480) {

    analogWrite(in1, (motorSpeedRt)); //Rt
    digitalWrite(in2, 0); //Lt

    motorSpeedRt = map(xAxis, 480, 0, 0, 255);
  
  }
  else if (xAxis > 520) {

    digitalWrite(in1, 0); // Rt
    analogWrite(in2, motorSpeedLt); // Lft
    
    motorSpeedLt = map(xAxis, 520, 1023, 0, 255);
  }

  else {
    digitalWrite(in2, 0); // Lt
    digitalWrite(in1, 0); // Rt
  }

}
void resetData() {
  Serial.print("Connection not made!");
  // Reset the values when there is no radio connection - Set initial default values
   digitalWrite(in1, 0);
   digitalWrite(in2, 0);
   digitalWrite(in3, 0);
   digitalWrite(in4, 0);
}
