#include "TEF6686.h"

// https://en.wikipedia.org/wiki/Radio_Data_System#Program_types
/*
char* ptyLUT[51] = {      // America
      "      None      ",
      "      News      ",
      "  Information   ",
      "     Sports     ",
      "      Talk      ",
      "      Rock      ",
      "  Classic Rock  ",
      "   Adult Hits   ",
      "   Soft Rock    ",
      "     Top 40     ",
      "    Country     ",
      "     Oldies     ",
      "      Soft      ",
      "   Nostalgia    ",
      "      Jazz      ",
      "   Classical    ",
      "Rhythm and Blues",
      "   Soft R & B   ",
      "Foreign Language",
      "Religious Music ",
      " Religious Talk ",
      "  Personality   ",
      "     Public     ",
      "    College     ",
      " Reserved  -24- ",
      " Reserved  -25- ",
      " Reserved  -26- ",
      " Reserved  -27- ",
      " Reserved  -28- ",
      "     Weather    ",
      " Emergency Test ",
      "  !!!ALERT!!!   ",
      "Current Affairs ",
      "   Education    ",
      "     Drama      ",
      "    Cultures    ",
      "    Science     ",
      " Varied Speech  ",
      " Easy Listening ",
      " Light Classics ",
      "Serious Classics",
      "  Other Music   ",
      "    Finance     ",
      "Children's Progs",
      " Social Affairs ",
      "    Phone In    ",
      "Travel & Touring",
      "Leisure & Hobby ",
      " National Music ",
      "   Folk Music   ",
      "  Documentary   "};
*/

char* ptyLUT[51] = {   // Europe
      "      None      ",
      "      News      ",
      " Current Affairs",
      "   Information  ",
      "      Sport     ",
      "    Education   ",
      "     Drama      ",
      "     Culture    ",
      "     Science    ",
      "    Variable    ",
      "    Pop Music   ",
      "   Rock Music   ",
      "  Easy Listening",
      " Light Classical",
      "SeriousClassical",
      "  Other Music   ",
      "     Weather    ",
      "     Finance    ",
      " Childrens Prog ",
      " Social Affairs ",
      " Religious Talk ",
      "  Phone-In Talk ",
      "     Travel     ",
      "    Leisure     ",
      "   Jazz Music   ",
      "  Country Music ",
      "  National Music",
      "  Oldies Music  ",
      "   Folk Music   ",
      "   Documentary  ",
      " Emergency Test ",
      "  !!!ALERT!!!   ",
      "Current Affairs ",
      "   Education    ",
      "     Drama      ",
      "    Cultures    ",
      "    Science     ",
      " Varied Speech  ",
      " Easy Listening ",
      " Light Classics ",
      "Serious Classics",
      "  Other Music   ",
      "    Finance     ",
      "Children's Progs",
      " Social Affairs ",
      "    Phone In    ",
      "Travel & Touring",
      "Leisure & Hobby ",
      " National Music ",
      "   Folk Music   ",
      "  Documentary   "};

TEF6686::TEF6686() {
}

uint8_t TEF6686::init() {
  uint8_t result;
  uint8_t counter = 0;
  uint8_t status;

  Tuner_I2C_Init();
  
  delay(5);
  while (true) {
    result = devTEF668x_APPL_Get_Operation_Status(&status);
    if (result == 1) {
      Tuner_Init();
      powerOff();
      return 1; //Ok
    }
    else if (++counter > 50) {
      return 2; //Doesn't exist
    }
    else {
      delay(5);
      return 0;  //Busy
    }
  } 
}

void TEF6686::powerOn() {
  devTEF668x_APPL_Set_OperationMode(0);
}

void TEF6686::powerOff() {
  devTEF668x_APPL_Set_OperationMode(1);
}

void TEF6686::setFrequency(uint16_t frequency) {
  Radio_SetFreq(Radio_PRESETMODE, FM1_BAND, frequency);
}

uint16_t TEF6686::getFrequency() {
  return Radio_GetCurrentFreq();
}

uint16_t TEF6686::getLevel() {
  return Radio_Get_Level(1);
}

uint8_t TEF6686::getStereoStatus() {
  return Radio_CheckStereo();
}

uint16_t TEF6686::seekUp() {
  clearRDS();
	return seek(1);
}

