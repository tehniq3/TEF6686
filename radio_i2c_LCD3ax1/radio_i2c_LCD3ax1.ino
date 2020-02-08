/*
 * original sketch from https://github.com/makserge/tef6686_radio
 * adapted for i2c display and phisical buttons by Nicu FLORICA (niq_ro)
 * http://www.arduinotehniq.com/
 * 
 */
#include <Wire.h> 
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x3F,20,4);  // set the LCD address to 0x3F (0x27) for a 16 chars and 2 line display

#include "TEF6686.h"

uint16_t frequency;
int volume;
int volume2;
byte plus = 0;

bool isRdsSync;  
;

char programTypePrevious[17] = "                ";
char programServicePrevious[9];
char programServiceUnsafePrevious[9];
char programIdPrevious[4];
char radioTextPrevious[65];

TEF6686 radio;
RdsInfo rdsInfo;

#define volumplus 4
#define volumminus 5
#define cautareplus 6
#define cautareminus 7
#define inainte 8
#define inapoi 9


byte antena[] = {   // https://maxpromer.github.io/LCD-Character-Creator/
  B10001,
  B10101,
  B10101,
  B01110,
  B00100,
  B00100,
  B00100,
  B00100
};

/*
byte antena[] = {
  B11111,
  B10101,
  B10101,
  B01110,
  B01110,
  B00100,
  B00100,
  B00100
};
*/

byte difuzor[] = {
  B00001,
  B00001,
  B00011,
  B11101,
  B10101,
  B11101,
  B00011,
  B00001
};

void setup() {
  pinMode(volumminus, INPUT);
  pinMode(volumplus, INPUT);
  pinMode(cautareminus, INPUT);
  pinMode(cautareplus, INPUT);
  pinMode(inainte, INPUT);
  pinMode(inapoi, INPUT); 
  digitalWrite(volumminus, HIGH);
  digitalWrite(volumplus, HIGH);
  digitalWrite(cautareminus, HIGH);
  digitalWrite(cautareplus, HIGH);  
  digitalWrite(inainte, HIGH);
  digitalWrite(inapoi, HIGH); 
  
  //lcd.init();                      // initialize the lcd 
  lcd.begin(); // initialize the LCD
  lcd.createChar(0, antena);
  lcd.createChar(1, difuzor);
  // Print a message to the LCD.
  lcd.backlight();
  lcd.clear(); 
  // Print a logo message to the LCD.
  lcd.setCursor(0, 0);
  lcd.print(" TEF6686 radio v.3x1");
  lcd.setCursor(0, 1);
  lcd.print("github.com/makserge ");
  lcd.setCursor(0, 2);
  lcd.print(" updated by  niq_ro ");
  lcd.setCursor(0, 3);
  lcd.print(" github.com/tehniq3 ");
  delay (3000);
  lcd.clear();
  
  delay(1000);
  Serial.begin(115200);
  while (!Serial);
  Serial.println(F("Start"));
  radio.init();
  radio.powerOn();
  radio.setFrequency(9200);
  frequency = radio.getFrequency();
  //displayInfo();
  radioGui(1);

}

void loop() {
  readRds();
  radioGui(0);
  displayInfo();

    if (digitalRead(volumplus) == LOW)
    {
      volume += 4;
      if (volume >= 24) volume = 24;
      radio.setVolume(volume);
      displayInfo();
      delay(250);
    } 
    if (digitalRead(volumminus) == LOW)
    {
      volume -= 4;
      if (volume < -60) volume = -60;
      radio.setVolume(volume);
      displayInfo();
      delay(250);
    } 
    if (digitalRead(cautareplus) == LOW)
    {
      frequency = radio.seekUp();
      displayInfo();
      delay(250);
    }
    if (digitalRead(cautareminus) == LOW)
    {
      frequency = radio.seekDown();
      displayInfo();
      delay(250);
    }

    if (digitalRead(inainte) == LOW)
    {
      frequency = radio.tuneUp();
      displayInfo();
      delay(250);
    }
   if (digitalRead(inapoi) == LOW)
    {
      frequency = radio.tuneDown();
      displayInfo();
      delay(250);
    }


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
 lcd.setCursor(3, 0);
 lcd.print("                    ");    
 lcd.setCursor(4, 0);
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
  lcd.setCursor(0,3);
   lcd.print("Pi: ");
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
    lcd.setCursor(0,2);
    lcd.print("PTY:");
      for (int i = 0; i < 15; i++) 
     {
      Serial.print(radioTextPrevious[i]);
      lcd.setCursor(i+4,2);
      lcd.print(rdsInfo.programType[i]);   
     }
  //  lcd.print(rdsInfo.programType); 
  }  

bool isNewPty() {
  return (isRdsSync == 1) && !strcmp(rdsInfo.programType, programTypePrevious, 16);
}


void showPS() {
  if (isNewPs()) {
    strcpy(programServicePrevious, rdsInfo.programService);
  }
  Serial.print(rdsInfo.programService);
//  lcd.setCursor(0,3);
//  lcd.print(rdsInfo.programService);  
}


void showPsUnsafe(){
  if (isNewPsUnsafe()){
    strcpy(programServiceUnsafePrevious, rdsInfo.programServiceUnsafe);
  }
  Serial.print(rdsInfo.programServiceUnsafe);
  lcd.setCursor(10,3);
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
 //   lcd.setCursor(10,3);
  //  lcd.print(rdsInfo.radioText); 
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
    lcd.setCursor(0, 0);
    if (frequency < 10000) lcd.print(" ");
    lcd.print(frequency/100); 
    lcd.print(",");
    lcd.print(frequency%100/10); // thanks to carkiller08
    lcd.print(frequency%10);
    lcd.print("MHz");
    lcd.setCursor(0,1);
//    lcd.print("vol.");
    lcd.write(1);  // speaker sign
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
    lcd.setCursor(13,0);
 //   lcd.print("level:");
    lcd.write(0);  // antenna sign
    if (radio.getLevel() < 100)
    lcd.print(" "); 
    if (radio.getLevel() < 10)
    lcd.print(" ");      
    lcd.print(radio.getLevel());
    lcd.print("%");
    lcd.setCursor(13,1);
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
//    showPS();
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
