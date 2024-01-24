#include <SevSeg.h>
#include <toneAC.h>
#include <IRremote.hpp>
/*
    Base for IR Controlled Timer for educational purposes
    Comes with many bugs in stock
*/

const byte IR_RECEIVE_PIN = 11;
const byte SS_D1_PIN = 7;
const byte SS_A_PIN = 6;
const byte SS_F_PIN = 5;
const byte SS_D2_PIN = 4;
const byte SS_D3_PIN = 3;
const byte SS_B_PIN = 2;
const byte SS_E_PIN = A0;
const byte SS_D_PIN = A1;
const byte SS_DP_PIN = A2;
const byte SS_C_PIN = A3;
const byte SS_G_PIN = A4;
const byte SS_D4_PIN = A5;


typedef enum remote_button{
    BUT0 = 0,
    BUT1 = 1,
    BUT2 = 2,
    BUT3 = 3,
    BUT4 = 4,
    BUT5 = 5,
    BUT6 = 6,
    BUT7 = 7,
    BUT8 = 8,
    BUT9 = 9,
    BUTPOW = 10,
    BUTVOLP = 11,
    BUTFUNCSTOP = 12,
    BUTREWIND = 13,
    BUTFASTFORWARD = 14,
    BUTPAUSE = 15,
    BUTDOWN = 16,
    BUTUP = 17,
    BUTVOLM = 18,
    BUTEQ = 19,
    BUTSTS = 20,
    BUTTONHELD = 21,
    BUTTONUNKNOWN = 22
  }remote_button_t;

SevSeg sevseg;
int seconds_left = 0;
bool is_running = false;
void setup()
{
   Serial.begin(115200);
   Serial.println("Starting Final Alarm Clock");
   IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);
    byte numDigits = 4;  
    byte digitPins[] = {SS_D1_PIN, SS_D2_PIN, SS_D3_PIN, SS_D4_PIN}; // D1, D2, D3, D4 When they are low, the value on the segments is displayed
    byte segmentPins[] = {SS_A_PIN, SS_B_PIN, SS_C_PIN, SS_D_PIN, SS_E_PIN, SS_F_PIN, SS_G_PIN, SS_DP_PIN}; // DP, and segments ABCDEFG, by turning some on and others off, you can display different numbers
    bool resistorsOnSegments = 0; 
    byte hardwareConfig = COMMON_CATHODE; // You may need to change this if your display is COMMON ANODE
    bool updateWithDelays = false;
    bool leadingZeros = true;
    bool disableDecPoint = false;
    sevseg.begin(hardwareConfig, numDigits, digitPins, segmentPins, resistorsOnSegments,
    updateWithDelays, leadingZeros, disableDecPoint);
    sevseg.setBrightness(60);
    sevseg.setNumber(1234);
    
}
unsigned long lastUpdateTime = 0;
const int SECONDS_INTERVAL = 1000;  // Update every 1000 milliseconds (1 second)

void loop(){
  sevseg.refreshDisplay();
  // Check if a second has elapsed
  if (millis() - lastUpdateTime >= SECONDS_INTERVAL) {
    lastUpdateTime = millis();  // Update the last update time

    // Decrease sec_left every second
    if (is_running && seconds_left > 0) {
      seconds_left--;
      sevseg.setNumber(seconds_left);
    }
    if(seconds_left == 0 && is_running){
        toneAC(330, 10, 5000);
        is_running = 0;
    }
  }
   if (IrReceiver.decode())
   {
      byte recieved = IrReceiver.decodedIRData.command;
      remote_button_t button_pressed = decode(recieved);
      if(button_pressed == BUTEQ){
            changeTimeLeft();
        }else if(button_pressed == BUTPOW){
          sevseg.blank();
          }else if(button_pressed == BUTSTS){
            sevseg.setNumber(1234);
            }else if(button_pressed == BUTPAUSE){
              is_running =  !is_running;
              }
      IrReceiver.resume();
   }
  
   
}

void changeTimeLeft(void){
    int sec_val = 0;
    for(int i = 0; i< 4;){
      if(IrReceiver.decode()){
   
      byte recieved = IrReceiver.decodedIRData.command;
      remote_button_t button_pressed = decode(recieved);
      if(button_pressed<=BUT9 && button_pressed >= BUT0){
        if(i==0)sec_val+=button_pressed*1000;
        if(i==1)sec_val+=button_pressed*100;
        if(i==2)sec_val+=button_pressed*10;
        if(i==3)sec_val+=button_pressed;
        i++;
      }
      IrReceiver.resume();
      Serial.println(i);
      Serial.println(button_pressed);
      Serial.println(sec_val);
      }
      sevseg.setNumber(sec_val);
      sevseg.refreshDisplay();
    }
    seconds_left = sec_val;
    is_running = false;
}

remote_button_t decode(byte val){
    remote_button_t result;
    switch (val){
      case 0x0:
        result = BUTTONHELD;
        break;
      case 0x45:
        result = BUTPOW;
        break;
      case 0x46:
        result = BUTVOLP;
        break;
      case 0x47:
        result = BUTFUNCSTOP;
        break;
      case 0x44:
        result = BUTREWIND;
        break;
      case 0x40:
        result = BUTPAUSE;
        break;
      case 0x43:
        result = BUTFASTFORWARD;
        break;
      case 0x7:
        result = BUTDOWN;
        break;
      case 0x15:
        result = BUTVOLM;
        break;
      case 0x9:
        result = BUTUP;
        break;
      case 0xD:
        result = BUTSTS;
        break;
      case 0x19:
        result = BUTEQ;
        break;
      case 0x16:
        result = BUT0;
        break;
      case 0xC:
        result = BUT1;
        break;
      case 0x18:
        result = BUT2;
        break;
      case 0x5E:
        result = BUT3;
        break;
      case 0x8:
        result = BUT4;
        break;
      case 0x1C:
        result = BUT5;
        break;
      case 0x5A:
        result = BUT6;
        break;
      case 0x42:
        result = BUT7;
        break;
      case 0x52:
        result = BUT8;
        break;
      case 0x4A:
        result = BUT9;
        break;
      default:
        result = BUTTONUNKNOWN;
        Serial.println(val, HEX);
        break;
      }
    return result;
  }
