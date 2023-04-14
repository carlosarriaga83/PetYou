//-------------------------------------------------------------------------------------//
// LIBRERIAS
//-------------------------------------------------------------------------------------//

#include <Arduino.h>
#include <SoftwareSerial.h>

#include <Adafruit_NeoPixel.h>

#include <Time.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TimeLib.h>
#include "DS1307.h"
#include <Adafruit_SSD1306.h>
#include <TinyGPS.h>
#include <Wire.h>
#include <SparkFun_ADXL345.h>

#include "MPU9250.h"
#include "RTClib.h"
//#include "LowPower.h"

#include "PETYOU.h"
//#include "SIM800.h"

//-------------------------------------------------------------------------------------//
// DEFINICIONES
//-------------------------------------------------------------------------------------//


//DS1307 clock;

#define OLED_RESET -1
Adafruit_SSD1306 display(OLED_RESET);
TinyGPS TNYgps;
ADXL345 adxl = ADXL345();
int ADXL345 = 0x53;
RTC_DS1307 rtc;
//CGPRS_SIM800 SIMgprs;
Adafruit_NeoPixel strip = Adafruit_NeoPixel(1, NEO_INT_PIN, NEO_GRB + NEO_KHZ400);
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

config_t SETTINGS;

//-------------------------------------------------------------------------------------//
// VARIABLES
//-------------------------------------------------------------------------------------//



float X_out, Y_out, Z_out, Xavg, Yavg, Zavg;  // Outputs
float roll, pitch, rollF, pitchF, faceF = 0;
float a, ax, ay, az, h, hx, hy, hz, IMU_TEMP;
float VECTOR_OLD_X, VECTOR_OLD_Y, VECTOR_OLD_Z, VECTOR_NEW_X, VECTOR_NEW_Z, VECTOR_NEW_Y, VECTOR_DELTA_X, VECTOR_DELTA_Y, VECTOR_DELTA_Z;


int GPS_baud = 9600;
//int GPS_baud = 57600;
bool S_REG;
bool S_GSM;
bool S_GPS;
String C_GPS = "0"; //COMMAND GPS
bool K_GPS; //KEEP ACTIVE
bool ERR_TCP_FLAG = false; //FLAG FOR TCP CONNECTION ERRORS



int FIXING;

String R_GPS_TS, R_GPS_LAT, R_GPS_LON, R_GPS_LASTFIX, R_GPS_SAT, R_GPS_ALT, R_GPS_SPEED;
String R_GSM_TS, R_GSM_LAT, R_GSM_LON;
String R_GPS, R_BAT, R_IP, R_IMEI;
String DEV_NAME = "GPS3";

String URL = "gps2.turniot.com/php/din.php?";


String D_OUT = "";
String RAZON = "I";
String HOST_IP;
String OLED_OLD_LBL;

String PAYLOAD = "";
int TX_OK_ID = 0;
int TX_NOK_ID = 0;

bool ERR_TCP = true;
bool ERR_SEND = true;

bool FLAG_SEND_1 = false;
bool FLAG_SEND_2 = false;

bool IMU_E = false;
bool ONE_E = true;

int DIN_COUNTER;

int AN1;
int AN2;

long int R_RUNTIME;
int T_CONF = 10;
int T_STATIC = 0;
int T_DIN = 0;
int T_GPS = 1;
int T_LAST = 0;
int RETURN_TO_STATIC_COUNT = 0;
int SEND_TRY = 2;// INTENTOS ANTE FALLOS DE CONEXION
int T_FAIL_TRY = 300;// SEG ESPERA ANTE ENTRE INTENTO DE CONEXION
int FAIL_SEND = 15;// INTENTOS PARA OBTENER IP



long int TS_NOW;

long int T_AGE_TOLERANCE = 9000; //MILI SEC TOLERACIA DE AGE
int SPEED_TRIGGER = 2; // SPEED TRIGGER T_DIN
int COUNTER_DIN = 0;


float  VECTOR_TOLERANCE = 1500;

String IMEI;
String SENAL;

int CORE_BAT;

const char compile_date[] = __DATE__ " " __TIME__;

//-------------------------------------------------------------------------------------//
// PUERTOS
//-------------------------------------------------------------------------------------//