uint16_t TEF6686::seekDown() {
  clearRDS();
	return seek(0);
}

uint16_t TEF6686::tuneUp() {
  clearRDS();
  return tune(1);
}

uint16_t TEF6686::tuneDown() {
  clearRDS();
  return tune(0);
}

void TEF6686::setVolume(uint16_t volume) {
  devTEF668x_Audio_Set_Volume(volume);
}

void TEF6686::setMute() {
  devTEF668x_Audio_Set_Mute(1);
}

void TEF6686::setUnMute() {
  devTEF668x_Audio_Set_Mute(0);
}

bool TEF6686::readRDS() {
  char status;
  uint8_t rdsBHigh, rdsBLow, rdsCHigh, rdsCLow, rdsDHigh, rdsDLow, isPsReady;

  uint16_t rdsStat, rdsA, rdsB, rdsC, rdsD, rdsErr;
  uint16_t result = devTEF668x_Radio_Get_RDS_Data(1, &rdsStat, &rdsA, &rdsB, &rdsC, &rdsD, &rdsErr);

  // status
  // dataAvailable: is there any usable data
  // dataLoss: 0: prevoius data was read
  // dataType: 1: first PI appeared after sync
  // groupVersion: 0: A, 1: B
  // sync: is RDS sync available
  bool dataAvailable = bitRead(rdsStat, 15); 
  bool dataLoss = bitRead(rdsStat, 14);
  bool dataType = bitRead(rdsStat, 13);
  bool groupVersion = bitRead(rdsStat, 12);
  bool sync = bitRead(rdsStat, 9);

  if (!dataAvailable) {
    return sync;
  }

  // error
  uint8_t errA = (rdsErr & 0b1100000000000000) >> 14;
  uint8_t errB = (rdsErr & 0b0011000000000000) >> 12;
  uint8_t errC = (rdsErr & 0b0000110000000000) >> 10;
  uint8_t errD = (rdsErr & 0b0000001100000000) >> 8;
/*
  Serial.print("result:");
  Serial.print(result);
  Serial.println();
  
  Serial.print("data available:");
  Serial.print(dataAvailable);
  Serial.println();

  Serial.print("data loss:");
  Serial.print(dataLoss);
  Serial.println();

  Serial.print("data available type:");
  Serial.print(dataType);
  Serial.println();

  Serial.print("group version:");
  Serial.print(groupVersion);
  Serial.println();

  Serial.print("sync status:");
  Serial.print(sync);
  Serial.println();
*/


  rdsBHigh = (uint8_t)(rdsB >> 8);
  rdsBLow = (uint8_t)rdsB;
  rdsCHigh = (uint8_t)(rdsC >> 8);
  rdsCLow = (uint8_t)rdsC;
  rdsDHigh = (uint8_t)(rdsD >> 8);
  rdsDLow = (uint8_t)rdsD;

  // group type (0: PS, 2: RT, 4: CT, 10: PTYN
  uint8_t type = (rdsBHigh >> 4) & 15;

  // PTY
  if (errB <= 1) {
    uint8_t programType = ((rdsBHigh & 3) << 3) | ((rdsBLow >> 5) & 7);
    strcpy(rdsProgramType, (programType >= 0 && programType < 32) ? ptyLUT[programType] : "    PTY ERROR   ");
  }

  // PI
  char Hex[16]={ '0','1','2','3','4','5','6','7','8','9','A','B','C','D','E','F'};
  rdsProgramId[0]=Hex[(rdsA & 0xF000U) >> 12];
  rdsProgramId[1]=Hex[(rdsA & 0x0F00U) >> 8];
  rdsProgramId[2]=Hex[(rdsA & 0x00F0U) >> 4];
  rdsProgramId[3]=Hex[(rdsA & 0x000FU)];
  rdsProgramId[4]='\0';


  /*
  if (groupVersion == 1){
    Serial.print("error A:");
    Serial.print(errA);
    Serial.println();
  
    Serial.print("error B:");
    Serial.print(errB);
    Serial.println();
  
    Serial.print("error C:");
    Serial.print(errC);
    Serial.println();
  
    Serial.print("error D:");
    Serial.print(errD);
    Serial.println();
    
    Serial.print(Hex[(rdsA & 0xF000U) >> 12]);
    Serial.print(Hex[(rdsA & 0x0F00U) >> 8]);
    Serial.print(Hex[(rdsA & 0x00F0U) >> 4]);
    Serial.print(Hex[(rdsA & 0x000FU)]);    
    Serial.print(" - ");
    Serial.print(Hex[(rdsC & 0xF000U) >> 12]);
    Serial.print(Hex[(rdsC & 0x0F00U) >> 8]);
    Serial.print(Hex[(rdsC & 0x00F0U) >> 4]);
    Serial.print(Hex[(rdsC & 0x000FU)]);
    Serial.println();
  }

  Serial.print("group type number: ");
  Serial.print(type);
  Serial.println();

  if (type == 0){
    Serial.print(char(rdsDHigh));
    Serial.print(char(rdsDLow));
    Serial.println();
  }
*/

  // Groups 0A & 0B
  // Basic tuning and switching information only
   if (type == 0) {
    // Safe PS
    uint8_t address = rdsBLow & 3;
    uint8_t errPs = errB > errD ? errB : errD;
    if (address >= 0 && address <= 3) {
      if (address < prevAddress) {
        psErrors = psErrors | 0x44444444;
        psAB = !psAB;
      }
      prevAddress = address;
      // store ps block if: no stored ps char yet || arriving data's error <= 1 || previously set data's error >= 2
      if (!bitRead(psCharIsSet, 7 - (psAB * 4 + address))  || errPs <= 1 || (bitRead(psErrors, 31 - (psAB * 16 + address * 4 + 2)) && !bitRead(psErrors, 31 - (psAB * 16 + address * 4)))) {
        //psCharIsSet set
        bitWrite(psCharIsSet, 7 - (psAB * 4 + address), 1);
        
        // setting psErrors 
        bitWrite(psErrors, 31 - (psAB * 16 + address * 4), 0);
        bitWrite(psErrors, 31 - (psAB * 16 + address * 4 + 1), 0);
        if (errPs <= 1) {
          bitWrite(psErrors, 31 - (psAB * 16 + address * 4 + 2), 0);
          if (errPs == 1) {
            bitWrite(psErrors, 31 - (psAB * 16 + address * 4 + 3), 1);
          }
        }
        if (errPs == 0 || errPs == 2) {
          bitWrite(psErrors, 31 - (psAB * 16 + address * 4 + 3), 0);
        }

        // SAFE MODE
        if (rdsDHigh != '\0') {
          unsafePs[psAB][address * 2] = rdsDHigh;
        }  
        if (rdsDLow != '\0') {
          unsafePs[psAB][address * 2 + 1] = rdsDLow;
        }

        // UNSAFE MODE 
        if (errPs <= 1) {
          if (rdsDHigh != '\0') {
            rdsProgramServiceUnsafe[address * 2] = rdsDHigh;
          }  
          if (rdsDLow != '\0') {
            rdsProgramServiceUnsafe[address * 2 + 1] = rdsDLow;
          }
          // TODO: escape only rdsDHigh + rdsDLow
          rdsFormatString(rdsProgramServiceUnsafe, 8);
        }
      }    

      //check if current unsafePs can be written to safe PS
      if ((psCharIsSet == 0xFF && strncmp(unsafePs[0], unsafePs[1], 8) == 0)  || (psAB ? (psErrors & 0xFFFF) == 0 : (psErrors & 0xFFFF0000) == 0)) {
        strncpy(rdsProgramService, unsafePs[psAB], 8);
        rdsProgramService[8] = '\0';
        rdsFormatString(rdsProgramService, 8);
        psCharIsSet = 0;
        psErrors = 0xFFFFFFFF;
      }
    } 
  }
   
  // Groups 2A & 2B
  // Radio Text
  else if (type == 2 && errB <= 1) {
    uint16_t addressRT = rdsBLow & 15;
    uint8_t ab = bitRead(rdsBLow, 4);
    uint8_t cr = 0;
    uint8_t len = 64;
    if (groupVersion == 0) {
      if (addressRT >= 0 && addressRT <= 15) {
        if (errC <=1) {
          if (rdsCHigh != 0x0D) {
            rdsRadioText[addressRT*4] = rdsCHigh;
          }  
          else {
            len = addressRT * 4;
            cr = 1;
          }
          if (rdsCLow != 0x0D) {
            rdsRadioText[addressRT * 4 + 1] = rdsCLow;
          }  
          else {
            len = addressRT * 4 + 1;
            cr = 1;
          }
        }
        else if (errD <= 1) {
          if (rdsDHigh != 0x0D) {
            rdsRadioText[addressRT * 4 + 2] = rdsDHigh;
          }  
          else {
            len = addressRT * 4 + 2;
            cr = 1;
          }
          if (rdsDLow != 0x0D) {
            rdsRadioText[addressRT * 4 + 3] = rdsDLow;
          }
          else {
            len = addressRT * 4 + 3;
            cr = 1;
          }
        }
      }
    }
    else if (errD <= 1) {
      if (addressRT >= 0 && addressRT <= 7) {
        if (rdsDHigh != '\0') {
          rdsRadioText[addressRT * 2] = rdsDHigh;
        }  
        if (rdsDLow != '\0') {
          rdsRadioText[addressRT * 2 + 1] = rdsDLow;
        }
      }
    }
    if (cr) {
      for (uint8_t i = len; i < 64; i++) {
        rdsRadioText[i] = ' ';
      }
    }
    if (ab != rdsAb) {      
      for (uint8_t i = 0; i < 64; i++) {
        rdsRadioText[i] = ' ';
      }
      rdsRadioText[64] = '\0';     
      isRdsNewRadioText = 1;
    }
    else {
      isRdsNewRadioText = 0;
    }
    rdsAb = ab;
    rdsFormatString(rdsRadioText, 64);
  }
  return true; 
}
 
