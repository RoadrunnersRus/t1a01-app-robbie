/*Copyright (c) 2010 bildr community
 Permission is hereby granted, free of charge, to any person obtaining a copy 
 of this software and associated documentation files (the "Software"), to deal
 in the Software without restriction, including without limitation the rights
 to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 copies of the Software, and to permit persons to whom the Software is
 furnished to do so, subject to the following conditions:
 The above copyright notice and this permission notice shall be included in
 all copies or substantial portions of the Software.
 THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 THE SOFTWARE.
 
 ~~~~~~ CIRCUIT ~~~~~~~
 MPR121 <-----> Photon
   3.3v         3V3
    IRQ         D2
    SCL         D1
    SDA         D0
    GND         GND
*/
                                        // ALL PERSONAL EDITS ARE SHOWN WITH ALL CAPS COMMENTS
#include "mpr121.h"
//#include <Wire.h>
int irqpin = 2;  // Digital 2
int state = 0;
int Astate = 0;




boolean touchStates[12]; //to keep track of the previous touch states

void setup(){
  pinMode(irqpin, INPUT);
  digitalWrite(irqpin, HIGH);//enable pullup resistor
                                                        // MY CODE
  pinMode (A5, OUTPUT);
  pinMode (A4, OUTPUT);
  pinMode (D6, OUTPUT);
                                                        // MY CODE ENDS
  Spark.function("my-main", myMain);    // Ajax Better
  RGB.brightness(1);                    //Ajax Better
  
  
  
  Serial.begin(9600);
  Wire.begin();

  mpr121_setup();
}

void loop(){
  readTouchInputs();
  AjaxBetter();
  delay(100);
}


void readTouchInputs(){
  if(!checkInterrupt()){
    
    //read the touch state from the MPR121
    Wire.requestFrom(0x5A,2); 
    
    byte LSB = Wire.read();
    byte MSB = Wire.read();
    
    uint16_t touched = ((MSB << 8) | LSB); //16bits that make up the touch states

    
    for (int i=0; i < 12; i++){  // Check what electrodes were pressed
      if(touched & (1<<i)){
      
        if(touchStates[i] == 0){
          //pin i was just touched
          Serial.print("pin ");
          Serial.print(i);
          Serial.println(" was just touched");
          
         // String myA0 = String(i, DEC);
         // Particle.publish("pin",myA0,60,PRIVATE);
          delay(100);
        }else if(touchStates[i] == 1){
          //pin i is still being touched
        }  
        
      if (i=1 ){
          analogWrite (A5, (Astate) ? HIGH : LOW);           // servo ON/OFF TOGGLE
          Astate = !state;                                 // TOGGLE FOR LOCK
          
      }
      
      
        
      if (i=2){
            analogWrite (A4, 255);                  // TV LINEAR SERVO GOES UP AND DOWN TO PRESS POWER BUTTON
            delay (1000);                           // OR SWINGS TO PRESS POWER BUTTON IF NORMAL SERVO
            analogWrite (A4, 0);
            
        }
        
       
      if (i=3){
          digitalWrite (D6, (state) ? HIGH : LOW);           // LED ON/OFF TOGGLE
          state = !state;
          delay (1000);
      }
          
        
        
        
        
        
        
      
        touchStates[i] = 1;      
      }else{
        if(touchStates[i] == 1){
          Serial.print("pin ");
          Serial.print(i);
          Serial.println(" is no longer being touched");
          
          //pin i is no longer being touched
       }
        
        touchStates[i] = 0;
      }
    
    }
    
  }
}

void AjaxBetter(){
    
    //PUT YOUR LOOP CODE HERE

}