#define MODEM_PUERTO Serial1
#define BT_PUERTO    Serial
SoftwareSerial mySerial(CON_RX_PIN, CON_TX_PIN); // RX , TX
#define CON_PUERTO mySerial

//-------------------------------------------------------------------------------------//
// INSTANCIAS / BOARDS
//-------------------------------------------------------------------------------------//

BOARD PCB;



//-------------------------------------------------------------------------------------//
// SETUP
//-------------------------------------------------------------------------------------//


void setup() {


  // DISABLE JTAG
  uint8_t tmp = 1 << JTD; // Disable JTAG SUPER IMPORTANTE PARA QUE EL PIN 18 FUNCIONE COMO OUTPUT HACIA EL DTR DEL MODEM!!!
  MCUCR = tmp; // Disable JTAG
  MCUCR = tmp; // Disable JTAG

  PCB.LEVEL = 0;

  MODEM_PUERTO.begin(38400);
  BT_PUERTO.begin(9600);
  CON_PUERTO.begin(38400);


  PCB.BT_BEGIN(BT_PUERTO);
  PCB.MODEM_BEGIN(MODEM_PUERTO);
  PCB.CON_BEGIN(CON_PUERTO);

  PCB.PRINT( 0, "" , "" , true);
  PCB.PRINT( 0, "" , "=================================================================" , true);
  PCB.PRINT( 0, "COMPILED ON" ,  String(compile_date) , true);
  PCB.PRINT( 0, "" , "=================================================================" , true);

  


  SETTINGS.PAUSE = 0;
  SETTINGS.T_GPS = 0;
  SETTINGS.T_DIN = 0;
  SETTINGS.T_STATIC = 0;
  SETTINGS.VECTOR_TOLERANCE = 1500;
  SETTINGS.OLED = 0;
  SETTINGS.E_GPS = 0;

  EEPROM_WRITE(1, SETTINGS);




  pinMode(GSM_DTR_PIN, OUTPUT); digitalWrite(GSM_DTR_PIN, LOW);// LOW = module can quit sleep mode.

  PCB.LEVEL = 0;
  PCB.LEVEL++;

  PCB.PRINT( 0, "" , "STARTING MODEM" + PCB.SEPARATOR , true);
  PCB.GSM_ENSURE_PWR(true);
  PCB.PRINT( 0, "", "INITIAL CONFIG MODEM" + PCB.SEPARATOR , true);
  PCB.GSM_SETUP();


}




//-------------------------------------------------------------------------------------//
// LOOP
//-------------------------------------------------------------------------------------//

void loop() {

  PCB.LEVEL = 0;
  PCB.LEVEL++;

  PCB.PRINT( 0, "", "STARTING MODEM" + PCB.SEPARATOR , true);

  PCB.GSM_ENSURE_PWR(true);

  PCB.GSM_ENSURE_ONLINE();

  PCB.GSM_IMEI();

  PCB.PRINT( 0, "", "SENDING TCP MODE" + PCB.SEPARATOR , true);

  PCB.GSM_SOLVE_DNS();

  PCB.GSM_TCP_CLOSE();

  PCB.GSM_TCP_CONNECT();

  PCB.GSM_TCP_SEND("device=" + PCB.GSM_IMEI());

  PCB.GSM_TCP_CLOSE();


  PCB.PRINT( 0, "", "SENDING HTTP MODE" + PCB.SEPARATOR , true);

  PCB.GSM_HTTP_ON();

  PCB.GSM_HTTP_STATUS();

  PCB.GSM_HTTP_OFF();

  PCB.PRINT( 0, "", "SHUTING DOWN MODEM" + PCB.SEPARATOR , true);

  PCB.GSM_ENSURE_PWR(false);

  delay(20000);





}





//-------------------------------------------------------------------------------------//
// RUTINAS EEPROM
//-------------------------------------------------------------------------------------//

template <class T> int EEPROM_WRITE(int ee, const T& value) {
  const byte* p = (const byte*)(const void*)&value;
  unsigned int i;
  for (i = 0; i < sizeof(value); i++)
    EEPROM.write(ee++, *p++);
  return i;
}

template <class T> int EEPROM_READ(int ee, T& value) {
  byte* p = (byte*)(void*)&value;
  unsigned int i;
  for (i = 0; i < sizeof(value); i++)
    *p++ = EEPROM.read(ee++);
  return i;
}