void TEF6686::getRDS(RdsInfo* rdsInfo) {
  strcpy(rdsInfo->programType, rdsProgramType);
  strcpy(rdsInfo->programId, rdsProgramId);
  strcpy(rdsInfo->programService, rdsProgramService);
  strcpy(rdsInfo->programServiceUnsafe, rdsProgramServiceUnsafe);
  strcpy(rdsInfo->radioText, rdsRadioText);
}

void TEF6686::clearRDS() {
  strcpy(rdsProgramType, "     No PTY     ");
  strcpy(rdsProgramId, "0000");
  strcpy(rdsProgramService, "        ");
  strcpy(rdsProgramServiceUnsafe, "        ");
  strcpy(rdsRadioText, "No RT");
  psErrors = 0xFFFFFFFF;
  psCharIsSet = 0;
}

void TEF6686::rdsFormatString(char* str, uint16_t length) {  
  for (uint16_t i = 0; i < length; i++) {    
    if ((str[i] != 0 && str[i] < 32) || str[i] > 126 ) {
      str[i] = ' ';  
    }
  }
}

uint16_t TEF6686::seek(uint8_t up) {
  uint16_t mode = 20;
  uint16_t startFrequency = Radio_GetCurrentFreq();

  while (true) {
    switch(mode){
      case 20:
        Radio_ChangeFreqOneStep(up);
        Radio_SetFreq(Radio_SEARCHMODE, Radio_GetCurrentBand(), Radio_GetCurrentFreq());
      
        mode = 30;
        Radio_CheckStationInit();
        Radio_ClearCurrentStation();
        
        break;
      
      case 30:
        delay(20);
        Radio_CheckStation();
        if (Radio_CheckStationStatus() >= NO_STATION) {
          mode = 40;
        }   
        
        break;

      case 40:
        if (Radio_CheckStationStatus() == NO_STATION) {        
          mode = (startFrequency == Radio_GetCurrentFreq()) ? 50 : 20;
        }
        else if (Radio_CheckStationStatus() == PRESENT_STATION) {
          mode = 50;
        }
        
        break;
      
      case 50:
        Radio_SetFreq(Radio_PRESETMODE, Radio_GetCurrentBand(), Radio_GetCurrentFreq());
        return Radio_GetCurrentFreq();
    }
  }
  return 0;
}

uint16_t TEF6686::tune(uint8_t up) {
  Radio_ChangeFreqOneStep(up);

  Radio_SetFreq(Radio_PRESETMODE, Radio_GetCurrentBand(), Radio_GetCurrentFreq());
  Radio_ClearCurrentStation();
  return Radio_GetCurrentFreq();
}
