#include <MsTimer2.h>
#include <Wire.h>


const int SLAVE_ADDRESS = 0x14;
const char COMM_RELAY = 120;//'c'
volatile char comm;
volatile bool blink = false;

int pin_pul = 8;
int pin_dir = 9;
int pin_connectA = 10;
int pin_connectB = 11;
bool connectAB=false;
const int tempTimer = 100;//период таймера, мс.
volatile int puls = 0;// <0 : влево, >0 : вправо, =0 не поворачиваем.
volatile bool hl = true;

void setup() {
  //-------------pin driver and relay------------------
  pinMode(pin_pul, OUTPUT);
  pinMode(pin_dir, OUTPUT);
  pinMode(pin_connectA, OUTPUT);
  pinMode(pin_connectB, OUTPUT);
  digitalWrite(pin_connectA, HIGH);
  digitalWrite(pin_connectB, HIGH);

  //-------------Timer----------------------------------
  MsTimer2::set(tempTimer, timerInterupt); // задаем период(tempTimer) прерывания по таймеру
  MsTimer2::start(); // разрешаем прерывание по таймеру

  //-------------test blink-----------------------------
  pinMode(LED_BUILTIN, OUTPUT);//test

  //-------------I2C slave init-------------------------
  Wire.begin(SLAVE_ADDRESS);
  Wire.onReceive(processMessage);
  Wire.onRequest(processAnsver);
}

void loop() {
  //================================================
  if (blink) {
    digitalWrite(LED_BUILTIN, HIGH);//////////////
  }
  else {
    digitalWrite(LED_BUILTIN, LOW);///////////////
  }
  //================================================
}

void timerInterupt() {
  if (puls != 0) {
    if (puls < 0) {
      digitalWrite(pin_dir, HIGH);
    if (hl)puls++;
    } else {
      digitalWrite(pin_dir, LOW);
      if (hl)puls--;
    }

    if (hl) {
      digitalWrite(pin_pul, HIGH);
    }
    else {
      digitalWrite(pin_pul, LOW);
    }

    hl = !hl;

//==================================
    //blink = !blink;
//==================================
  }
}

void processMessage(int n) {
  if(Wire.available()) {
    comm = Wire.read();
    if (comm == COMM_RELAY)
    {
      connectAB=!connectAB;
      if(connectAB) {
        digitalWrite(pin_connectA, LOW);
        digitalWrite(pin_connectB, LOW);

        blink = true;/////////////////////////////////
      } else {
        digitalWrite(pin_connectA, HIGH);
        digitalWrite(pin_connectB, HIGH);
        
        blink = false;////////////////////////////////
      }
    } else if (comm == 'g') {
      puls+=50;
    } else if (comm == 'f') {
      puls-=50;
    }
  }
}

void processAnsver() {
  //if(comm == 'g') {
    //Wire.write(puls);
  //}
  //if(comm == 'f') {
    //Wire.write(puls);
  //}
  
}
