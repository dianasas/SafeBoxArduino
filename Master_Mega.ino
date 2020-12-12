// Library for working with the keypad
#include <Keypad.h>
// Library for I2C
#include <Wire.h>
// Library for working with the LCD
#include <LiquidCrystal.h>


int A_Pin = 9; // pin brightness
int Brightness = 255;
int RS_Pin = 12;
int E_Pin = 11;
int D4_Pin = 5;
int D5_Pin = 4;
int D6_Pin = 3;
int D7_Pin = 2;
int V0_Pin = 6;
int Contrast = 110;
LiquidCrystal lcd(RS_Pin, E_Pin, D4_Pin, D5_Pin, D6_Pin, D7_Pin);
/*
   K, RW, VSS - GND
   VDD - 5V
   V0 - contrast
*/

const int ROW_NUM = 4; // four rows
const int COLUMN_NUM = 4; // four columns

char keys[ROW_NUM][COLUMN_NUM] = {
  {'1', '2', '3', 'A'},
  {'4', '5', '6', 'B'},
  {'7', '8', '9', 'C'},
  {'*', '0', '#', 'D'}
};

byte pin_rows[ROW_NUM] = {22, 24, 26, 28}; // connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {23, 25, 27, 29}; // connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

const String password = "1359";
String inputPassword = "";
String receivedValue = "";
int wrongTries = 0;
char key = ' ';
char bluetoothCommand = 0;

long firstTime = 0;
long prevT = 0, curT = 0;
long interval = 60000;
boolean first = false;

void setup() {
  Serial.begin(9600); // format frame serial implicit
  Serial2.begin(9600); // Serial 2 interface( Bluetooth)
  // open I2C bus as master
  Wire.begin(1);
  analogWrite(A_Pin, Brightness);
  analogWrite(V0_Pin, Contrast);
  lcd.begin(16, 2);
  printMessage(1, key); // information about the safe's state
  inputPassword.reserve(10);
  receivedValue.reserve(10);
}

void loop() {
  getInfoBluetooth();
  blockSafe();
  introducePIN();
}

void introducePIN() {
  key = keypad.getKey();
  if (key) {
    if (key == 'C') {
      printMessage(4, key); // PIN introduced. / PIN correct
      Wire.beginTransmission(2); // send to device #2
      Wire.write('C'); // open
      Wire.endTransmission();
      Wire.requestFrom(2, 4); // request from device #2 4 chars(format CLSD) 
      receive();
    }
    else {
      if (key == '*') { // the user is starting to introduce the PIN
        printMessage(3, key); // Introducing pin
        inputPassword = ""; // clear input password
      }
      else if (inputPassword.length() == 4) { // correct PIN length is 4
        if (key == '#') { // the user validated the input
          if (password == inputPassword) {
            wrongTries = 0;
            printMessage(4, key); // PIN introduced. / PIN correct
            Wire.beginTransmission(2); // send to device #2
            Wire.write('O'); // open
            Wire.endTransmission();
            Wire.requestFrom(2, 4); // request from device #2 4 chars(format OPEN)
            receive();
            inputPassword = ""; // clear input password
          } else {
            printMessage(5, key); // PIN introduced. / PIN incorrect.
            wrongTries++;
            inputPassword = ""; // clear input password
          }
        }
        else {
          if (key != 'D') {
            printMessage(6, key); // PIN too long
            wrongTries++;
            inputPassword = ""; // clear input password
          }
          else { // the user wants to delete the last char introduced
            inputPassword = inputPassword.substring(0, inputPassword.length() - 1);
            printMessage(7, key);
          }
        }
      }
      else {
        if (key == 'D') { // delete last char
          if (inputPassword.length() > 0) {
            inputPassword = inputPassword.substring(0, inputPassword.length() - 1);
            printMessage(7, key);
          }
        }
        else {
          if (inputPassword.length() == 0) {
            printMessage(8, key); // set cursor to first column
          }
          printMessage(9, key); // print key
          inputPassword += key; // append new character to input password string
        }
      }
    }
  }


}

void receive() {
  while (Wire.available() > 0) {
    char c = Wire.read();
    receivedValue += c; // concat every received char
  }
  if (receivedValue == "CLSD") {
    printMessage(1, key);
  }
  else {
    if (receivedValue == "OPEN") {
      printMessage(11, key);
    }
  }
  receivedValue = "";
}


void getInfoBluetooth() {
  if (Serial2.available()) { // read from Bluetooth terminal
    bluetoothCommand = Serial2.read();
  }
}

void blockSafe() {
  while ( bluetoothCommand == 'L') { // safe is blocked forever
    printMessage(12, key);
  }
  while (wrongTries == 3) { // safe is blocked for 1 minute
    interval = 60000;
    printMessage(2, key);
    if (first == false) {
      curT = millis();
      firstTime = curT;
      first = true;
    }
    else {
      curT = millis();
    }
    if ( curT - firstTime >= interval) {
      wrongTries = 0;
      first = false;
      printMessage(13, key);
    }
  }
}

void printMessage(int messageNumber, char key) {
  switch (messageNumber) {
    case 1: lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 0);
      lcd.print("Safe is locked");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      break;
    case 2: lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 0);
      lcd.print("Blocked. Try ");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("again in 1 min.");
      break;
    case 3: lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 0);
      lcd.print("Introducing PIN");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      break;
    case 4: lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 0);
      lcd.print("PIN introduced");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("PIN correct");
      break;
    case 5: lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 0);
      lcd.print("PIN introduced");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("PIN incorrect");
      break;
    case 6: lcd.print(key);
      lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 0);
      lcd.print("PIN too long.");
      break;
    case 7: lcd.setCursor(inputPassword.length(), 1);
      lcd.print(" ");
      lcd.setCursor(inputPassword.length(), 1);
      break;
    case 8: lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 0);
      lcd.print("Introducing PIN");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      break;
    case 9: lcd.print(key);
      break;
    case 10: lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 0);
      lcd.print("Safe is closed.");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      break;
    case 11: lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 0);
      lcd.print("Safe is unlocked");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      break;
    case 12: lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 0);
      lcd.print("Blocked via");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("Bluetooth.");
      break;
    case 13: lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 0);
      lcd.print("You can try");
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      lcd.print("again now.");
      break;
    default: lcd.setCursor(0, 0);
      lcd.print("                ");
      lcd.setCursor(0, 0);
      lcd.setCursor(0, 1);
      lcd.print("                ");
      lcd.setCursor(0, 1);
      break;
  }
}
