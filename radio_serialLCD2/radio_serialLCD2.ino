// original from https://github.com/makserge/tef6686_radio
// small changes by Nicu FLORICA (niq_ro)

#include "TEF6686.h"

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


int frequency;
int volume;
int volume2;
byte plus = 0;

uint8_t isRDSReady;  

char programTypePrevious[17] = "                ";
char programServicePrevious[9];
char radioTextPrevious[65];

boolean isFmSeekMode;
boolean isFmSeekUp;

TEF6686 radio;
RdsInfo rdsInfo;

void setup() {
  NSS.begin(9600); 
  lcd.begin();
  lcd.clear(); 
  // Print a logo message to the LCD.
  lcd.setPosition(1, 0);
  lcd.print("   TEF6686 radio    ");
  lcd.setPosition(2, 0);
  lcd.print("github.com/kkonradpl");
  lcd.setPosition(3, 0);
  lcd.print(" updated by  niq_ro ");
  lcd.setPosition(4, 0);
  lcd.print(" github.com/tehniq3 ");
  delay (3000);
  lcd.clear();
  
//  delay(1000);
  Serial.begin(115200);
  while (!Serial);
  Serial.println("TEF6686 radio");
  radio.init();
  radio.powerOn();
  radio.setFrequency(9400);
  frequency = radio.getFrequency();
  displayInfo();    
}

void loop() {
  readRds();
  showFmSeek();
  displayInfo();
  
  if (Serial.available()) {
    char ch = Serial.read();
    if (ch == 'm') {
      radio.setMute();
      displayInfo();
    }
    else if (ch == 'n') {
      radio.setUnMute();
      displayInfo();
    }
    else if (ch == 'p') {
      radio.powerOn();
      displayInfo();
    } 
    else if (ch == 'o') {
      radio.powerOff();
      displayInfo();
    }
    else if (ch == 'u') {
      frequency = radio.seekUp();
      displayInfo();
    } 
    else if (ch == 'd') {
      frequency = radio.seekDown();
      displayInfo();
    }
    else if (ch == 'c') {
      Serial.println("Seeking up");
      isFmSeekMode = true;
      isFmSeekUp = true;
    } 
    else if (ch == 'e') {
      Serial.println("Seeking down");
      isFmSeekMode = true;
      isFmSeekUp = false;
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
    else if (ch == 'a') {
      frequency = radio.tuneUp();
      displayInfo();
    }
    else if (ch == 'b') {
      frequency = radio.tuneDown();
      displayInfo();
    }
  }
}

void readRds() {
  isRDSReady = radio.readRDS(); 
  radio.getRDS(&rdsInfo);

  showPTY();
  showPS();
  showRadioText(); 
}

void showPTY() {
  if ((isRDSReady == 1) && !strcmp(rdsInfo.programType, programTypePrevious, 16)) { 
    Serial.print(rdsInfo.programType);
    strcpy(programTypePrevious, rdsInfo.programType);
    Serial.println();
    lcd.setPosition(4, 11);
    //lcd.print(rdsInfo.programType); 
    lcd.print(strcpy(programTypePrevious, rdsInfo.programType));
  }  
}

void showPS() {
  if ((isRDSReady == 1) && (strlen(rdsInfo.programService) == 8) && !strcmp(rdsInfo.programService, programServicePrevious, 8)) {
    Serial.print("-=[ ");
    Serial.print(rdsInfo.programService);
    Serial.print(" ]=-");
    strcpy(programServicePrevious, rdsInfo.programService);
    Serial.println();
    lcd.setPosition(4, 0);
    //lcd.print(rdsInfo.programService);
    lcd.print(strcpy(programServicePrevious, rdsInfo.programService));
  }
}

void showRadioText() {
  if ((isRDSReady == 1) && !strcmp(rdsInfo.radioText, radioTextPrevious, 65)){
    Serial.print(rdsInfo.radioText);
    strcpy(radioTextPrevious, rdsInfo.radioText);
    Serial.println();
//    lcd.setPosition(3, 11);
    lcd.setPosition(3, 0);
    // lcd.print(rdsInfo.radioText);   
    lcd.print(strcpy(radioTextPrevious, rdsInfo.radioText));
  }
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
   Serial.print("Frequency:"); 
   Serial.print(frequency); 
   Serial.print(" Volume:"); 
   Serial.println(volume);
   Serial.print("Level:"); 
   Serial.println(radio.getLevel() / 10);
   Serial.print("Stereo:"); 
   Serial.println(radio.getStereoStatus());

    lcd.setPosition(1, 0);
    if (frequency < 10000) lcd.print(" ");
    lcd.print(frequency/100); 
    lcd.print(",");
    lcd.print(frequency%100);
    lcd.print("MHz");
    lcd.setPosition(2, 0);
    lcd.print("vol.");
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
    lcd.setPosition(2, 10);
    lcd.print("level:");
    if (radio.getLevel() < 100)
    lcd.print(" "); 
    if (radio.getLevel() < 10)
    lcd.print(" ");      
    lcd.print(radio.getLevel());
    lcd.print("%");
    lcd.setPosition(1, 13);
    if (radio.getStereoStatus() == 1)
    lcd.print("STEREO"); 
    else
    lcd.print("      ");    
}

void showFmSeek() {
  if (isFmSeekMode) {
    if (radio.seekSync(isFmSeekUp)) {
      isFmSeekMode = false;
      Serial.println("Seek stopped");
   //   lcd.setPosition(3, 1);
   //   lcd.print("Seek stopped");    
      frequency = radio.getFrequency();
      displayInfo();
    }
  }
}

