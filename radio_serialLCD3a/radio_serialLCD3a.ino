// original from https://github.com/makserge/tef6686_radio
// small changes by Nicu FLORICA (niq_ro)

// add LCD
#include <SoftwareSerial.h>
#include <SerLCD.h>
// Make a SoftwareSerial object, and a new SerLCD object, 
// which points at the NewSoftSerial object. 
//
// In this demo the Rx pin of the SerLCD is connected to 
// Arduino digital pin 4 (Tx)  We set the SoftwareSerial 
// Rx to 0 because the SerLCD has no facility to transmit
// back. 
//
SoftwareSerial NSS(255,4,1);
SerLCD lcd(NSS,20,4); 

#include "TEF6686.h"

uint16_t frequency;
int volume;
int volume2;
byte plus = 0;

bool isRdsSync;  

char programTypePrevious[17] = "                ";
char programServicePrevious[9];
char programServiceUnsafePrevious[9];
char programIdPrevious[4];
char radioTextPrevious[65];

TEF6686 radio;
RdsInfo rdsInfo;

void setup() {
   NSS.begin(9600); 
  lcd.begin();
  lcd.clear(); 
  // Print a logo message to the LCD.
  lcd.setPosition(1, 0);
  lcd.print(" TEF6686 radio  v.3 ");
  lcd.setPosition(2, 0);
  lcd.print("github.com/kkonradpl");
  lcd.setPosition(3, 0);
  lcd.print(" updated by  niq_ro ");
  lcd.setPosition(4, 0);
  lcd.print(" github.com/tehniq3 ");
  delay (3000);
  lcd.clear();
  
  delay(1000);
  Serial.begin(115200);
  while (!Serial);
  Serial.println(F("Start"));
  radio.init();
  radio.powerOn();
  radio.setFrequency(9800);
  frequency = radio.getFrequency();
  //displayInfo();
  radioGui(1);

}

void loop() {
  readRds();
  radioGui(0);
  displayInfo();
  if (Serial.available()) {
    char ch = Serial.read();
    if (ch == 'm') {
      radio.setMute();
      Serial.println("Mute");    
     //displayInfo();
    }
    else if (ch == 'n') {
      radio.setUnMute();
      Serial.println("Unmute");      
      //displayInfo();
    }
    else if (ch == 'p') {
      radio.powerOn();
      Serial.println("Power ON");    
      //displayInfo();
    } 
    else if (ch == 'o') {
      radio.powerOff();
      Serial.println("Power OFF");      
      //displayInfo();
    }
    if (ch == 'u') {
      frequency = radio.seekUp();           
      //displayInfo();
      radioGui(true);
    } 
    else if (ch == 'd') {
      frequency = radio.seekDown();  
      //displayInfo();
      radioGui(true);
    } 
    else if (ch == '+') {
      volume += 4;
      if (volume >= 24) volume = 24;
      radio.setVolume(volume);
      displayInfo();
    } 
    else if (ch == '-') {
      volume -= 4;
      if (volume < -60) volume = -60;
      radio.setVolume(volume);
      displayInfo();
    } 
    else if (ch == '8') {
      frequency = radio.tuneUp();
      //displayInfo();
      radioGui(true);
    }
    else if (ch == '2') {
      frequency = radio.tuneDown();
      //displayInfo();
      radioGui(true);
    }    
    else if (ch == 'r') {
      radioGui(true);
    }
  }
  /*
delay(500);
 lcd.setPosition(3, 0);
 lcd.print("                    ");    
 lcd.setPosition(4, 0);
 lcd.print("                    "); 
 */
}

void readRds() {
  isRdsSync = radio.readRDS(); 
  radio.getRDS(&rdsInfo);
}

void showPI() {
  if (isNewPi()){
    strcpy(programIdPrevious, rdsInfo.programId);
  }
  Serial.print(rdsInfo.programId);
  lcd.setPosition(2, 13);
  lcd.print(rdsInfo.programId);  
}

bool isNewPi(){
  return (strlen(rdsInfo.programId) == 4) && !strcmp(rdsInfo.programId, programIdPrevious, 4);
}


void showPTY() {
  if (isNewPty()) { 
    strcpy(programTypePrevious, rdsInfo.programType);
  }
    Serial.print(rdsInfo.programType);  
    lcd.setPosition(3, 10);
    lcd.print(rdsInfo.programType); 
  }  

