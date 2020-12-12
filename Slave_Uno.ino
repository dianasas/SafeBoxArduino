// Library for I2C
#include <Wire.h>
// Library for servomotor
#include <Servo.h>

String sentValue="";
char command=0;

Servo servo;

void setup() {
    // open I2C bus as slave at address 2
    Wire.begin(2);
    // device #1 requests informations from slave
    Wire.onRequest(requestEvent);
    // device #2 receives informations from slave
    Wire.onReceive(receiveEvent);
    Serial.begin(9600); // format frame serial implicit
    Serial.println("Slave");
    sentValue.reserve(10);
    servo.attach(8);
    servo.write(90);
}

void loop() {
  
}

void receiveEvent(int bytes) {
    command = Wire.read(); // read char from I2C
}

void requestEvent(){
    if(command=='C'){
          Serial.println("Master wants to close the safe."); // show command
          sentValue="CLSD";
          if(servo.read()==180){  // if the safe is open, close it 
            servo.write(90);  
          } 
          else{
            Serial.println("Safe is already closed");    
          }
          Wire.write(sentValue.c_str(), sentValue.length()); // send value to device #1
    }
    if(command=='O'){
          Serial.println("Master wants to open the safe."); // show command
          sentValue="OPEN";
          if(servo.read()==90){ // if the safe is closed, open it
            servo.write(180);
          }
          else{
            Serial.println("Safe is already opened.");           
          }
          Wire.write(sentValue.c_str(), sentValue.length()); // send value to device #1
    }
}
