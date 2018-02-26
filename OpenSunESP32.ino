// OpenSun v3 //

#include <stdlib.h>
#include <BLEDevice.h>
#include <BLEServer.h>
#include <BLEUtils.h>
#include <BLE2902.h>
#include <string>
#include "EEPROM.h"

BLECharacteristic *pCharacteristic;
BLECharacteristic *pCharacteristic2;
BLECharacteristic *pCharacteristic3;
BLECharacteristic *pCharacteristic4;

int     pinReleM1                 = 2      ;
int     pinReleM2                 = 4      ;
int     pinReleC                  = 5      ;
int     pinReleL                  = 18     ;
int     sel                       = 0      ;
int     dir_tmax                  = 0      ;
String  inputString_BT            = ""     ;
String  codigoInicio              = "info" ;
String  codigoMotorAdelante       = "am1"  ;
String  codigoMotorAtras          = "am0"  ;
String  codigoMotorAdelanteManual = "mm1"  ;
String  codigoMotorAtrasManual    = "mm0"  ;
String  codigoMotorStop           = "st"   ;
String  codigoLuzOn               = "l1"   ;
String  codigoLuzOff              = "l0"   ;
String  codigoCalefaccionOn       = "c1"   ;
String  codigoCalefaccionOff      = "c0"   ;
String  codigoPos1                = "pos1" ;
String  codigoPos2                = "pos2" ;
String  codigoPos3                = "pos3" ;
String  codigoAT                  = "AT+NAME";
boolean BTstring_Complete         = 0      ; 
boolean Serialstring_Complete     = 0      ;

// Variables de tiempo
unsigned long Tmax = 14500;
unsigned long Tmax_aux = 0;
unsigned long Tpos = 21;
unsigned long Tpos1;
unsigned long Tpos2;
unsigned long Tpos3;
unsigned long Tinicio;
unsigned long Tactual = 1;
unsigned long Taux = 0;
unsigned long Tanterior1 = 0;
unsigned long Tanterior2 = 0;

bool deviceConnected = false;
bool motor_adelante = false;
bool motor_atras = false;
bool manual = false;
bool Apos = false;
bool luz = false;
bool cal = false;
bool consola = false;
uint8_t cc = 64;

#define SERVICE_UUID           "0000ffd0-0000-1000-8000-00805f9b34fb" // UART service UUID
#define CHARACTERISTIC_UUID_RX "0000ffe1-0000-1000-8000-00805f9b34fb"
#define CHARACTERISTIC_UUID_TX_pos "0000ffd1-0000-1000-8000-00805f9b34fb"
#define CHARACTERISTIC_UUID_TX_luz "0000ffd2-0000-1000-8000-00805f9b34fb"
#define CHARACTERISTIC_UUID_TX_cal "0000ffd3-0000-1000-8000-00805f9b34fb"


class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

class MyCallbacks: public BLECharacteristicCallbacks {
    void onWrite(BLECharacteristic *pCharacteristic) {
      std::string rxValue = pCharacteristic->getValue();

      if (rxValue.length() > 0) {
        for (int i = 0; i < rxValue.length(); i++)
        {
          //Serial.print(rxValue[i]);
          inputString_BT += rxValue[i];
        }
      }
      BTstring_Complete = 1;
    }
};

void setup() {
  Serial.begin(115200);
  BLEDevice::init("OpenSun");

  // Create the BLE Server
  BLEServer *pServer = BLEDevice::createServer();
  pServer->setCallbacks(new MyServerCallbacks());

  // Create the BLE Service
  BLEService *pService = pServer->createService(SERVICE_UUID);

  // Create a BLE Characteristic
  /////////////////////////////////////////////////////////////////////////////////
 BLECharacteristic *pCharacteristic = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID_RX,
                                         BLECharacteristic::PROPERTY_WRITE |
                                         BLECharacteristic::PROPERTY_NOTIFY
                                       );
pCharacteristic->addDescriptor(new BLE2902());