bool isNewPty() {
  return (isRdsSync == 1) && !strcmp(rdsInfo.programType, programTypePrevious, 16);
}

void showPS() {
  if (isNewPs()) {
    strcpy(programServicePrevious, rdsInfo.programService);
  }
  Serial.print(rdsInfo.programService);
  lcd.setPosition(3, 0);
  lcd.print(rdsInfo.programService); 
  
}

void showPsUnsafe(){
  if (isNewPsUnsafe()){
    strcpy(programServiceUnsafePrevious, rdsInfo.programServiceUnsafe);
  }
  Serial.print(rdsInfo.programServiceUnsafe);
  lcd.setPosition(4, 0);
  lcd.print(rdsInfo.programServiceUnsafe); 
}

bool isNewPs() {
  return (strlen(rdsInfo.programService) == 8) && !strcmp(rdsInfo.programService, programServicePrevious, 8);
}

bool isNewPsUnsafe() {
  return (strlen(rdsInfo.programServiceUnsafe) == 8) && !strcmp(rdsInfo.programServiceUnsafe, programServiceUnsafePrevious, 8);
}


void showRadioText() {
  if (isNewRt()){
    strcpy(radioTextPrevious, rdsInfo.radioText);
  }
    Serial.print(rdsInfo.radioText); 
    lcd.setPosition(4, 10);
    lcd.print(rdsInfo.radioText); 
}


bool isNewRt() {
  return (isRdsSync == 1) && !strcmp(rdsInfo.radioText, radioTextPrevious, 65);
}

bool strcmp(char* str1, char* str2, int length) {
  for (int i = 0; i < length; i++) {
    if (str1[i] != str2[i]) {
      return false;
    }    
  }  
  return true;
}

void displayInfo() {
   delay(10);
   Serial.print(F("Frequency:")); 
   Serial.print(frequency); 
   Serial.print(F(" Volume:")); 
   Serial.println(volume);
   Serial.print(F("Level:")); 
   Serial.println(radio.getLevel() / 10);
   Serial.print(F("Stereo:")); 
   Serial.println(radio.getStereoStatus());
     lcd.setPosition(1, 0);
    if (frequency < 10000) lcd.print(" ");
    lcd.print(frequency/100); 
    lcd.print(",");
    lcd.print(frequency%100);
    lcd.print("MHz");
    lcd.setPosition(2, 0);
//    lcd.print("vol.");
    if (volume > 0) 
    {
      plus = 1;
      volume2 = volume;
    }
    else
    {
      plus = 0;
      volume2 = -volume;
    }
    if (volume2/10 == 0) 
    lcd.print(" "); 
    if (volume2 == 0) lcd.print(" ");
    else
    if (plus == 0)
    {
    lcd.print("-"); 
    }
    else
    {
    lcd.print("+"); 
    }
    lcd.print(volume2);
    lcd.print("dB");
    lcd.setPosition(2, 6);
 //   lcd.print("level:");
    if (radio.getLevel() < 100)
    lcd.print(" "); 
    if (radio.getLevel() < 10)
    lcd.print(" ");      
    lcd.print(radio.getLevel());
    lcd.print("%");
    lcd.setPosition(1, 13);
    if (radio.getStereoStatus() == 1)
    lcd.print("STEREO "); 
    else
    lcd.print("      ");    
}

void radioGui(bool force){
  if (force || isNewPi() || isNewPs() || isNewPsUnsafe()) {
    //clrscr
    Serial.write(27);       // ESC command
    Serial.print("[2J");    // clear screen command
    Serial.write(27);
    Serial.print("[H");     // cursor to home command

    //first line
    Serial.print(F("FM "));
    Serial.print(frequency / 100);
    Serial.print(F("."));
    Serial.print(frequency % 100 / 10);
    Serial.print(F(" MHz   "));

    Serial.print(frequency >= 10000 ? F("[ ") : F(" [ "));
    showPS();
    Serial.print(" ]");

    //second line
    Serial.println();
    Serial.print(F("PI:  "));
    showPI();
    Serial.print(F("      ( "));
    showPsUnsafe();
    Serial.print(F(" ) "));

    Serial.println();
    Serial.print(F("PTY:  "));
    showPTY();
    Serial.println();
    Serial.print( ("radio text:  "));
    showRadioText();
    Serial.println();
    Serial.println();
  }
}