int myMain(String myCode) {
    
    myCode.toUpperCase();           // set argument to uppercase--remove for better security
    
    // used send instead of write since I needed it to be 4 characters long.
    

    // d7-send-1 or d7-send-high or d7-send-on    to turn on D7
    // d7-send-0   or d7-send-low  or d7-send-off to tuurn off D7
    // d5-read    read D5

    // a0-send-0     turn A0 off
    // a0-send-255   turn A0 maximum
    // a1-read       read A1
     
    // Block sets pinNumber for digital 0-7 or analog 10-17 from the number
    int mySetWrite = 0;
    int pinNumber = myCode.charAt(1) - '0';
    if (pinNumber< 0 || pinNumber >7) return -1; 
    if (myCode.startsWith("A")){pinNumber = pinNumber+10;}  //+10 is for analog numbers
    
    
  
    String  myActivity = myCode.substring(3,7);     // take 4 characters starting at the 3rd.
     
    
    //Following sets the 7 and on characters to integers
    String myOptional = myCode.substring(8);      
    if(myOptional == "HIGH") {mySetWrite = 1;}
        else if(myOptional == "LOW") {mySetWrite = 0; }
            else if(myOptional == "ON") {mySetWrite = 1;}
                else if(myOptional == "OFF") {mySetWrite = 0; }
                   else {mySetWrite = myOptional.toInt();  }  // sets  write value
    
    
    // myCode parsing complete
    
    
    if (pinNumber < 9) {   // digital pins activated
   
        if (myActivity == "READ"){    //digital read
            pinMode(pinNumber, INPUT_PULLDOWN);
            return digitalRead(pinNumber);
        }
        
        if (myActivity == "SEND"){    //digital write
            pinMode(pinNumber, OUTPUT);
            digitalWrite(pinNumber, mySetWrite);
            return mySetWrite;
        }        
        
        
    }  else {      // analog pins activated
    
        if (myActivity == "READ"){    //Analog read
          //  pinMode(pinNumber, INPUT_PULLUP); // stangely not needed on the photon
            return analogRead(pinNumber);
        }
        
        if (myActivity == "SEND"){    //Analog Write
            pinMode(pinNumber, OUTPUT);
            analogWrite(pinNumber,  mySetWrite);
            return mySetWrite;
        }        
      }
    
}






























void mpr121_setup(void){

  set_register(0x5A, ELE_CFG, 0x00); 
  
  // Section A - Controls filtering when data is > baseline.
  set_register(0x5A, MHD_R, 0x01);
  set_register(0x5A, NHD_R, 0x01);
  set_register(0x5A, NCL_R, 0x00);
  set_register(0x5A, FDL_R, 0x00);

  // Section B - Controls filtering when data is < baseline.
  set_register(0x5A, MHD_F, 0x01);
  set_register(0x5A, NHD_F, 0x01);
  set_register(0x5A, NCL_F, 0xFF);
  set_register(0x5A, FDL_F, 0x02);
  
  // Section C - Sets touch and release thresholds for each electrode
  set_register(0x5A, ELE0_T, TOU_THRESH);
  set_register(0x5A, ELE0_R, REL_THRESH);
 
  set_register(0x5A, ELE1_T, TOU_THRESH);
  set_register(0x5A, ELE1_R, REL_THRESH);
  
  set_register(0x5A, ELE2_T, TOU_THRESH);
  set_register(0x5A, ELE2_R, REL_THRESH);
  
  set_register(0x5A, ELE3_T, TOU_THRESH);
  set_register(0x5A, ELE3_R, REL_THRESH);
  
  set_register(0x5A, ELE4_T, TOU_THRESH);
  set_register(0x5A, ELE4_R, REL_THRESH);
  
  set_register(0x5A, ELE5_T, TOU_THRESH);
  set_register(0x5A, ELE5_R, REL_THRESH);
  
  set_register(0x5A, ELE6_T, TOU_THRESH);
  set_register(0x5A, ELE6_R, REL_THRESH);
  
  set_register(0x5A, ELE7_T, TOU_THRESH);
  set_register(0x5A, ELE7_R, REL_THRESH);
  
  set_register(0x5A, ELE8_T, TOU_THRESH);
  set_register(0x5A, ELE8_R, REL_THRESH);
  
  set_register(0x5A, ELE9_T, TOU_THRESH);
  set_register(0x5A, ELE9_R, REL_THRESH);
  
  set_register(0x5A, ELE10_T, TOU_THRESH);
  set_register(0x5A, ELE10_R, REL_THRESH);
  
  set_register(0x5A, ELE11_T, TOU_THRESH);
  set_register(0x5A, ELE11_R, REL_THRESH);
  
  // Section D
  // Set the Filter Configuration
  // Set ESI2
  set_register(0x5A, FIL_CFG, 0x04);
  
  // Section E
  // Electrode Configuration
  // Set ELE_CFG to 0x00 to return to standby mode
  set_register(0x5A, ELE_CFG, 0x0C);  // Enables all 12 Electrodes
  
  
  // Section F
  // Enable Auto Config and auto Reconfig
  /*set_register(0x5A, ATO_CFG0, 0x0B);
  set_register(0x5A, ATO_CFGU, 0xC9);  // USL = (Vdd-0.7)/vdd*256 = 0xC9 @3.3V   set_register(0x5A, ATO_CFGL, 0x82);  // LSL = 0.65*USL = 0x82 @3.3V
  set_register(0x5A, ATO_CFGT, 0xB5);*/  // Target = 0.9*USL = 0xB5 @3.3V
  
  set_register(0x5A, ELE_CFG, 0x0C);
  
}


boolean checkInterrupt(void){
  return digitalRead(irqpin);
}


void set_register(int address, unsigned char r, unsigned char v){
    Wire.beginTransmission(address);
    Wire.write(r);
    Wire.write(v);
    Wire.endTransmission();
}