pCharacteristic->setCallbacks(new MyCallbacks());
  /////////////////////////////////////////////////////////////////////////////////
  BLECharacteristic *pCharacteristic2 = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID_TX_pos,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_NOTIFY
                                       );
pCharacteristic2->addDescriptor(new BLE2902());
 /////////////////////////////////////////////////////////////////////////////////
 BLECharacteristic *pCharacteristic3 = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID_TX_cal,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_NOTIFY
                                       );
pCharacteristic3->addDescriptor(new BLE2902());                                       
 /////////////////////////////////////////////////////////////////////////////////
 BLECharacteristic *pCharacteristic4 = pService->createCharacteristic(
                                         CHARACTERISTIC_UUID_TX_luz,
                                         BLECharacteristic::PROPERTY_READ |
                                         BLECharacteristic::PROPERTY_NOTIFY
                                       );
pCharacteristic4->addDescriptor(new BLE2902());
 /////////////////////////////////////////////////////////////////////////////////

  pCharacteristic2->setValue(&cc, 1);
  pCharacteristic3->setValue(&cc, 1);
  pCharacteristic4->setValue(&cc, 1);
  
  // Start the service
  pService->start();
  
  // Start advertising
  pServer->getAdvertising()->start();
  Serial.println("Waiting a client connection to notify...");

  pinMode(pinReleM1, OUTPUT);
  pinMode(pinReleM2, OUTPUT);
  pinMode(pinReleC , OUTPUT);
  pinMode(pinReleL , OUTPUT);
  
  digitalWrite(pinReleM1, LOW);
  digitalWrite(pinReleM2, LOW);
  digitalWrite(pinReleC , LOW);
  digitalWrite(pinReleL , LOW);

}

void loop() {
 
  if (consola == true)
 {
 Serial.print(Tactual);
 Serial.print(" ");
 Serial.print(Tmax);
 Serial.print(" ");
 Serial.print(Tpos);
 Serial.print(" ");
 Serial.print(motor_adelante);
 Serial.print(" ");
 Serial.print(motor_atras);
 Serial.print(" ");
 Serial.print(Apos);
 Serial.print(" ");
 Serial.print(luz);
 Serial.print(" ");
 Serial.print(cal);
 Serial.print('\n');
 }

if (BTstring_Complete == 1)
  { 
    if (deviceConnected) {
      pCharacteristic2->setValue("1");
      pCharacteristic2->notify();
    }
    
    BTstring_Complete = 0; 
    Serial.println(inputString_BT);
    if(inputString_BT[0] == 'p')
    {
      if (inputString_BT == codigoPos1){Tpos = Tpos1;}
      if (inputString_BT == codigoPos2){Tpos = Tpos2;}
      if (inputString_BT == codigoPos3){Tpos = Tpos3;}
      Apos = true;
      if ( Tactual >= Tpos)
      {
        digitalWrite(pinReleM1, LOW);
        digitalWrite(pinReleM2, HIGH);
        motor_adelante = false;
        motor_atras = true;
        Tinicio = millis();
        }

      if ( Tactual < Tpos)
      {
        digitalWrite(pinReleM2, LOW);
        digitalWrite(pinReleM1, HIGH);
        motor_adelante = true;
        motor_atras = false;
        Tinicio = millis();
      }
    }
    
    if (inputString_BT == codigoMotorStop){sel = 1; inputString_BT = "";}
    if (inputString_BT == codigoMotorAdelante){sel = 2; inputString_BT = "";}
    if (inputString_BT == codigoMotorAtras){sel = 3; inputString_BT = "";}
    if (inputString_BT == codigoMotorAdelanteManual){sel = 4; inputString_BT = "";}
    if (inputString_BT == codigoMotorAtrasManual){sel = 5; inputString_BT = "";}
    if (inputString_BT == codigoLuzOn){sel = 6; inputString_BT = "";}
    if (inputString_BT == codigoLuzOff){sel = 7; inputString_BT = "";}
    if (inputString_BT == codigoCalefaccionOn){sel = 8; inputString_BT = "";}
    if (inputString_BT == codigoCalefaccionOff){sel = 9; inputString_BT = "";}
    
    RLcode();
    inputString_BT = ""; 
    
  }
 
 if (manual == 0)
    {
      if ((Tactual == Tmax || Tactual == 0) && (motor_adelante == true || motor_atras == true))
      {
        digitalWrite(pinReleM1, LOW);
        digitalWrite(pinReleM2, LOW);
        motor_adelante = false;
        motor_atras = false;
        Tanterior1 = 0;
        Tanterior2 = 0;
        if (Tactual == Tmax){Tactual = Tmax - 1;};
        if (Tactual == 0){Tactual = 1;};
      }
 
      if ((Tactual >= (Tpos - 20)) && (Tactual < (Tpos + 20)) && (motor_adelante == true || motor_atras == true) && Apos == true)
      {
        digitalWrite(pinReleM1, LOW);
        digitalWrite(pinReleM2, LOW);
        motor_adelante = false;
        motor_atras = false;
        Tanterior1 = 0;
        Tanterior2 = 0;
        Apos = false;
      }   
    }

  
 if (motor_adelante == true)
 {
    Tactual = Tactual + (millis() - Tinicio - Tanterior1);
    Tanterior1 = millis() - Tinicio;    
    if ((Tactual > Tmax) && manual == false) 
    {
      Tactual = Tmax;
    }
 }

 if (motor_atras == true)
 {
    Tactual = Tactual - (millis() - Tinicio - Tanterior2);
    Tanterior2 = millis() - Tinicio;
    if (Tactual > Tmax) 
    {
      Tactual = 0;
    }
 }


}

void RLcode()
{
  switch (sel){
    case 1:
      if (manual == true)
      {
        if (motor_adelante == true)
        {
          Tmax = Tactual;
          Tpos1 = Tmax / 4;
          Tpos2 = Tpos1 * 2;
          Tpos3 = Tpos1 * 3;
        }
      }
     digitalWrite(pinReleM1, LOW);
     digitalWrite(pinReleM2, LOW);
     motor_adelante = false;
     motor_atras = false;
     Tanterior1 = 0;
     Tanterior2 = 0;
     sel = 0;
    break;

  case 2:
      if(motor_atras == true)
      {
        Tanterior1 = 0;
        Tanterior2 = 0;
      }
      digitalWrite(pinReleM2, LOW);
      digitalWrite(pinReleM1, HIGH);
      motor_adelante = true;
      motor_atras = false;
      Tinicio = millis();
      Tpos = 0;
      Apos = false;
      sel = 0;
     break;

  case 3:
     if(motor_adelante == true)
      {
        Tanterior1 = 0;
        Tanterior2 = 0;
      }
      digitalWrite(pinReleM1, LOW);
      digitalWrite(pinReleM2, HIGH);
      motor_adelante = false;
      motor_atras = true;
      Tinicio = millis();
      Tpos = 0;
      Apos = false;
      sel = 0;
    break;
    
    case 4:
      digitalWrite(pinReleM2, LOW);
      digitalWrite(pinReleM1, HIGH);
      motor_adelante = true;
      motor_atras = false;
      Tinicio = millis();
      Tpos = 0;
      manual = true;
      Apos = false;
      sel = 0;
    break;

    case 5:
      digitalWrite(pinReleM1, LOW);
      digitalWrite(pinReleM2, HIGH);
      motor_atras = true;
      motor_adelante = false;
      Tinicio = millis();
      Tpos = 0;
      manual = true;
      Apos = false;
      sel = 0;
    break;
    
    case 6:
    cal = true; 
    digitalWrite(pinReleC, HIGH);
    break;
    
    case 7:
    cal = false;
    digitalWrite(pinReleC, LOW);
    sel = 0;
    break;
    
    case 8:
    luz = true; 
    digitalWrite(pinReleL, HIGH);
    sel = 0;
    break;

    case 9:
    luz = false; 
    digitalWrite(pinReleL, LOW);
    sel = 0;
    break;
   }    
}
